#pragma once

#include "Modules/ModuleManager.h"

class FMounteaToolsLibraryEditor : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	TSharedPtr<class FSlateStyleSet> RotationComponentSet;
};