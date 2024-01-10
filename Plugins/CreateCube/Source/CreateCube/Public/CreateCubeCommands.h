// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CreateCubeStyle.h"

class FCreateCubeCommands : public TCommands<FCreateCubeCommands>
{
public:

	FCreateCubeCommands()
		: TCommands<FCreateCubeCommands>(TEXT("CreateCube"), NSLOCTEXT("Contexts", "CreateCube", "CreateCube Plugin"), NAME_None, FCreateCubeStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
