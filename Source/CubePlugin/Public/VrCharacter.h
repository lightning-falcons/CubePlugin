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
//#include "AssetRegistryModule.h"
//#include "Ability/FOEffect.h"
#include "Engine/Blueprint.h"
#include "Modules/ModuleManager.h"
// #include "UObject/SoftObjectPtr.h"
//#include "AssetRegistryModule.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "IXRTrackingSystem.h"
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

	// Current time
	double ClockTime;

	// The array index for the local maps that we are up to
	int ScanIndex;

	// 2D array of odometry data
	TArray<TArray<double>> Odometry;

	// Spring arm component connected to character
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	// Camera component connected to spring arm component
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
	class UVideoWidget* TestVideo;

	// UPROPERTY(EditAnywhere)
	// class UWidgetComponent* VideoWidgetComponent;

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

	// Input mapping context that is applied to the character 
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputMappingContext* VRMappingContext;

	// Input action for toggling between bird's eye view and the ground
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	UInputAction* BirdAction;

	// UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	// UInputAction* GroundAction;

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

	UFUNCTION(BlueprintCallable, Category = "Input")
	void Bird(const FInputActionValue& Value);

	void Ground(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void Movement(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void BackTime(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ForwardTime(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleVideoVisibility(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OrthogonalRight(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OrthogonalLeft(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OrthogonalUp(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OrthogonalDown(const FInputActionValue& Value);


public:
	
	// Set the playback time and adjust the scene to match that time
	// The scene should be specifically chosen, not played back until it is correct 
	void SetTime(double Time);

	// Immediately reload the VRCharacter position, without needing to move between
	// data points. Would usually be used when the time has been manually changed.
	bool ImmediateReload = false;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Input)
	FRotator PitchAdjustmentRotator;

	// This is for tracing the odometry for debugging purpose, expose to blueprints
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Debugging)
	FVector CurrentOdometryLocation;
	
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Camera)
	double Exposure;


private:

	// These values indicate the orthogonal offsets applied to the
	// character location as desired by the user keyboard inputs
	UPROPERTY(EditAnywhere)
	double OrthogonalX = 0.0;

	UPROPERTY(EditAnywhere)
	double OrthogonalY = 0.0;



};
