// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "SFTargetSearch_MockSFConditional.h"

#if WITH_EDITOR
bool USFTargetSearch_MockSFConditional::TryAddChild_Implementation(USFConditional* Child)
{
	Children.Emplace(Child);
	return true;
}

bool USFTargetSearch_MockSFConditional::TryRemoveChild_Implementation(USFConditional* Child)
{
	return Children.Remove(Child) > 0;
}
#endif

FSFConditionalAnswer USFTargetSearch_MockSFConditional::EvaluateInternal_Implementation(
	const FSFConditionalEvaluationContext& EvaluationContext)
{
	return Answer;
}

FInt32Range USFTargetSearch_MockSFConditional::GetAllowedChildrenNumRange_Implementation() const
{
	return AllowedChildrenNumRange;
}

TArray<USFConditional*> USFTargetSearch_MockSFConditional::GetImmediateChildren_Implementation() const
{
	return Children;
}

const FSFConditionalAnswer& SF::Conditional::Answer::Error::Mock()
{
	static FSFConditionalAnswer Instance = FromErrorMsg(FName("Mock"));
	return Instance;
}
