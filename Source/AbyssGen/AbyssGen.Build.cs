// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AbyssGen : ModuleRules
{
	public AbyssGen(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"Niagara",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AbyssGen",
			"AbyssGen/Dungeon",
			"AbyssGen/Dungeon/Core",
			"AbyssGen/Dungeon/Stages",
			"AbyssGen/Dungeon/ThirdParty",
			"AbyssGen/Variant_Platforming",
			"AbyssGen/Variant_Platforming/Animation",
			"AbyssGen/Variant_Combat",
			"AbyssGen/Variant_Combat/AI",
			"AbyssGen/Variant_Combat/Animation",
			"AbyssGen/Variant_Combat/Gameplay",
			"AbyssGen/Variant_Combat/Interfaces",
			"AbyssGen/Variant_Combat/UI",
			"AbyssGen/Variant_SideScrolling",
			"AbyssGen/Variant_SideScrolling/AI",
			"AbyssGen/Variant_SideScrolling/Gameplay",
			"AbyssGen/Variant_SideScrolling/Interfaces",
			"AbyssGen/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
