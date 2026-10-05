using UnrealBuildTool;

public class PartialDiveVR : ModuleRules
{
	public PartialDiveVR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"HeadMountedDisplay",   // UMotionControllerComponent
			"PartialDiveBridge",    // intent in / haptics out
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AnimationCore",        // two-bone IK
		});
	}
}
