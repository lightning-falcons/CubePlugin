// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicGameState.h"
#include "VrCharacter.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

ADynamicGameState::ADynamicGameState()
{
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Read the file
	data = toml::parse("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin 5.3\\config.toml");
	loading = toml::find(data, "Loading");
	params = toml::find(data, "VR_Parameters");

	// Need in constructor, should be one the first things loaded
	NumberFrames = toml::find<int>(params, "number_frames");

	// Set the playback speed ratio
	PlaybackSpeed = toml::find<double>(params, "playback_speed");
	PlaybackSpeedConfigured = PlaybackSpeed;

	DownSamplePer = toml::find<int>(params, "import_every");

	PhotoImport = toml::find<bool>(params, "photo_import");

	LoadGlobal = toml::find<bool>(params, "load_global");
	LoadOne = toml::find<bool>(params, "load_one");



	PitchCorrection = toml::find<double>(loading, "pitch_correction");
	YawCorrection = toml::find<double>(loading, "yaw_correction");
	RollCorrection = toml::find<double>(loading, "roll_correction");

	


	// Set the bird's eye view height
	HeightBirds = toml::find<double>(params, "height_birds");
	UE_LOG(LogTemp, Warning, TEXT("[TOML] The bird's eye view height is set as : %lf "), HeightBirds);


	// Odometry Data Path
	std::string DataPath = toml::find<std::string>(loading, "data_path");
	std::string OdometryPath = toml::find<std::string>(loading, "odometry_name");

	FullOdometryPath = DataPath;
	FullOdometryPath /= OdometryPath;

	// Video Path
	std::string ImagePath = toml::find<std::string>(loading, "image_name");
	
	FullImagePath = DataPath;
	FullImagePath /= ImagePath;

	// Point cloud sizes
	LocalGroundSize = toml::find<double>(params, "local_ground_size");
	LocalBirdSize = toml::find<double>(params, "local_bird_size");
	GlobalGroundSize = toml::find<double>(params, "global_ground_size");
	GlobalBirdSize = toml::find<double>(params, "global_bird_size");

}


void ADynamicGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[TOML] The odometry source is : %s "), *FString(FullOdometryPath.c_str()));

	/*
	std::string OdometryPath = toml::find<std::string>(loading, "odometry_name");
	auto GlobalPath = toml::find<std::string>(loading, "global_name");
	auto LocalPath = toml::find<std::string>(loading, "local_name");
	auto ImagePath = toml::find<std::string>(loading, "image_name");

	auto NumberFrames = toml::find<int>(params, "number_frames");
	auto ImportEvery = toml::find<int>(params, "import_every");
	auto PlaybackSpeed = toml::find<double>(params, "playback_speed");
	auto PhotoImport = toml::find<bool>(params, "photo_import");


	UE_LOG(LogTemp, Warning, TEXT("[TOML] The odometry source is : %s "), *FString(OdometryPath.c_str()));
	UE_LOG(LogTemp, Warning, TEXT("[TOML] The global source is : %s "), *FString(GlobalPath.c_str()));
	UE_LOG(LogTemp, Warning, TEXT("[TOML] The local source is : %s "), *FString(LocalPath.c_str()));
	UE_LOG(LogTemp, Warning, TEXT("[TOML] The image source is : %s "), *FString(ImagePath.c_str()));

	UE_LOG(LogTemp, Warning, TEXT("[TOML] The number of frames is : %d "), NumberFrames);
	UE_LOG(LogTemp, Warning, TEXT("[TOML] The import every is : %d "), ImportEvery);
	UE_LOG(LogTemp, Warning, TEXT("[TOML] The playback speed is : %d "), PlaybackSpeed);
	UE_LOG(LogTemp, Warning, TEXT("[TOML] The photo import is : %d "), PhotoImport);
	*/

	// Parse the data
	// ClockTime = 0;
	ScanIndex = -1;

	// ULevel* Level = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	std::string DataPath = toml::find<std::string>(loading, "data_path");

	// Create an actor for the global and local maps
	GlobalMapActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor(ALidarPointCloudActor::StaticClass()));
	DynamicActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor(ALidarPointCloudActor::StaticClass()));
	DynamicActorInterlaced = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor(ALidarPointCloudActor::StaticClass()));

	if (LoadGlobal)
	{
		std::string GlobalPath = toml::find<std::string>(loading, "global_name");
		std::filesystem::path FullGlobalPath = DataPath;
		FullGlobalPath /= GlobalPath;
		const FString PathToFile = FString(FullGlobalPath.c_str());
	

	//const FString PathToFile = FString("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Data\\Global Point Cloud.las");
	//const FString PathToFile = FString("C:\\Users\\admin\\Downloads\\colour.las");
	//const FString PathToFile = FString("C:\\Users\\admin\\cudal_short_parsed.las");

		// Create the Global Map Point Cloud from the file
		GlobalMap = ULidarPointCloud::CreateFromFile(PathToFile);

		// Specify that the global point cloud should be displayed with colour source from data and with the particular point size
		GlobalMapActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::Data;
		GlobalMapActor->GetPointCloudComponent()->PointSize = 0.05;
	}

	DynamicActor->GetPointCloudComponent()->PointSize = 1;
	DynamicActorInterlaced->GetPointCloudComponent()->PointSize = 1;



	// Now we need to import the local maps
	//FString directoryToSearch = TEXT("C:\\Users\\admin\\Desktop\\Sequence_Cudal");
	UE_LOG(LogTemp, Warning, TEXT("THE VALUE OF LoadOne IS %d"), LoadOne);
	if (!LoadOne)
	{

		std::string LocalPath = toml::find<std::string>(loading, "local_name");
		std::filesystem::path FullLocalPath = DataPath;
		FullLocalPath /= LocalPath;

		FString directoryToSearch = FString(FullLocalPath.c_str());
		//FString directoryToSearch = TEXT("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Data\\Local Point Clouds");
		FString filesStartingWith = TEXT("");
		FString fileExtensions = TEXT("las");

		// Get all the las files
		filesInDirectory = GetAllFilesInDirectory(directoryToSearch, true, filesStartingWith, fileExtensions);

		// Select every nth file, limit to m files
		filesInDirectory = ExtractEvery(filesInDirectory, DownSamplePer, NumberFrames);

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
	}
	else
	{
		// We are loading only one point cloud
		std::string SingleName = toml::find<std::string>(loading, "single_name");
		std::filesystem::path FullSingleName = DataPath;
		FullSingleName /= SingleName;

		LoadedPointClouds.Add(ULidarPointCloud::CreateFromFile(FString(FullSingleName.c_str())));

		TimeStamp.Add(0.0);
		ClockTime = TimeStamp[0];
	}


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

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVrCharacter::StaticClass(), FoundActors);
	AVrCharacter* ControlledCharacter = (AVrCharacter*)FoundActors[0];

	if (GlobalMap)
	{
		// Set the point clouds first
		GlobalMapActor->SetPointCloud(GlobalMap);
		SetColor(FColor(255, 255, 255, 0.5), GlobalMap);
		GlobalMap->SetLocationOffset(GlobalMap->OriginalCoordinates);

		// Correction to ensure odometry-global map alignment
		GlobalMapActor->SetActorRotation(FRotator(PitchCorrection, YawCorrection, RollCorrection));
	}

	

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

