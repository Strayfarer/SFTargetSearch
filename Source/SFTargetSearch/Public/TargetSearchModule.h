// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "Modules/ModuleManager.h"

class FSFTargetSearchModule : public IModuleInterface
{
public:
	// IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// --
};
