// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LostPlace : ModuleRules
{
	public LostPlace(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {  "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","UMG","AIModule", "OnlineSubsystem","OnlineSubsystemSteam","OnlineSubsystemUtils","GameplayAbilities"});
		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "GameplayTasks", "NavigationSystem", "Niagara" });

	}
}
