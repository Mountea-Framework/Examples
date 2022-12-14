// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"


// Log category definition
MOUNTEATOOLSLIBRARY_API DECLARE_LOG_CATEGORY_EXTERN(MounteaToolsLibrary, Display, All);

#define MTL_LOG(Verbosity, Format, ...) \
{ \
UE_LOG(MounteaToolsLibrary, Verbosity, Format, ##__VA_ARGS__); \
}