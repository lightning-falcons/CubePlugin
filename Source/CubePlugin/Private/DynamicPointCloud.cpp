// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicPointCloud.h"
#include "DynamicGameState.h"
#include "VrCharacter.h"

ADynamicPointCloud::ADynamicPointCloud()
{
	GameStateClass = ADynamicGameState::StaticClass();
	DefaultPawnClass = AVrCharacter::StaticClass();

}