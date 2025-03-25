// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrackamaniaGameMode.h"
#include "TrackamaniaPlayerController.h"

ATrackamaniaGameMode::ATrackamaniaGameMode()
{
	PlayerControllerClass = ATrackamaniaPlayerController::StaticClass();
}
