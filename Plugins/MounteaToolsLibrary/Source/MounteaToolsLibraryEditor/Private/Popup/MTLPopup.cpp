#include "MTLPopup.h"
#include "MTLPopupConfig.h"
#include "EditorStyleSet.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "SWebBrowser.h"
#include "Interfaces/IPluginManager.h"

void MTLPopup::OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata)
{
	const FString* URL = Metadata.Find(TEXT("href"));

	if (URL)
	{
		FPlatformProcess::LaunchURL(**URL, nullptr, nullptr);
	}
}

void MTLPopup::Register()
{
	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaToolsLibrary"))->GetBaseDir();
	const FString UpdatedConfigFile = PluginDirectory + "/Config/UpdateConfig.ini";
	const FString CurrentPluginVersion = "1.0";

	UMTLPopupConfig* MTLPopupConfig = GetMutableDefault<UMTLPopupConfig>();

	if (FPaths::FileExists(UpdatedConfigFile))
	{
		MTLPopupConfig->LoadConfig(nullptr, *UpdatedConfigFile);
	}
	else
	{
		MTLPopupConfig->SaveConfig(CPF_Config, *UpdatedConfigFile);
	}

	if (MTLPopupConfig->PluginVersionUpdate != CurrentPluginVersion)
	{
		MTLPopupConfig->PluginVersionUpdate = CurrentPluginVersion;
		MTLPopupConfig->SaveConfig(CPF_Config, *UpdatedConfigFile);
		
		FCoreDelegates::OnPostEngineInit.AddLambda([]()
		{
			OpenMTLPopup();
		});
	}
}

void MTLPopup::OpenMTLPopup()
{
	if (!FSlateApplication::Get().CanDisplayWindows())
	{
		return;
	}

	const TSharedRef<SBorder> WindowContent = SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(FMargin(8.0f, 8.0f));

	TSharedPtr<SWindow> Window = SNew(SWindow)
				.AutoCenter(EAutoCenter::PreferredWorkArea)
				.SupportsMaximize(false)
				.SupportsMinimize(false)
				.SizingRule(ESizingRule::FixedSize)
				.ClientSize(FVector2D(800, 600))
				.Title(FText::FromString("Mountea Tools Library"))
				.IsTopmostWindow(true)
	[
		WindowContent
	];

	const FSlateFontInfo HeadingFont = FCoreStyle::GetDefaultFontStyle("Regular", 24);
	const FSlateFontInfo ContentFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);

	const TSharedRef<SVerticalBox> InnerContent = SNew(SVerticalBox)
		// Default settings example
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(10)
		[
			SNew(STextBlock)
			.Font(HeadingFont)
			.Text(FText::FromString("Mountea Tools Library v1.0"))
			.Justification(ETextJustify::Center)
		]
		+ SVerticalBox::Slot()
		  .FillHeight(1.0)
		  .Padding(10)
		[
			SNew(SBorder)
			.Padding(10)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SNew(SRichTextBlock)
					.Text(FText::FromString(R"(
<LargeText>Hello and thank you for using Mountea Tools Library!</>

First thing first, if you've been enjoying using it, it would mean a lot if you could just drop <a id="browser" href="https://bit.ly/AIntP_UE4Marketplace">a small review on the marketplace page</> :).

I also made a paid <a id="browser" href="https://bit.ly/ModularSwordsPack_UE4Marketplace">Modular Sword Pack</>. It's a simple yet powerful tool that allows creating thousands upon thousands of unique swords with a simple click, now with a free upgrade of Modular Scabbard System!
					
But let's keep it short, here are the cool new features (and bugfixes) of version 1.0!

<LargeText>Version 1.0</>

<RichTextBlock.Bold>Features</>

* Add new Interactor Component Base Class implementing <RichTextBlock.Italic>IMounteaInteractorInterface</>

<RichTextBlock.Bold>Bugfixes</>

* Fix missed descriptions
* Add <RichTextBlock.Bold>DEPRECATED</> to old Component Classes
)"))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.DecoratorStyleSet(&FAppStyle::Get())
					.AutoWrapText(true)
					+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateStatic(&OnBrowserLinkClicked))
				]
			]
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(10)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Leave a review <3"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://bit.ly/AIntP_UE4Marketplace";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Support our work"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://bit.ly/AIntP_GitHubDonate";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Join support Discord"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([]()
				{
					const FString URL = "https://discord.gg/2vXWEEN";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Unreal Bucket"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([Window]()
				{
					const FString URL = "https://www.unrealengine.com/marketplace/en-US/product/3ce48046720d4a66b4f804b0d135a820";
					FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(20, 10))
			]
			+ SHorizontalBox::Slot().FillWidth(1.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Close window"))
				.HAlign(HAlign_Center)
				.OnClicked_Lambda([Window]()
				{
					Window->RequestDestroyWindow();

					return FReply::Handled();
				})
			]
		];

	WindowContent->SetContent(InnerContent);
	Window = FSlateApplication::Get().AddWindow(Window.ToSharedRef());
}