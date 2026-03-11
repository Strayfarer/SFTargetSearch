// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "SFTargetSearchSubsystem.h"

#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"

class USFTargetQueryDataAsset;

class FGameplayDebuggerCategory_SFTargetSearch final : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_SFTargetSearch();

	static FName GetCategoryName() { return "TargetSearch"; }
	static TSharedRef<FGameplayDebuggerCategory> MakeInstance()
	{
		return MakeShareable(new FGameplayDebuggerCategory_SFTargetSearch);
	}

	// - FGameplayDebuggerCategory
	virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;
	// --

protected:
	static void DrawControls(FGameplayDebuggerCanvasContext& CanvasContext);
	void DrawSelectableInstigators(FGameplayDebuggerCanvasContext& CanvasContext) const;
	void DrawSelectableQueries(FGameplayDebuggerCanvasContext& CanvasContext) const;
	
	void Input_NextInstigator();
	void Input_PreviousInstigator();
	void Input_NextQuery();
	void Input_PreviousQuery();
	void Input_LastInstigatorAndQuery();

	const TMap<TObjectPtr<UObject>, FSFQueryResultsCache>& TryGetQueryCacheFromTargetService() const;
	uint32 GetCachedQueryResultNumForInstigator() const;
	
	void UpdateSelectionFromIndices();
	
	USFTargetSearchSubsystem* FindTargetSearchSubsystem() const;
	
	TWeakObjectPtr<UObject> CachedOwnerPc;
	TWeakObjectPtr<UObject> SelectedInstigator;
	TWeakObjectPtr<USFTargetQueryDataAsset> SelectedQuery;
	
	int32 SelectedInstigatorIndex = 0;
	int32 SelectedQueryIndex = 0;
};

#endif // WITH_GAMEPLAY_DEBUGGER