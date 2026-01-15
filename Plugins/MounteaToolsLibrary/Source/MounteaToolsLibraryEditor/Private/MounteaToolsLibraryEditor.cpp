#include "MounteaToolsLibraryEditor.h"

#include "HelpButton/MTLCommands.h"
#include "HelpButton/MTLHelpStyle.h"
#include "Helpers/MounteaToolsLibraryEditorLog.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "Popup/MTLPopup.h"

#include "ToolMenus.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FMounteaToolsLibraryEditor"

void FMounteaToolsLibraryEditor::StartupModule()
{
	RotationComponentSet = MakeShareable(new FSlateStyleSet("CameraFacingComp Style"));
	AudioComponentSet = MakeShareable(new FSlateStyleSet("MounteaAudioComp Style"));

	{
		const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("MounteaToolsLibrary");
		const FString ContentDir = IPluginManager::Get().FindPlugin("MounteaToolsLibrary")->GetBaseDir();
		
		// Rotation Component
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

		// Audio Component
		{
			AudioComponentSet->SetContentRoot(ContentDir);
        		
			FSlateImageBrush* MounteaAudioComponentClassThumb = new FSlateImageBrush(AudioComponentSet->RootToContentDir(TEXT("Resources/MounteaAudioComponentIcon_128"), TEXT(".png")), FVector2D(128.f, 128.f));
			FSlateImageBrush* MounteaAudioComponentClassIcon = new FSlateImageBrush(AudioComponentSet->RootToContentDir(TEXT("Resources/MounteaAudioComponentIcon_16"), TEXT(".png")), FVector2D(16.f, 16.f));
			if (MounteaAudioComponentClassThumb && MounteaAudioComponentClassIcon)
			{
				AudioComponentSet->Set("ClassThumbnail.MounteaAudioComponent", MounteaAudioComponentClassThumb);
				AudioComponentSet->Set("ClassIcon.MounteaAudioComponent", MounteaAudioComponentClassIcon);
     
				//Register the created style
				FSlateStyleRegistry::RegisterSlateStyle(*AudioComponentSet.Get());
			}
		}
	}

	// Register popup
	{
		MTLPopup::Register();
	}

	// Register Help Button
	{
		FMTLHelpStyle::Initialize();
		FMTLHelpStyle::ReloadTextures();

		FMTLCommands::Register();

		PluginCommands = MakeShareable(new FUICommandList);

		PluginCommands->MapAction(
			FMTLCommands::Get().PluginAction,
			FExecuteAction::CreateRaw(this, &FMounteaToolsLibraryEditor::PluginButtonClicked), 
			FCanExecuteAction());

		IMainFrameModule& mainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
		mainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMounteaToolsLibraryEditor::RegisterMenus));
	}
	
	MTLEditor_LOG(Warning, TEXT("MounteaToolsLibraryEditor module has been loaded"));
}

void FMounteaToolsLibraryEditor::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(RotationComponentSet->GetStyleSetName());
	FSlateStyleRegistry::UnRegisterSlateStyle(AudioComponentSet->GetStyleSetName());

	// Help Button Cleanup
	{
		UToolMenus::UnRegisterStartupCallback(this);

		UToolMenus::UnregisterOwner(this);

		FMTLHelpStyle::Shutdown();

		FMTLCommands::Unregister();
	}
	
	MTLEditor_LOG(Warning, TEXT("MounteaToolsLibraryEditor module has been unloaded"));
}

void FMounteaToolsLibraryEditor::PluginButtonClicked()
{
	const FString URL = "https://discord.gg/2vXWEEN";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
}

void FMounteaToolsLibraryEditor::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	// Register in Window tab
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help");
		{
			if (! Menu->FindSection(TEXT("MounteaFramework")))
			{
				FToolMenuSection& Section = Menu->FindOrAddSection("MounteaFramework");

				if (!Section.FindEntry(TEXT("Mountea Framework")) )
				{
					Section.Label = FText::FromString(TEXT("Mountea Framework"));
						
					FToolMenuEntry Entry = Section.AddMenuEntryWithCommandList
					(
						FMTLCommands::Get().PluginAction,
						PluginCommands,
						NSLOCTEXT("MounteaSupport", "TabTitle", "Mountea Support"),
						NSLOCTEXT("MounteaSupport", "TooltipText", "Opens Mountea Framework Support channel"),
						FSlateIcon(FMTLHelpStyle::GetStyleSetName(), "AIntPSupport.PluginAction.small")
					);
				}
			}
		}
	}

	// Register in Level Editor Toolbar
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			if (!ToolbarMenu->FindSection(TEXT("MounteaFramework")))
			{
				FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("MounteaFramework");
				{
					if (!Section.FindEntry(TEXT("Mountea Framework")) )
					{
						Section.Label = FText::FromString(TEXT("Mountea Framework"));
				
						FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMTLCommands::Get().PluginAction));
						Entry.SetCommandList(PluginCommands);
				
						Entry.InsertPosition.Position = EToolMenuInsertType::First;
					}
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaToolsLibraryEditor, MounteaToolsLibraryEditor)