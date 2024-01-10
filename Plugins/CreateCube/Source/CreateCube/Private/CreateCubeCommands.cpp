// Copyright Epic Games, Inc. All Rights Reserved.

#include "CreateCubeCommands.h"

#define LOCTEXT_NAMESPACE "FCreateCubeModule"

void FCreateCubeCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "CreateCube", "Execute CreateCube action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
