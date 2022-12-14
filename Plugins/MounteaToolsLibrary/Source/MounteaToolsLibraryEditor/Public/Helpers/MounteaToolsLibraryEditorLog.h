// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"

// Log category definition
MOUNTEATOOLSLIBRARYEDITOR_API DECLARE_LOG_CATEGORY_EXTERN(MounteaToolsLibraryEditor, Display, All);

#define MTLEditor_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(MounteaToolsLibraryEditor, Verbosity, Format, ##__VA_ARGS__); \
}