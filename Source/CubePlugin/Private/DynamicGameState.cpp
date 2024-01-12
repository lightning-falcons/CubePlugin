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

}

void ADynamicGameState::BeginPlay()
{
	Super::BeginPlay();

	// ClockTime = 0;
	ScanIndex = -1;

	// ULevel* Level = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	
	const FString PathToFile = FString("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Data\\Global Point Cloud.las");
	//const FString PathToFile = FString("C:\\Users\\admin\\Downloads\\colour.las");
	//const FString PathToFile = FString("C:\\Users\\admin\\cudal_short_parsed.las");


	// Create an actor for the global and local maps
	GlobalMapActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor( ALidarPointCloudActor::StaticClass() ));
	DynamicActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor(ALidarPointCloudActor::StaticClass()));

	// Create the Global Map Point Cloud from the file
	GlobalMap = ULidarPointCloud::CreateFromFile(PathToFile);

	// Specify that the global point cloud should be displayed with colour source from data and with the particular point size
	GlobalMapActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::Data;
	GlobalMapActor->GetPointCloudComponent()->PointSize = 0.05;

	DynamicActor->GetPointCloudComponent()->PointSize = 1;


	// Now we need to import the local maps
	//FString directoryToSearch = TEXT("C:\\Users\\admin\\Desktop\\Sequence_Cudal");

	FString directoryToSearch = TEXT("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Data\\Local Point Clouds");
	FString filesStartingWith = TEXT("");
	FString fileExtensions = TEXT("las");

	// Get all the las files
	filesInDirectory = GetAllFilesInDirectory(directoryToSearch, true, filesStartingWith, fileExtensions);

	// Select every nth file, limit to m files
	filesInDirectory = ExtractEvery(filesInDirectory, DownSamplePer, 600);

	// Load each point cloud and also get the time stamp
	for (auto it : filesInDirectory)
	{
		LoadedPointClouds.Add(ULidarPointCloud::CreateFromFile(it));
		double OccurrenceTime = FCString::Atod(*FPaths::GetBaseFilename(it));
		TimeStamp.Add(OccurrenceTime);
		//UE_LOG(LogTemp, Warning, TEXT("Time: %lf"), OccurrenceTime);
	}

	// Set the start time to the first time stamp
	ClockTime = TimeStamp[0];

	// Search for all landscape components
	TArray<AActor*> FoundLandscapeStreamingProxy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandscapeStreamingProxy::StaticClass(), FoundLandscapeStreamingProxy);

	// Delete those landscape components that are found
	for (auto it : FoundLandscapeStreamingProxy)
	{
		it->Destroy();
	}

	// Search for all camera actors
	TArray<AActor*> Camera;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Camera);

	// Delete those camera actors that are found
	for (auto it : Camera)
	{
		it->Destroy();
	}

	// Make the world dark
	TArray<AActor*> DirectionalLight;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), DirectionalLight);
	((ADirectionalLight*)DirectionalLight[0])->SetLightColor(FLinearColor(0, 0, 0));


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
	
	g_num_mutex.lock();

	// The GameState itself doesn't actually do anything on each tick
	// Instead, the VRCharacter controls what is displayed
	//UE_LOG(LogTemp, Warning, TEXT("GAME STATE TICK %lf %lf %f"), ClockTime, TimeStamp[ScanIndex], DeltaSeconds);

	// Local tracking of passed time
	ClockTime += DeltaSeconds * 0.6;

	//UE_LOG(LogTemp, Warning, TEXT("GAME STATE TICK %lf %lf %f"), ClockTime, TimeStamp[ScanIndex], DeltaSeconds);

	// Set the point clouds first
	GlobalMapActor->SetPointCloud(GlobalMap);
	SetColor(FColor(255, 255, 255, 0.5), GlobalMap);
	GlobalMap->SetLocationOffset(GlobalMap->OriginalCoordinates);

	// Check if the clock time is such that the next frame should be loaded
	if (ClockTime > TimeStamp[ScanIndex + 1])
	{
		// Load the next frame
		ScanIndex += 1;

		UE_LOG(LogTemp, Warning, TEXT("NEXT FRAME LOADING GAME STATE (LIDAR)"));

		// Get the rotator, which describes the orientation of the character should face
		FRotator rot = FRotator(-Odometry[ScanIndex][4], -Odometry[ScanIndex][5], Odometry[ScanIndex][3]);

		// The local scan is loaded with the same position and orientation as the character
		LoadNext(FVector(Odometry[ScanIndex][0], -Odometry[ScanIndex][1], Odometry[ScanIndex][2]), ScanIndex, rot);

		// Print the time discrepancy
		// double DiscrepantTime = Odometry[ScanIndex][6] - TimeStamp[ScanIndex];
		// UE_LOG(LogTemp, Warning, TEXT("DISCREPANT TIME %lf"), DiscrepantTime);

	}

	g_num_mutex.unlock();
	
}

void ADynamicGameState::LoadNext( FVector CharacterLocation, int Index, FRotator LocalRotation )
{

	std::ofstream myfile;
	// myfile.open("C:\\Users\\admin\\Downloads\\EXAMPLECRASHDATA.txt");
	// myfile << std::string(TCHAR_TO_UTF8(*(filesInDirectory[Index])));
	// myfile << std::string("%i %i", Index, LoadedPointClouds.Num());
	// myfile.close();

	UE_LOG(LogTemp, Warning, TEXT("ARRAY TRACKER %d, %d"), Index, LoadedPointClouds.Num());

	try {

		if (LoadedPointClouds[Index]->IsFullyLoaded() != true)
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT FULLY LOADED"));
		//	return;
		}

		DynamicActor->SetPointCloud(LoadedPointClouds[Index]);


		// THEN, set the locations of the point clouds using OFFSET
		LoadedPointClouds[Index]->SetLocationOffset(LoadedPointClouds[Index]->OriginalCoordinates + FVector(0, 0, 190));

		// Get the individual points
		LoadedPointClouds[Index]->GetPoints(Points);

		// Iterate through the points
		/*
		for (auto it : Points)
		{

			// Apply the location to each point
			it->Location += FVector3f(0.f, 0.f, 190.f);

		}
	

		Points.Empty();

		*/
	
	} catch ( ... ) {
		UE_LOG(LogTemp, Warning, TEXT("CAUGHT"));
		// nothing 
	}

	// Set the parameters for the local point cloud
	DynamicActor->GetPointCloudComponent()->IntensityInfluence = 0.9;
	DynamicActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::Data;
	
	// Set the location and rotation of the local point cloud
	// The LIDAR point cloud is shifted 190 CM up from the character so it aligns with the global point cloud
	DynamicActor->SetActorLocationAndRotation(FVector(CharacterLocation), LocalRotation, false, 0, ETeleportType::None);

}

void ADynamicGameState::SetColor(FColor AppliedColor, ULidarPointCloud* Map)
{
	// Set colour for visible points only
	bool ApplyLimited = false;

	// Apply the colour
	GlobalMap->ApplyColorToAllPoints(AppliedColor, ApplyLimited);

	// Get the individual points
	GlobalMap->GetPoints(Points);

	// Iterate through the points
	for (auto it : Points)
	{

		// Apply the colour to each point
		it->Color = AppliedColor;

	}

	// Clear the points array
	Points.Empty();

}
