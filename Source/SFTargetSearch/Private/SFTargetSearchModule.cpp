// Copyright Strayfarer & Contributors. Released under the MIT license.

#include "SFTargetSearchModule.h"

#include "GameplayDebugger.h"
#include "GameplayDebugger/GameplayDebuggerCategory_SFTargetSearch.h"

void FSFTargetSearchModule::StartupModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger::Get().RegisterCategory(
			FGameplayDebuggerCategory_SFTargetSearch::GetCategoryName(), 
			IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_SFTargetSearch::MakeInstance));
	}
#endif
}

void FSFTargetSearchModule::ShutdownModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger::Get().UnregisterCategory(FGameplayDebuggerCategory_SFTargetSearch::GetCategoryName());
	}
#endif
}
	
IMPLEMENT_MODULE(FSFTargetSearchModule, SFTargetSearch)