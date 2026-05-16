// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "TargetComponent.h"

#include "TargetSearchLog.h"
#include "TargetSearchSubsystem.h"

SF::UTargetComponent::UTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void SF::UTargetComponent::BeginPlay()
{
	Super::BeginPlay();
	if (const auto TargetSearchSubsystem = UTargetSearchSubsystem::Get(*this))
	{
		TargetSearchSubsystem->RegisterTarget(this, TargetCategory);
	}
	else
	{
		UE_LOG(LogTargetSearch, Verbose, TEXT("%hs did not register %s since subsystem is disabled!"), 
			__FUNCTION__, *GetOwner()->GetName())
	}
}

void SF::UTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const auto TargetSearchSubsystem = UTargetSearchSubsystem::Get(*this))
	{
		TargetSearchSubsystem->DeregisterTarget(this, TargetCategory);
	}
	Super::EndPlay(EndPlayReason);
}
