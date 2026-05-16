// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "TargetSearch_MockConditional.h"

#if WITH_EDITOR
bool UTargetSearch_MockConditional::TryAddChild_Implementation(UConditional* Child)
{
	Children.Emplace(Child);
	return true;
}

bool UTargetSearch_MockConditional::TryRemoveChild_Implementation(UConditional* Child)
{
	return Children.Remove(Child) > 0;
}
#endif

SF::FConditionalAnswer UTargetSearch_MockConditional::EvaluateInternal_Implementation(
	const SF::FConditionalEvaluationContext& EvaluationContext)
{
	return Answer;
}

FInt32Range UTargetSearch_MockConditional::GetAllowedChildrenNumRange_Implementation() const
{
	return AllowedChildrenNumRange;
}

TArray<SF::UConditional*> UTargetSearch_MockConditional::GetImmediateChildren_Implementation() const
{
	return Children;
}

const SF::FConditionalAnswer& SF::Conditional::Answer::Error::Mock()
{
	static FConditionalAnswer Instance = FromErrorMsg(FName("Mock"));
	return Instance;
}
