// Copyright Strayfarer & Contributors. Released under the MIT license.

#include "TargetSearchModule.h"

#include "GameplayDebugger.h"
#include "GameplayDebugger/GameplayDebuggerCategory_TargetSearch.h"

void FSFTargetSearchModule::StartupModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger::Get().RegisterCategory(
			SF::FGameplayDebuggerCategory_TargetSearch::GetCategoryName(), 
			IGameplayDebugger::FOnGetCategory::CreateStatic(&SF::FGameplayDebuggerCategory_TargetSearch::MakeInstance));
	}
#endif
}

void FSFTargetSearchModule::ShutdownModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger::Get().UnregisterCategory(SF::FGameplayDebuggerCategory_TargetSearch::GetCategoryName());
	}
#endif
}
	
IMPLEMENT_MODULE(FSFTargetSearchModule, SFTargetSearch)