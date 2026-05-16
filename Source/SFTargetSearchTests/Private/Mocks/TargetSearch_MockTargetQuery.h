// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "TargetQueryDataAsset.h"
#include "TargetSearch_MockTargetQuery.generated.h"

class SF::UConditional;

/**
 * Mock target query used for testing.
 */
UCLASS(NotBlueprintType, Hidden)
class UTargetSearch_MockTargetQuery : public SF::UTargetQueryDataAsset
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetCanAcquireInstigatorAsTarget(const bool bInCanAcquireInstigatorAsTarget)
	{
		bCanAcquireInstigatorAsTarget = bInCanAcquireInstigatorAsTarget;
	}

	FORCEINLINE void SetTargetCategory(const FGameplayTag& InTargetCategory)
	{
		TargetCategory = InTargetCategory;
	}

	FORCEINLINE void SetSearchCondition(SF::UConditional* InSearchCondition)
	{
		SearchCondition = InSearchCondition;
	}
};
