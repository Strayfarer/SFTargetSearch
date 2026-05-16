// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "TargetQueryDataAsset.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#if WITH_EDITOR
EDataValidationResult SF::UTargetQueryDataAsset::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	if (SearchCondition)
	{
		Result = CombineDataValidationResults(Result, SearchCondition->IsDataValid(Context));
	}
	else
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		Context.AddError(FText::FromString(FString::Printf(TEXT("%s has invalid search condition!"), *GetName())));
	}
	return Result;
}
#endif
