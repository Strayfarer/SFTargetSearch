// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "SFConditional.h"
#include "GameplayTagContainer.h"
#include "TargetCategoryTags.h"
#include "Engine/DataAsset.h"
#include "TargetQueryDataAsset.generated.h"

namespace SF
{
	/**
	 * Define a single, reusable query for targets using a tree of conditions.
	 */
	UCLASS(BlueprintType, ClassGroup="Target Search")
	class SFTARGETSEARCH_API UTargetQueryDataAsset : public UDataAsset
	{
		GENERATED_BODY()

	public:
		// UObject
#if WITH_EDITOR
		virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
		// --

		/** Whether the instigator with which the query is run is eligible as target. */
		FORCEINLINE bool CanAcquireInstigatorAsTarget() const { return bCanAcquireInstigatorAsTarget; }

		/** Pre-filter for targets. */
		FORCEINLINE FGameplayTag GetTargetCategory() const { return TargetCategory; }

		/** Condition by which a target gets selected. */
		FORCEINLINE USFConditional* GetSearchCondition() const { return SearchCondition; }

	protected:
		/** Whether the instigator with which the query is run is eligible as target. */
		UPROPERTY(EditDefaultsOnly)
		bool bCanAcquireInstigatorAsTarget = false;

		/** Pre-filter for targets using this target category tag. */
		UPROPERTY(EditDefaultsOnly, meta=(Categories="TargetCategory"))
		FGameplayTag TargetCategory = TAG_TargetCategory;

		/** 
		 * Among all target candidates giving a true binary answer to this condition,
		 * the one with highest fuzzy score will be chosen as target.
		 */
		UPROPERTY(EditDefaultsOnly, Instanced)
		TObjectPtr<USFConditional> SearchCondition = nullptr;
	};
}
