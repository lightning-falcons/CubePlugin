// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Editor/EditorEngine.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "UObject/Object.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Components/LightComponent.h"
#include "Engine/Engine.h"
#include "Engine/PostProcessVolume.h"
#include "Editor/EditorEngine.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

//#include "Engine.h"
//#include "Editor.h"

#include "LidarPointCloud.h"
#include "LidarPointCloudActor.h"
#include "LidarPointCloudComponent.h"
#include "LidarPointCloudShared.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "Runtime/Landscape/Classes/Landscape.h"
#include "Runtime/Landscape/Classes/LandscapeStreamingProxy.h"
#include "Engine/Light.h"
#include "Engine/DirectionalLight.h"
#include "Camera/CameraActor.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include "DynamicGameState.generated.h"

/**
 * 
 */
UCLASS()
class CUBEPLUGIN_API ADynamicGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ADynamicGameState();
	virtual ~ADynamicGameState() = default;

	void BeginPlay();
	void Tick( float DeltaSeconds );
	void LoadNext( FVector CharacterLocation, int Index, FRotator LocalRotation);

	template <typename T>
	static TArray<T> ExtractEvery(TArray<T> Array, int Per, int MaxLength = -1)
	{
		TArray<T> ExtractArray;

		for (int i = 0; i < Array.Num(); i++)
		{
			if (ExtractArray.Num() == MaxLength)
			{
				break;
			}

			if (i % Per == 0)
			{
				ExtractArray.Add(Array[i]);
			}
		}

		return ExtractArray;
	}

	// Array of timestamps for the local maps
	TArray<double> TimeStamp;

	// The Game State is responsible for keeping track of the time and the
	// current details e.g the CURRENT position & rotation according to the odometry.
	// Please note that this is needed because update of the point cloud and
	// character orientation and position may now be non-synchronous.
	double ClockTime; // The time in seconds, as according to the names of the files
	int ScanIndex; // Scan number to display for the POINT CLOUD

	TArray<double> CurrentOdometry; // Current odometry details (applied)
	
	// The Game State also has odometry data that corresponds SPECIFICALLY to the LIDAR data
	TArray<TArray<double>> Odometry;

	// Downsampling Rate
	int DownSamplePer = 20;

private:

	TArray<FString> GetAllFilesInDirectory(const FString directory, const bool fullPath, const FString onlyFilesStartingWith, const FString onlyFilesWithExtension);
	ALidarPointCloudActor* GlobalMapActor;
	ALidarPointCloudActor* DynamicActor;



	TArray<FString> filesInDirectory;
	TArray< ULidarPointCloud* > LoadedPointClouds;
	ULidarPointCloud* GlobalMap;
	TArray<FLidarPointCloudPoint*> Points;
	void SetColor(FColor AppliedColor, ULidarPointCloud* Map);
	std::mutex g_num_mutex;




};
