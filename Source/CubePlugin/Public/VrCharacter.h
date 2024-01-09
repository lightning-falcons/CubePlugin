// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "Math/Rotator.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/MathFwd.h"
#include "GameFramework/Actor.h"
#include <cmath>
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DynamicGameState.h"
#include "IXRTrackingSystem.h"
#include "VrCharacter.generated.h"


UCLASS()
class CUBEPLUGIN_API AVrCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVrCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	double ClockTime;
	int ScanIndex; // Position Number to Move
	TArray<TArray<double>> Odometry;


	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;

	// Array of timestamps for the odometry readings
	TArray<double> TimeStampOdometry;



};