void ADynamicGameState::Tick(float DeltaSeconds)
{

	g_num_mutex.lock();

	if (LoadOne && SingleCloudLoaded > 0)
	{
		LoadNext(FVector(0, 0, 0), 0, FRotator(0, 0, 0), DynamicActor);
		SingleCloudLoaded -= 1;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVrCharacter::StaticClass(), FoundActors);
	AVrCharacter* ControlledCharacter = (AVrCharacter*)FoundActors[0];

	double Exposure = toml::find<double>(params, "exposure");
	ControlledCharacter->Exposure = Exposure;

	// Set the PlayBack speed depending on whether the VR Character is stopped or moving
	if (ControlledCharacter->Motion == STOPPED)
	{
		PlaybackSpeed = 0.0;
	}
	else if (ControlledCharacter->Motion == MOVING)
	{
		PlaybackSpeed = PlaybackSpeedConfigured;
	}

	// The GameState itself doesn't actually do anything on each tick
	// Instead, the VRCharacter controls what is displayed
	//UE_LOG(LogTemp, Warning, TEXT("GAME STATE TICK %lf %lf %f"), ClockTime, TimeStamp[ScanIndex], DeltaSeconds);

	// Local tracking of passed time
	ClockTime += DeltaSeconds * PlaybackSpeed;

	//UE_LOG(LogTemp, Warning, TEXT("GAME STATE TICK %lf %lf %f"), ClockTime, TimeStamp[ScanIndex], DeltaSeconds);



	// Hide all actors within 1 m of the camera

	TFunction<void(FLidarPointCloudPoint* Point)> pFuncRef = [this](FLidarPointCloudPoint* Point) { HidePoint(Point); };
	if (DynamicActor)
	{
		DynamicActor->GetPointCloudComponent()->SetVisibilityOfPointsInSphere(false, FSphere(ControlledCharacter->GetActorLocation(), 150.0));
	}

	if (DynamicActorInterlaced)
	{
		DynamicActorInterlaced->GetPointCloudComponent()->SetVisibilityOfPointsInSphere(false, FSphere(ControlledCharacter->GetActorLocation(), 150.0));
	}


	if (ControlledCharacter->CurrentView == BIRDVIEW)
	{
		GlobalMapActor->GetPointCloudComponent()->PointSize = GlobalBirdSize;
	}
	else
	{
		GlobalMapActor->GetPointCloudComponent()->PointSize = GlobalGroundSize;
	}

	if (!LoadOne)
	{

		// Check if we are at the last frame included already
		// If so, revert the simulation to the very start
		if (ScanIndex >= TimeStamp.Num() - 2)
		{
			/*
			PlaybackSpeed = 0;
			PlaybackSpeedConfigured = 0;

			for (auto it : LoadedPointClouds)
			{
				it->ConditionalBeginDestroy();
			}
			//LoadedPointClouds.Empty();
			GEngine->ForceGarbageCollection();
			*/

			ResetSimulation();
		}
	}
	

	UE_LOG(LogTemp, Warning, TEXT("[356] THE VALUE OF LoadOne IS %d"), LoadOne);

	// Check if the clock time is such that the next frame should be loaded
	if (!LoadOne)
	{
		if ((ClockTime > TimeStamp[ScanIndex + 1]) || ImmediateReload || ImmediateReloadSecond)
		{
			if (!ImmediateReload)
			{
				// Load the next frame
				ScanIndex += 1;
				ImmediateReloadSecond = false;
			}
			else
			{
				ImmediateReload = false;
				ScanIndex -= 1;
			}


			// The Actor to be changed should be different depending on whether the scan is ODD or EVEN
			if (ScanIndex % 2 == 0)
			{
				// Even Scan
				// Change the normal actor

				UE_LOG(LogTemp, Warning, TEXT("NEXT FRAME LOADING GAME STATE (LIDAR) - EVEN"));

				// Get the rotator, which describes the orientation of the character should face
				// FRotator rot = FRotator(Odometry[ScanIndex + 1][4], -Odometry[ScanIndex + 1][5], Odometry[ScanIndex + 1][3]);
				FRotator rot = FRotator(-Odometry[ScanIndex + 1][4], -Odometry[ScanIndex + 1][3], Odometry[ScanIndex + 1][5]);

				UE_LOG(LogTemp, Warning, TEXT("[Point Cloud Rotator - RAW ODOM] YAW %lf PITCH %lf ROLL %lf"), Odometry[ScanIndex + 1][3], Odometry[ScanIndex + 1][4], Odometry[ScanIndex + 1][5]);
				UE_LOG(LogTemp, Warning, TEXT("[Point Cloud Rotator] YAW %lf PITCH %lf ROLL %lf"), rot.Yaw, rot.Pitch, rot.Roll);

				// The local scan is loaded with the same position and orientation as the character
				LoadNext(FVector(Odometry[ScanIndex + 1][0], -Odometry[ScanIndex + 1][1], Odometry[ScanIndex + 1][2]), ScanIndex + 1, rot, DynamicActor);

			}
			else
			{
				// Odd Scan
				// Change the interlaced actor

				UE_LOG(LogTemp, Warning, TEXT("NEXT FRAME LOADING GAME STATE (LIDAR) - ODD"));

				// Get the rotator, which describes the orientation of the character should face
				// FRotator rot = FRotator(Odometry[ScanIndex + 1][4], -Odometry[ScanIndex + 1][5], Odometry[ScanIndex + 1][3]);
				FRotator rot = FRotator(-Odometry[ScanIndex + 1][4], -Odometry[ScanIndex + 1][3], Odometry[ScanIndex + 1][5]);

				// The local scan is loaded with the same position and orientation as the character
				LoadNext(FVector(Odometry[ScanIndex + 1][0], -Odometry[ScanIndex + 1][1], Odometry[ScanIndex + 1][2]), ScanIndex + 1, rot, DynamicActorInterlaced);
			}





			// Print the time discrepancy
			// double DiscrepantTime = Odometry[ScanIndex][6] - TimeStamp[ScanIndex];
			// UE_LOG(LogTemp, Warning, TEXT("DISCREPANT TIME %lf"), DiscrepantTime);
		}

	}

	g_num_mutex.unlock();
	
}

