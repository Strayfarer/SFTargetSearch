// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TargetCategoryTags.h"
#include "TargetQueryResult.h"
#include "TargetSearchSubsystem.generated.h"

namespace SF
{
	class UTargetQueryDataAsset;

	USTRUCT()
	struct FTargetArray
	{
		GENERATED_BODY()
		
		UPROPERTY()
		TArray<TObjectPtr<UObject>> Targets = {};
	};

	USTRUCT()
	struct FQueryResultsCache
	{
		GENERATED_BODY()
		
		UPROPERTY()
		TMap<TObjectPtr<UTargetQueryDataAsset>, FTargetQueryResult> Map;
	};

	/**
	 * Run target queries using this subsystem.
	 * Queries are run only on a subset of objects, that register themselves as targets.
	 */
	UCLASS(BlueprintType, ClassGroup="Target Search")
	class SFTARGETSEARCH_API UTargetSearchSubsystem : public UWorldSubsystem
	{
		GENERATED_BODY()

	public:
		/** Retrieve the target search subsystem. */
		static UTargetSearchSubsystem* Get(const UObject& WorldContext);

		// USubsystem
		virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
		// --

		/**
		 * Run a query with the given instigator.
		 * 
		 * Note that the instigator will be passed to the conditional tree and may be used there.
		 * Returns a report on all eligible target candidates and a potential best one.
		 */
		UFUNCTION(BlueprintCallable)
		FTargetQueryResult RunQuery(UObject* Instigator, UTargetQueryDataAsset* Query);

		/** Register an object as a target candidate. */
		UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="TargetCategory", Categories="TargetCategory", TargetCategory="TargetCategory.Default"))
		void RegisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory);

		/** Deregister an object as a target candidate. */
		UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="TargetCategory", Categories="TargetCategory", TargetCategory="TargetCategory.Default"))
		void DeregisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory);

		/** Whether the given object is registered as a target candidate under the given category. */
		UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="TargetCategory", Categories="TargetCategory", TargetCategory="TargetCategory.Default"))
		bool IsTargetRegistered(UObject* Object, const FGameplayTag& TargetCategory);

		const FTargetQueryResult* FindQueryResultCache(UObject* Instigator, UTargetQueryDataAsset* Query) const;

		FORCEINLINE const TMap<TObjectPtr<UObject>, FQueryResultsCache>& GetQueryResultByInstigatorCache() const
		{
			return QueryResultByInstigatorCache;
		}

#if WITH_GAMEPLAY_DEBUGGER
		FORCEINLINE UObject* GetLastInstigator() const { return LastInstigator.Get(); }
		FORCEINLINE UTargetQueryDataAsset* GetLastQuery() const { return LastQuery.Get(); }
#endif

	protected:
		void CacheQueryResult(const FTargetQueryResult& Result);

		bool TryRetrieveRelevantTargets(UObject* Instigator, const UTargetQueryDataAsset* Query, TArray<UObject*>& OutCandidates);

		UPROPERTY()
		TMap<FGameplayTag, FTargetArray> RegisteredTargetCandidates = {};

		UPROPERTY()
		TMap<TObjectPtr<UObject>, FQueryResultsCache> QueryResultByInstigatorCache = {};

		TWeakObjectPtr<UObject> LastInstigator;
		TWeakObjectPtr<UTargetQueryDataAsset> LastQuery;
	};
}
