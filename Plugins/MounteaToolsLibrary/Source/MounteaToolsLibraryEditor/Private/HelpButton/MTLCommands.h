// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "MTLHelpStyle.h"


class FMTLCommands : public TCommands<FMTLCommands>
{
public:

	FMTLCommands()
	: TCommands<FMTLCommands>(TEXT("AMTLSupport"), NSLOCTEXT("Contexts", "Support", "MounteaInteraction Plugin"), NAME_None, FMTLHelpStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	
	TSharedPtr< FUICommandInfo > PluginAction;
};
