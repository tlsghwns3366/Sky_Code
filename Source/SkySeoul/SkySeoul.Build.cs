// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkySeoul : ModuleRules
{
	public SkySeoul(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "SkySeoul"
            }
        );


        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "UMG", });
	}
}
