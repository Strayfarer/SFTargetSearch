// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "TargetSearchSettings.h"

bool SF::UTargetSearchSettings::ShouldSystemStartInWorld(const UWorld& World) const
{
	return MapsToEnableTargetSearchIn.IsEmpty() || MapsToEnableTargetSearchIn.Contains(&World);
}
