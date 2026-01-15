#pragma once

#include "Engine/DeveloperSettings.h"
#include "MTLPopupConfig.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class UMTLPopupConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UMTLPopupConfig()
	{
	}

	UPROPERTY(config)
	FString PluginVersionUpdate = "";
	
};
