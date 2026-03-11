// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "SFTargetComponent.h"

#include "SFTargetSearchLog.h"
#include "SFTargetSearchSubsystem.h"

USFTargetComponent::USFTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFTargetComponent::BeginPlay()
{
	Super::BeginPlay();
	if (const auto TargetSearchSubsystem = USFTargetSearchSubsystem::Get(*this))
	{
		TargetSearchSubsystem->RegisterTarget(this, TargetCategory);
	}
	else
	{
		UE_LOG(LogSFTargetSearch, Verbose, TEXT("%hs did not register %s since subsystem is disabled!"), 
			__FUNCTION__, *GetOwner()->GetName())
	}
}

void USFTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const auto TargetSearchSubsystem = USFTargetSearchSubsystem::Get(*this))
	{
		TargetSearchSubsystem->DeregisterTarget(this, TargetCategory);
	}
	Super::EndPlay(EndPlayReason);
}
