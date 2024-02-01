// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CubePlugin : ModuleRules
{
	public CubePlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "LidarPointCloudRuntime", });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
				"C:\\Program Files\\Epic Games\\UE_5.3\\Engine\\Plugins\\Enterprise\\LidarPointCloud\\Source\\LidarPointCloudRuntime\\Public",
                //"C:\\Program Files\\Epic Games\\UE_5.3\\Engine\\**",
                // "C:\\Program Files\\Epic Games\\UE_5.3\\Engine\\Source\\Runtime\\AssetRegistry\\Public\\AssetRegistry",
                //"C:\\Program Files\\Epic Games\\UE_5.3\\Engine\\Source\\Runtime\\Core\\Public",
            }
            );

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
				"..\\Plugins\\Enterprise\\LidarPointCloud\\Source\\LidarPointCloudRuntime\\Public",
                // "C:\\Users\\its\\Downloads\\toml11-master\\toml11-master",
                "C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin 5.3\\toml11-master",
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
				// ... add other public dependencies that you statically link with here ...
				"LidarPointCloudRuntime",
                "Landscape",
                "UMG",
                "EnhancedInput",
                "HeadMountedDisplay",
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects",
                "InputCore",
                "EditorFramework",
                "UnrealEd",
                "ToolMenus",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

    }
}
