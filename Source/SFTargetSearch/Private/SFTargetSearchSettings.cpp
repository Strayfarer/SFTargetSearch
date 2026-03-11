// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "SFTargetSearchSettings.h"

bool USFTargetSearchSettings::ShouldSystemStartInWorld(const UWorld& World) const
{
	return MapsToEnableTargetSearchIn.IsEmpty() || MapsToEnableTargetSearchIn.Contains(&World);
}
