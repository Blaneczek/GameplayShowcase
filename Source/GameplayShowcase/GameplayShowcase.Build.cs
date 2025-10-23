// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameplayShowcase : ModuleRules
{
	public GameplayShowcase(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayTags",
            "GameplayTasks",
            "GameplayAbilities",
            "NavigationSystem",
            "Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] {
			"GameplayShowcase",
			"GameplayShowcase/Variant_Platforming",
			"GameplayShowcase/Variant_Platforming/Animation",
			"GameplayShowcase/Variant_Combat",
			"GameplayShowcase/Variant_Combat/AI",
			"GameplayShowcase/Variant_Combat/Animation",
			"GameplayShowcase/Variant_Combat/Gameplay",
			"GameplayShowcase/Variant_Combat/Interfaces",
			"GameplayShowcase/Variant_Combat/UI",
			"GameplayShowcase/Variant_SideScrolling",
			"GameplayShowcase/Variant_SideScrolling/AI",
			"GameplayShowcase/Variant_SideScrolling/Gameplay",
			"GameplayShowcase/Variant_SideScrolling/Interfaces",
			"GameplayShowcase/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