void ADynamicGameState::LoadNext( FVector CharacterLocation, int Index, FRotator LocalRotation, ALidarPointCloudActor* PointCloudActor )
{

	// std::ofstream myfile;
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

		

		PointCloudActor->SetPointCloud(LoadedPointClouds[Index]);


		// THEN, set the locations of the point clouds using OFFSET
		LoadedPointClouds[Index]->SetLocationOffset(LoadedPointClouds[Index]->OriginalCoordinates);

		// Get the individual points
		//LoadedPointClouds[Index]->GetPoints(Points);

		// Iterate through the points
		/*
		for (auto it : Points)
		{

			// UE_LOG(LogTemp, Warning, TEXT("PDIP"));

			// Apply the location to each point
			// it->Location += FVector3f(0.f, 0.f, 190.f);
			// it->Color = FColor(255.f, 0.f, 0.f);

			// it->bVisible = 0;

		}
		*/

	

		Points.Empty();

		
	
	} catch ( ... ) {
		UE_LOG(LogTemp, Warning, TEXT("CAUGHT"));
		// nothing 
	}

	// Set the parameters for the local point cloud
	PointCloudActor->GetPointCloudComponent()->IntensityInfluence = 0.9;
	PointCloudActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::Data;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVrCharacter::StaticClass(), FoundActors);
	AVrCharacter* ControlledCharacter = (AVrCharacter*)FoundActors[0];

	if (ControlledCharacter->CurrentView == BIRDVIEW)
	{
		PointCloudActor->GetPointCloudComponent()->PointSize = LocalBirdSize;
	}
	else
	{
		PointCloudActor->GetPointCloudComponent()->PointSize = LocalGroundSize;
	}
		
	
	// Set the location and rotation of the local point cloud
	// The LIDAR point cloud is shifted 190 CM up from the character so it aligns with the global point cloud
	PointCloudActor->SetActorLocationAndRotation(FVector(CharacterLocation), LocalRotation, false, 0, ETeleportType::None);
	PointCloudActor->AddActorLocalOffset(FVector(0, 0, 190));

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

void ADynamicGameState::SetTime(double Time)
{

	// Find out what the current time is
	double CurrentTime = ClockTime;

	// Need to change the actual time
	// This doesn't matter that much because we are pulling
	// the time from the game state anyways
	ClockTime = Time;

	// Check whether we go back or forwards in time
	if (ClockTime < CurrentTime)
	{
		// We need to go back in time

		// Need to change the ScanIndex (which will then set the position)
		while (ClockTime < Odometry[ScanIndex][6] && ScanIndex > 0)
		{
			ScanIndex -= 1;
		}
	}
	else if (ClockTime > CurrentTime)
	{
		while (ClockTime > Odometry[ScanIndex][6] && ScanIndex < Odometry.Num() - 3)
		{
			ScanIndex += 1;
		}
	}

}

void ADynamicGameState::ResetSimulation()
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVrCharacter::StaticClass(), FoundActors);
	AVrCharacter* ControlledCharacter = (AVrCharacter*)FoundActors[0];

	ControlledCharacter->SetTime(TimeStamp[0]);
	SetTime(TimeStamp[0]);
	UVideoWidget* VideoWidgetFound = (UVideoWidget*)ControlledCharacter->ItemReferences2[0];
	VideoWidgetFound->SetTime(TimeStamp[0]);
}

void ADynamicGameState::HidePoint(FLidarPointCloudPoint* Point)
{
	Point->Color = FColor(255.f, 0.f, 0.f);
	Point->bVisible = 0;
}

