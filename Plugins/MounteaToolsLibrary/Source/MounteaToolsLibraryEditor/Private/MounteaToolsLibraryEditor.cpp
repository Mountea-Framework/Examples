#include "MounteaToolsLibraryEditor.h"

#include "Helpers/MounteaToolsLibraryEditorLog.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FMounteaToolsLibraryEditor"

void FMounteaToolsLibraryEditor::StartupModule()
{
	RotationComponentSet = MakeShareable(new FSlateStyleSet("CameraFacingComp Style"));

	{
		const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("MounteaToolsLibrary");
		const FString ContentDir = IPluginManager::Get().FindPlugin("MounteaToolsLibrary")->GetBaseDir();
		
		// Interactable Rotation Component
		{
			RotationComponentSet->SetContentRoot(ContentDir);
        		
			FSlateImageBrush* RotationComponentClassThumb = new FSlateImageBrush(RotationComponentSet->RootToContentDir(TEXT("Resources/InteractableRotationIcon_128"), TEXT(".png")), FVector2D(128.f, 128.f));
			FSlateImageBrush* RotationComponentClassIcon = new FSlateImageBrush(RotationComponentSet->RootToContentDir(TEXT("Resources/InteractableRotationIcon_16"), TEXT(".png")), FVector2D(16.f, 16.f));
			if (RotationComponentClassThumb && RotationComponentClassIcon)
			{
				RotationComponentSet->Set("ClassThumbnail.CameraFacingComponent", RotationComponentClassThumb);
				RotationComponentSet->Set("ClassIcon.CameraFacingComponent", RotationComponentClassIcon);
     
				//Register the created style
				FSlateStyleRegistry::RegisterSlateStyle(*RotationComponentSet.Get());
			}
		}
	}
	
	MTLEditor_LOG(Warning, TEXT("MounteaToolsLibraryEditor module has been loaded"));
}

void FMounteaToolsLibraryEditor::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(RotationComponentSet->GetStyleSetName());
	
	MTLEditor_LOG(Warning, TEXT("MounteaToolsLibraryEditor module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaToolsLibraryEditor, MounteaToolsLibraryEditor)