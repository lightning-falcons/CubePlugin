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

// Meed to avoid macro conflict
#pragma push_macro("check")   // store 'check' macro current definition
#undef check  // undef to avoid conflicts
#include "toml.hpp"
// #include "C:\\Users\\its\\Downloads\\toml11-master\\toml11-master\\toml.hpp"
#pragma pop_macro("check")  // restore definition

#include <filesystem>

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
	void LoadNext( FVector CharacterLocation, int Index, FRotator LocalRotation, ALidarPointCloudActor* PointCloudActor);

	// Templated function for array downsampling
	// We down sample every 1 in Per items, with up
	// to MaxLength number of items in the downsampled array
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
	int DownSamplePer = 1;

	// Get all the files in the directory, with restrictable extensions
	static TArray<FString> GetAllFilesInDirectory(const FString directory, const bool fullPath, const FString onlyFilesStartingWith, const FString onlyFilesWithExtension);


private:

	UPROPERTY()
	ALidarPointCloudActor* GlobalMapActor; // To hold the global point cloud
	
	UPROPERTY()
	ALidarPointCloudActor* DynamicActor; // Primary actor to hold the local point cloud

	// This is used for displaying 2 point clouds at once
	UPROPERTY()
	ALidarPointCloudActor* DynamicActorInterlaced; // Secondary actor to hold the local point cloud

	TArray<FString> filesInDirectory; // Array to hold all the extracted filenames from a folder

	UPROPERTY()
	TArray< ULidarPointCloud* > LoadedPointClouds; // Array to store all the loaded local point clouds
	
	UPROPERTY()
	ULidarPointCloud* GlobalMap; // The global point cloud

	// UPROPERTY()
	TArray<FLidarPointCloudPoint*> Points; // Array to store the points of a point cloud

	// Set all points of a point cloud to a particular colour
	void SetColor(FColor AppliedColor, ULidarPointCloud* Map);

	std::mutex g_num_mutex;

public:

	// Set the playback time and adjust the scene to match that time
	void SetTime(double Time);

	// Reset the scene to start of playback
	void ResetSimulation();

	// This contains the config file information
	toml::value data; // Contains entire config info
	toml::value loading; // Contains locations of files for loading
	toml::value params; // Contains parameters for the loading

	std::filesystem::path FullOdometryPath; // Full path to the odometry file
	std::filesystem::path FullImagePath; // Full path to the image folder

	double PlaybackSpeed; // Playback speed as a multiple of normal
	double PlaybackSpeedConfigured; // Playback speed as configured by the config file
	int NumberFrames; // Number of frames to import
	bool PhotoImport; // Whether the video should be played
	double HeightBirds; // How high up the bird's eye view should be

	bool ImmediateReload = false;
	bool ImmediateReloadSecond = false;

	// AVrCharacter* ControlledCharacter;

	// Point sizes
	double LocalGroundSize = 1.0;
	double LocalBirdSize = 0.5;
	double GlobalGroundSize = 0.05;
	double GlobalBirdSize = 0.05;

	void HidePoint(FLidarPointCloudPoint* Point);

	double PitchCorrection;
	double YawCorrection;
	double RollCorrection; // Global point cloud roll correction angle

	bool LoadGlobal;
	bool LoadOne;

	ULidarPointCloud* SingleCloud;
	int SingleCloudLoaded = 2;

	
};
