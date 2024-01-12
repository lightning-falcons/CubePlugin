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
#include "Camera/CameraActor.h"
#include "VideoWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "UObject/Object.h"
#include <UObject/ConstructorHelpers.h>
#include <Engine/DataTable.h>
#include "AssetRegistryModule.h"
//#include "Ability/FOEffect.h"
#include "Engine/Blueprint.h"
#include "Modules/ModuleManager.h"
#include "UObject/SoftObjectPtr.h"
#include "AssetRegistryModule.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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

public:
	// Video Display
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<class UVideoWidget> VideoWidgetClass;

private:
	UPROPERTY()
	class UVideoWidget* RealVideo;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* VideoWidgetComponent;

	UPROPERTY()
	TArray<TSubclassOf<UVideoWidget>> ItemReferences;

	UPROPERTY()
	TArray<UUserWidget *> ItemReferences2;

	bool VideoWidgetSet = false;


};
