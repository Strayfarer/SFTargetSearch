// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "SFTargetRegistrationComponent.h"

#include "SFTargetSearchLog.h"
#include "SFTargetSearchSubsystem.h"

USFTargetRegistrationComponent::USFTargetRegistrationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFTargetRegistrationComponent::BeginPlay()
{
	Super::BeginPlay();
	Activate();
}

void USFTargetRegistrationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Deactivate();
	Super::EndPlay(EndPlayReason);
}

void USFTargetRegistrationComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	if (const auto TargetSearchSubsystem = USFTargetSearchSubsystem::Get(*this))
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
		UE_LOG(LogSFTargetSearch, Verbose, TEXT("%hs did not register %s since subsystem is disabled!"), 
			__FUNCTION__, *GetOwner()->GetName())
	}
}

void USFTargetRegistrationComponent::Deactivate()
{
	Super::Deactivate();
	if (const auto TargetSearchSubsystem = USFTargetSearchSubsystem::Get(*this))
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

