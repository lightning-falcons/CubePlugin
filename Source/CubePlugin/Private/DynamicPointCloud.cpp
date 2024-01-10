// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicPointCloud.h"
#include "DynamicGameState.h"
#include "DefaultPlayerController.h"
#include "VrCharacter.h"

ADynamicPointCloud::ADynamicPointCloud()
{
	GameStateClass = ADynamicGameState::StaticClass();
	DefaultPawnClass = AVrCharacter::StaticClass();
	PlayerControllerClass = ADefaultPlayerController::StaticClass();

}