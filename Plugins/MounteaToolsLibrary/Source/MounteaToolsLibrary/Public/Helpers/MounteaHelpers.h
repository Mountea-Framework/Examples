// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "MounteaHelpers.generated.h"


#pragma region Playback

UENUM(BlueprintType)
enum class ETimelinePlayback : uint8
{
	ETP_Play					UMETA(DisplayName = "Play"),
	ETP_Reverse					UMETA(DisplayName = "Play Reverse"),
};


#pragma endregion 

#pragma region Playtime
UENUM()
enum class EPlaybackResult : uint8
{
	EPR_Update		UMETA(DisplayName = "Update"),
	EPR_Finished	UMETA(DisplayName = "Finished"),
	EPR_Paused		UMETA(DisplayName = "Paused")
};
#pragma endregion 