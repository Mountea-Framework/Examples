#pragma once
#include "Framework/Text/SlateHyperlinkRun.h"

class MTLPopup
{
public:
	static void Register();
	static void OpenMTLPopup();
	static void OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata);
};
