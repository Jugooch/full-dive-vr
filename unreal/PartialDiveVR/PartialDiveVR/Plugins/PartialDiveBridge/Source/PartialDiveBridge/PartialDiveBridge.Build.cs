using UnrealBuildTool;

public class PartialDiveBridge : ModuleRules
{
	public PartialDiveBridge(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Sockets",
			"Networking",
			"Json",
			"LSL", // LabStreamingLayer plugin: liblsl C API (lsl_local_clock, marker outlet)
		});
	}
}
