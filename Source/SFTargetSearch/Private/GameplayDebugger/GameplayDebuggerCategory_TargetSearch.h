// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "TargetSearchSubsystem.h"

#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"

namespace SF
{
	class UTargetQueryDataAsset;

	class FGameplayDebuggerCategory_TargetSearch final : public FGameplayDebuggerCategory
	{
	public:
		FGameplayDebuggerCategory_TargetSearch();

		static FName GetCategoryName() { return "TargetSearch"; }

		static TSharedRef<FGameplayDebuggerCategory> MakeInstance()
		{
			return MakeShareable(new FGameplayDebuggerCategory_TargetSearch);
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
		void Input_ToggleShowConditionalDebugTrace();

		const TMap<TObjectPtr<UObject>, FQueryResultsCache>& TryGetQueryCacheFromTargetService() const;
		uint32 GetCachedQueryResultNumForInstigator() const;

		void UpdateSelectionFromIndices();

		UTargetSearchSubsystem* FindTargetSearchSubsystem() const;

		TWeakObjectPtr<UObject> CachedOwnerPc;
		TWeakObjectPtr<UObject> SelectedInstigator;
		TWeakObjectPtr<UTargetQueryDataAsset> SelectedQuery;

		int32 SelectedInstigatorIndex = 0;
		int32 SelectedQueryIndex = 0;
		
		bool bShowConditionalDebugTrace = true;
	};
}

#endif // WITH_GAMEPLAY_DEBUGGER