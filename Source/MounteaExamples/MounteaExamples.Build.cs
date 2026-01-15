// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MounteaExamples : ModuleRules
{
	public MounteaExamples(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MounteaExamples",
			"MounteaExamples/Variant_Horror",
			"MounteaExamples/Variant_Horror/UI",
			"MounteaExamples/Variant_Shooter",
			"MounteaExamples/Variant_Shooter/AI",
			"MounteaExamples/Variant_Shooter/UI",
			"MounteaExamples/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
