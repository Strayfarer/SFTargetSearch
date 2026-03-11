// Copyright Strayfarer & Contributors. Released under the MIT license.

using UnrealBuildTool;

public class SFTargetSearch : ModuleRules
{
	public SFTargetSearch(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange([
			"Core",
			"SFConditional",
			"GameplayTags", 
			"ComponentPicker"
		]);
			
		PrivateDependencyModuleNames.AddRange([
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "GameplayDebugger",
            "InputCore",
            "DeveloperSettings"
        ]);
	}
}
