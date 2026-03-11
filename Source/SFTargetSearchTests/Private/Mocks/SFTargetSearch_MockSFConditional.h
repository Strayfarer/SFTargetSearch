// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "SFConditional/Public/SFConditional.h"
#include "SFTargetSearch_MockSFConditional.generated.h"

/**
 * Mock conditional used for testing.
 */
UCLASS()
class USFTargetSearch_MockSFConditional : public USFConditional
{
	GENERATED_BODY()
	
public:
	// - USFConditional
#if WITH_EDITOR
	virtual bool TryAddChild_Implementation(USFConditional* Child) override;
	virtual bool TryRemoveChild_Implementation(USFConditional* Child) override;
#endif
	// --
	
	FSFConditionalAnswer Answer = SF::Conditional::Answer::Yes();
	
	FInt32Range AllowedChildrenNumRange = FInt32Range(TRangeBound<int>::Inclusive(0), TRangeBound<int>::Inclusive(0));
	
	UPROPERTY()
	TArray<USFConditional*> Children = {};
	
	FORCEINLINE void SetWeight(const float InWeight) { Weight = InWeight; }
	FORCEINLINE void SetIsOptional(const bool bInIsOptional) { bIsOptional = bInIsOptional; }
	FORCEINLINE void SetIsInverted(const bool bInIsInverted) { bIsInverted = bInIsInverted; }
	FORCEINLINE void SetDoesImpactScoreOnFail(const bool bInDoesImpactScoreOnFail) { bDoesImpactScoreOnFail = bInDoesImpactScoreOnFail; }
	
protected:
	// - USFConditional
	virtual FSFConditionalAnswer EvaluateInternal_Implementation(const FSFConditionalEvaluationContext& EvaluationContext) override;
	virtual FInt32Range GetAllowedChildrenNumRange_Implementation() const override;
	virtual TArray<USFConditional*> GetImmediateChildren_Implementation() const override;
	// --
};

namespace SF::Conditional::Answer::Error
{
	const FSFConditionalAnswer& Mock();
}
