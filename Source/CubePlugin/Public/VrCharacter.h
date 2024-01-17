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
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "VrCharacter.generated.h"

// Two types of views are allowed
enum ViewType { ROADVIEW, BIRDVIEW };

// Two types of movement are allowed
enum MovementType { STOPPED, MOVING };

class UInputMappingContext;
class UInputAction;

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
	UPROPERTY()
	TArray<double> TimeStampOdometry;

public:
	// Video Display
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<class UVideoWidget> VideoWidgetClass;

	// For switching between bird's eye view and road view
	void SelectView(ViewType View);

	// For pausing the movement of the VR character
	void PauseMovement();

	// For restarting the movement of the VR character
	void StartMovement();

private:
	UPROPERTY()
	class UVideoWidget* RealVideo;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* VideoWidgetComponent;

	UPROPERTY()
	TArray<TSubclassOf<UVideoWidget>> ItemReferences;

public:
	UPROPERTY()
	TArray<UUserWidget *> ItemReferences2;

	bool VideoWidgetSet = false;

	// Adjustment for the bird's eye view
	double ZAdjustment = 0;
	double PitchAdjustment = 0;

	ViewType CurrentView = ROADVIEW;

	// Store whether the character should be moving
	MovementType Motion = MOVING;

protected:

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputMappingContext* VRMappingContext;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* BirdAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* GroundAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* BackTimeAction;
	
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* ForwardTimeAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* ToggleVideoVisibilityAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* OrthogonalRightAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* OrthogonalLeftAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* OrthogonalUpAction;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* OrthogonalDownAction;

	void Bird(const FInputActionValue& Value);
	void Ground(const FInputActionValue& Value);
	void Movement(const FInputActionValue& Value);
	void BackTime(const FInputActionValue& Value);
	void ForwardTime(const FInputActionValue& Value);
	void ToggleVideoVisibility(const FInputActionValue& Value);
	void OrthogonalRight(const FInputActionValue& Value);
	void OrthogonalLeft(const FInputActionValue& Value);
	void OrthogonalUp(const FInputActionValue& Value);
	void OrthogonalDown(const FInputActionValue& Value);


public:
	
	void SetTime(double Time);

private:

	// These values indicate the orthogonal offsets applied to the
	// character location as desired by the user keyboard inputs
	UPROPERTY(EditAnywhere)
	double OrthogonalX = 0.0;

	UPROPERTY(EditAnywhere)
	double OrthogonalY = 0.0;



};
