// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "SFTargetSearch/Public/SFTargetQueryDataAsset.h"
#include "SFTargetSearch_MockSFTargetQuery.generated.h"

class USFConditional;

/**
 * Mock target query used for testing.
 */
UCLASS()
class USFTargetSearch_MockSFTargetQuery : public USFTargetQueryDataAsset
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
	
	FORCEINLINE void SetSearchCondition(USFConditional* InSearchCondition)
	{
		SearchCondition = InSearchCondition;
	}
};
