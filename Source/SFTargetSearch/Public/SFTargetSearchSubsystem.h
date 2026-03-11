// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SFTargetQueryResult.h"
#include "SFTargetSearchSubsystem.generated.h"

class USFTargetQueryDataAsset;

USTRUCT() struct FSFTargetArray
{
	GENERATED_BODY()
	UPROPERTY() TArray<TObjectPtr<UObject>> Targets = {};
};

USTRUCT() struct FSFQueryResultsCache
{
	GENERATED_BODY()
	UPROPERTY() TMap<TObjectPtr<USFTargetQueryDataAsset>, FSFTargetQueryResult> Map;
};

/**
 * Run target queries using this subsystem.
 * Queries are run only on a subset of objects, that register themselves as targets.
 */
UCLASS(BlueprintType)
class SFTARGETSEARCH_API USFTargetSearchSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Retrieve the target search subsystem. */
	static USFTargetSearchSubsystem* Get(const UObject& WorldContext);

	// - USubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// --
	
	/**
	 * Run a query with the given instigator.
	 * 
	 * Note that the instigator will be passed to the conditional tree and may be used there.
	 * Returns a report on all eligible target candidates and a potential best one.
	 */
	UFUNCTION(BlueprintCallable)
	FSFTargetQueryResult RunQuery(UObject* Instigator, USFTargetQueryDataAsset* Query);

	/** Register an object as a target candidate. */
	UFUNCTION(BlueprintCallable)
	void RegisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory);

	/** Deregister an object as a target candidate. */
	UFUNCTION(BlueprintCallable)
	void DeregisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory);
	
	bool IsTargetRegistered(UObject* Target, const FGameplayTag& TargetCategory);
	
	const FSFTargetQueryResult* FindCachedQueryResult(UObject* Instigator, USFTargetQueryDataAsset* Query) const;

	FORCEINLINE const TMap<TObjectPtr<UObject>, FSFQueryResultsCache>& GetQueryResultByInstigatorCache() const { return QueryResultByInstigatorCache; }
	
#if WITH_GAMEPLAY_DEBUGGER
	FORCEINLINE UObject* GetLastInstigator() const { return LastInstigator.Get(); }
	FORCEINLINE USFTargetQueryDataAsset* GetLastQuery() const { return LastQuery.Get(); }
#endif
	
protected:
	void CacheQueryResult(const FSFTargetQueryResult& Result);
	
	bool TryRetrieveRelevantCandidates(UObject* Instigator, const USFTargetQueryDataAsset* Query, TArray<UObject*>& OutCandidates);
	
	UPROPERTY()
	TMap<FGameplayTag, FSFTargetArray> RegisteredTargetsByCategory = {};
	
	UPROPERTY()
	TMap<TObjectPtr<UObject>, FSFQueryResultsCache> QueryResultByInstigatorCache = {};

	TWeakObjectPtr<UObject> LastInstigator;
	TWeakObjectPtr<USFTargetQueryDataAsset> LastQuery;
};
