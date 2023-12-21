// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicGameState.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

ADynamicGameState::ADynamicGameState()
{
	
	
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bStartWithTickEnabled = true;
	UE_LOG(LogTemp, Warning, TEXT("AAAAAAAAAAAAAAAAAAAA"));

	
}

void ADynamicGameState::BeginPlay()
{
	Super::BeginPlay();

	ClockTime = 0;
	ScanIndex = 0;

	ULevel* Level = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	
	const FString PathToFile = FString("C:\\Users\\admin\\Downloads\\TransformedCloud_Cudal.las");
	//const FString PathToFile = FString("C:\\Users\\admin\\Downloads\\colour.las");
	//const FString PathToFile = FString("C:\\Users\\admin\\cudal_short_parsed.las");


	// Create an actor for the global and local maps
	GlobalMapActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor( ALidarPointCloudActor::StaticClass() ));
	DynamicActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor(ALidarPointCloudActor::StaticClass()));

	// Create the Global Map Point Cloud from the file
	GlobalMap = ULidarPointCloud::CreateFromFile(PathToFile);

	// Specify that the global point cloud should be displayed with colour source from data and with the particular point size
	GlobalMapActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::Data;
	GlobalMapActor->GetPointCloudComponent()->PointSize = 0.25;

	
	FString directoryToSearch = TEXT("C:\\Users\\admin\\Desktop\\Sequence_Cudal");
	FString filesStartingWith = TEXT("");
	FString fileExtensions = TEXT("las");

	UE_LOG(LogTemp, Warning, TEXT("%s"), *fileExtensions);




	filesInDirectory = GetAllFilesInDirectory(directoryToSearch, true, filesStartingWith, fileExtensions);

	// This is for optional skipping of files
	int SampleEvery = 1;
	int SampleCounter = 1;
	int MaxLoading = 300;
	int CurrentLoading = 0;

	for (auto it : filesInDirectory)
	{
		CurrentLoading += 1;
		LoadedPointClouds.Add(ULidarPointCloud::CreateFromFile(it));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *it);

		if (CurrentLoading == MaxLoading)
		{
			break;
		}
		/*
		if (SampleCounter == SampleEvery) 
		{
			LoadedPointClouds.Add(ULidarPointCloud::CreateFromFile(it));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *it);
			
			SampleCounter = 1;
			CurrentLoading += 1;
		}
		else 
		{
			SampleCounter += 1;
		}

		if (CurrentLoading == MaxLoading)
		{
			break;
		}

		*/
		
	}




	//UE_LOG(LogTemp, Warning, TEXT("ACTOR LOC %lf"), GlobalMap->OriginalCoordinates[2]);

	TArray< ULidarPointCloud* > LoadedPointCloudsCombined;
	LoadedPointCloudsCombined = LoadedPointClouds;
	LoadedPointCloudsCombined.Add(GlobalMap);


	//ULidarPointCloud::AlignClouds(LoadedPointClouds);

}

/**
Gets all the files in a given directory.
@param directory The full path of the directory we want to iterate over.
@param fullpath Whether the returned list should be the full file paths or just the filenames.
@param onlyFilesStartingWith Will only return filenames starting with this string. Also applies onlyFilesEndingWith if specified.
@param onlyFilesEndingWith Will only return filenames ending with this string (it looks at the extension as well!). Also applies onlyFilesStartingWith if specified.
@return A list of files (including the extension).
*/
TArray<FString> ADynamicGameState::GetAllFilesInDirectory(const FString directory, const bool fullPath, const FString onlyFilesStartingWith, const FString onlyFilesWithExtension)
{
	// Get all files in directory
	TArray<FString> directoriesToSkip;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, directoriesToSkip, directoriesToSkip, false);
	PlatformFile.IterateDirectory(*directory, Visitor);
	TArray<FString> files;

	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		const FString filePath = TimestampIt.Key();
		const FString fileName = FPaths::GetCleanFilename(filePath);
		bool shouldAddFile = true;

		// Check if filename starts with required characters
		if (!onlyFilesStartingWith.IsEmpty())
		{
			const FString left = fileName.Left(onlyFilesStartingWith.Len());

			if (!(fileName.Left(onlyFilesStartingWith.Len()).Equals(onlyFilesStartingWith)))
				shouldAddFile = false;
		}

		// Check if file extension is required characters
		if (!onlyFilesWithExtension.IsEmpty())
		{
			if (!(FPaths::GetExtension(fileName, false).Equals(onlyFilesWithExtension, ESearchCase::IgnoreCase)))
				shouldAddFile = false;
		}

		// Add full path to results
		if (shouldAddFile)
		{
			files.Add(fullPath ? filePath : fileName);
		}
	}

	return files;
}

void ADynamicGameState::Tick( float DeltaSeconds )
{

	// The GameState itself doesn't actually do anything on each tick
	// Instead, the VRCharacter controls what is displayed
	
}

void ADynamicGameState::LoadNext( FVector CharacterLocation, int Index, FRotator LocalRotation )
{
	UE_LOG(LogTemp, Warning, TEXT("The number of points in GlobalMap Octree is %i"), GlobalMap->GetNumVisiblePoints());
	
	FColor AppliedColor = FColor(250, 0, 0, 100);
	bool ApplyLimited = false;
	//GlobalMap->ApplyColorToAllPoints(AppliedColor, ApplyLimited);
	GlobalMap->RefreshRendering();
	GlobalMap->LoadAllNodes();
	UE_LOG(LogTemp, Warning, TEXT("FULLY LOADED: %i"), GlobalMap->IsFullyLoaded());

	
	if (GlobalMap->GetNumPoints() > 10000)
	{
		int counter = 0;

		//GlobalMap->GetPoints(Points, 0, GlobalMap->GetNumPoints());
		GlobalMap->GetPoints(Points);

		
		for (auto it : Points)
		{
			//it->Color.R = 255;
			//it->Color.A = 100;

			if (counter == 1000)
			{
				UE_LOG(LogTemp, Warning, TEXT("The red channel after setting it to 255 is %i"), it->Color.R);
			}

			counter += 1;
		}

		UE_LOG(LogTemp, Warning, TEXT("The red channel is %i"), Points[1000]->Color.R);

		//GlobalMap->SetData(Points);

		

		Points.Empty();

	}

	
	// Set the point clouds first
	GlobalMapActor->SetPointCloud(GlobalMap);
	DynamicActor->SetPointCloud(LoadedPointClouds[Index]);

	// THEN, set the locations of the point clouds using OFFSET
	LoadedPointClouds[Index]->SetLocationOffset(LoadedPointClouds[Index]->OriginalCoordinates);
	GlobalMap->SetLocationOffset(GlobalMap->OriginalCoordinates);

	// Set the parameters for the local point cloud
	DynamicActor->GetPointCloudComponent()->IntensityInfluence = 0.9;
	DynamicActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::None;
	
	// Set the location and rotation of the local point cloud
	// The LIDAR point cloud is shifted 150 CM up from the character so it aligns with the global point cloud
	DynamicActor->SetActorLocationAndRotation(CharacterLocation + FVector(0.0, 0.0, 150.0), LocalRotation, false, 0, ETeleportType::None);	

}