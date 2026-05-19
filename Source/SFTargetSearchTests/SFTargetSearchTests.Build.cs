// Copyright Strayfarer & Contributors. Released under the MIT license.

using UnrealBuildTool;

public class SFTargetSearchTests : ModuleRules
{
    public SFTargetSearchTests(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange([
            "Core",
            "GameplayTags"
        ]);

        PrivateDependencyModuleNames.AddRange([
            "CoreUObject",
            "Engine",
            "SFConditional",
            "SFTargetSearch"
        ]);
    }
}