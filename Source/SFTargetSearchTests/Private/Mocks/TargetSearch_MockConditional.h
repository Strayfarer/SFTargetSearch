// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "SFConditional/Public/Conditional.h"
#include "TargetSearch_MockConditional.generated.h"

/**
 * Mock conditional used for testing.
 */
UCLASS(NotBlueprintType, Hidden)
class UTargetSearch_MockConditional : public SF::UConditional
{
	GENERATED_BODY()
	
public:
	// UConditional
#if WITH_EDITOR
	virtual void AddChild_Implementation(UConditional* Child) override;
	virtual bool TryRemoveChild_Implementation(UConditional* Child) override;
#endif
	// --
	
	SF::FConditionalAnswer AnswerTargetA = SF::Conditional::Answer::Yes();
	SF::FConditionalAnswer AnswerTargetB = SF::Conditional::Answer::No();
	
	UPROPERTY()
	TObjectPtr<UObject> TargetA = nullptr;
	
	FInt32Range AllowedChildrenNumRange = FInt32Range(TRangeBound<int>::Inclusive(0), TRangeBound<int>::Inclusive(0));
	
	UPROPERTY()
	TArray<UConditional*> Children = {};
	
	FORCEINLINE void SetWeight(const float InWeight) { Weight = InWeight; }
	FORCEINLINE void SetIsOptional(const bool bInIsOptional) { bIsOptional = bInIsOptional; }
	FORCEINLINE void SetIsInverted(const bool bInIsInverted) { bIsInverted = bInIsInverted; }
	FORCEINLINE void SetDoesImpactScoreOnFail(const bool bInDoesImpactScoreOnFail) { bDoesImpactScoreOnFail = bInDoesImpactScoreOnFail; }
	
protected:
	// UConditional
	virtual SF::FConditionalAnswer EvaluateInternal_Implementation(const SF::FConditionalEvaluationContext& EvaluationContext) override;
	virtual FInt32Range GetAllowedChildrenNumRange_Implementation() const override;
	virtual TArray<UConditional*> GetImmediateChildren_Implementation() const override;
	// --
};

namespace SF::Conditional::Answer::Error
{
	const FConditionalAnswer& Mock();
}
