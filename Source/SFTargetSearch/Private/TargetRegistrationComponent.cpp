// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "TargetRegistrationComponent.h"

#include "TargetSearchLog.h"
#include "TargetSearchSubsystem.h"

SF::UTargetRegistrationComponent::UTargetRegistrationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void SF::UTargetRegistrationComponent::BeginPlay()
{
	Super::BeginPlay();
	Activate();
}

void SF::UTargetRegistrationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Deactivate();
	Super::EndPlay(EndPlayReason);
}

void SF::UTargetRegistrationComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	if (const auto TargetSearchSubsystem = UTargetSearchSubsystem::Get(*this))
	{
		if (bAutoRegisterOwnerActor)
		{
			TargetSearchSubsystem->RegisterTarget(GetOwner(), TargetCategory);
		}
		if (bAutoRegisterOwnerRootComponent)
		{
			TargetSearchSubsystem->RegisterTarget(GetOwner()->GetRootComponent(), TargetCategory);
		}
	}
	else
	{
		UE_LOG(LogTargetSearch, Verbose, TEXT("%hs did not register %s since subsystem is disabled!"), 
			__FUNCTION__, *GetOwner()->GetName())
	}
}

void SF::UTargetRegistrationComponent::Deactivate()
{
	Super::Deactivate();
	if (const auto TargetSearchSubsystem = UTargetSearchSubsystem::Get(*this))
	{
		if (bAutoRegisterOwnerActor)
		{
			TargetSearchSubsystem->DeregisterTarget(GetOwner(), TargetCategory);
		}
		if (bAutoRegisterOwnerRootComponent)
		{
			TargetSearchSubsystem->DeregisterTarget(GetOwner()->GetRootComponent(), TargetCategory);
		}
	}
}

