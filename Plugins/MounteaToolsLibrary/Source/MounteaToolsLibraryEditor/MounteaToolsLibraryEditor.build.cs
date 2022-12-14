using UnrealBuildTool;
 
public class MounteaToolsLibraryEditor : ModuleRules
{
	public MounteaToolsLibraryEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PrivateIncludePaths.AddRange
        (
	    new string[] 
			{
		    }
        );

		PublicDependencyModuleNames.AddRange
		(
	new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"UnrealEd", 
				"BlueprintGraph",
				"MounteaToolsLibrary",
				"Engine"
			}
		);

		PrivateDependencyModuleNames.AddRange
		(
	new string[]
			{
				"UnrealEd",
				"Projects",

				"Slate",
				"SlateCore",
				"AssetTools",

				"BlueprintGraph",
				"Kismet",
				"MounteaToolsLibrary",

				"CoreUObject",
				"Engine",
				"UnrealEd",
				"KismetCompiler",
				"GameplayTasksEditor",
				"BlueprintGraph",
				"AssetRegistry",

				"EditorStyle",
				"GraphEditor",
				"SlateCore",
				"ToolMenus",
				"Kismet",
				"KismetWidgets",
				"PropertyEditor",
				"UMG",
				"GameplayTasks"
			}
		);
			
		CircularlyReferencedDependentModules.AddRange
		(
	new string[] 
			{
				//"KismetCompiler",
				//"UnrealEd",
				//"GraphEditor",
				//"Kismet",
			}
		);
	}
}