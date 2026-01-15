// All rights reserved Dominik Pavlicek 2022.


#include "MTLCommands.h"

#define LOCTEXT_NAMESPACE "MounteaInteractionSystemEditorModule"

void FMTLCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Support", "Opens Mountea Framework Support channel", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control|EModifierKey::Shift|EModifierKey::Alt, EKeys::X));
}

#undef LOCTEXT_NAMESPACE