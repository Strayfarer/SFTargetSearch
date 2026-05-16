// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "TargetSearch_MockSFConditional.h"

#if WITH_EDITOR
bool UTargetSearch_MockSFConditional::TryAddChild_Implementation(USFConditional* Child)
{
	Children.Emplace(Child);
	return true;
}

bool UTargetSearch_MockSFConditional::TryRemoveChild_Implementation(USFConditional* Child)
{
	return Children.Remove(Child) > 0;
}
#endif

FSFConditionalAnswer UTargetSearch_MockSFConditional::EvaluateInternal_Implementation(
	const FSFConditionalEvaluationContext& EvaluationContext)
{
	return Answer;
}

FInt32Range UTargetSearch_MockSFConditional::GetAllowedChildrenNumRange_Implementation() const
{
	return AllowedChildrenNumRange;
}

TArray<USFConditional*> UTargetSearch_MockSFConditional::GetImmediateChildren_Implementation() const
{
	return Children;
}

const FSFConditionalAnswer& SF::Conditional::Answer::Error::Mock()
{
	static FSFConditionalAnswer Instance = FromErrorMsg(FName("Mock"));
	return Instance;
}
