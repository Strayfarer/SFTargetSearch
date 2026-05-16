// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TargetSearchSettings.generated.h"

namespace SF
{
	/**
	 * Project settings for the target search system.
	 */
	UCLASS(Config = Game, DefaultConfig, DisplayName = "Target Search")
	class SFTARGETSEARCH_API UTargetSearchSettings : public UDeveloperSettings
	{
		GENERATED_BODY()

	public:
		/** Whether target search is configured to run for the given world. */
		bool ShouldSystemStartInWorld(const UWorld& World) const;

	protected:
		/** 
		 * Maps to enable the target search feature in. 
		 * If this is empty, target search will be enabled for all maps.
		 */
		UPROPERTY(Config, EditAnywhere, Category="Target Search", meta=(AllowedClasses="/Script/Engine.World"))
		TArray<FSoftObjectPath> MapsToEnableTargetSearchIn = {};
	};
}
