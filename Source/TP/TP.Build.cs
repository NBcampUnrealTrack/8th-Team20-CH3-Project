// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TP : ModuleRules
{
    public TP(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // 팀원분들이 추가해 두신 UMG, Slate, AIModule 모듈을 완벽하게 보존하면서 EnhancedInput을 함께 연동합니다.
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore", "AIModule" });
    }
}