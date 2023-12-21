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

	

	//KatoombaActor = Cast<ALidarPointCloudActor>(GEditor->AddActor(Level, ALidarPointCloudActor::StaticClass(), FTransform()));
	KatoombaActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor( ALidarPointCloudActor::StaticClass() ));
	DynamicActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor(ALidarPointCloudActor::StaticClass()));


	Katoomba = ULidarPointCloud::CreateFromFile(PathToFile);

	

	UE_LOG(LogTemp, Warning, TEXT("Original coords %lf %lf %lf "), (Katoomba->OriginalCoordinates).X, (Katoomba->OriginalCoordinates).Y, (Katoomba->OriginalCoordinates).Z);



	//KatoombaActor->SetPointCloud(Katoomba);

	//KatoombaActor->SetActorLocation(Katoomba->OriginalCoordinates);
	
	//KatoombaActor->GetPointCloudComponent()->IntensityInfluence = 0.9;
	// Katoomba->RestoreOriginalCoordinates();


	UE_LOG(LogTemp, Warning, TEXT("Original coords %lf %lf %lf "), (Katoomba->OriginalCoordinates).X, (Katoomba->OriginalCoordinates).Y, (Katoomba->OriginalCoordinates).Z);


	// KatoombaActor->SetActorLocation(Katoomba->OriginalCoordinates);
	//KatoombaActor->SetActorLocation(FVector(48603.000000, -7325.000000, 1544.500000));

	//UE_LOG(LogTemp, Warning, TEXT(" Centered %d "), Katoomba->IsCentered());



	UE_LOG(LogTemp, Warning, TEXT("Original coords %lf %lf %lf "), (Katoomba->OriginalCoordinates).X, (Katoomba->OriginalCoordinates).Y, (Katoomba->OriginalCoordinates).Z);

	KatoombaActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::Data;
	KatoombaActor->GetPointCloudComponent()->PointSize = 0.25;

	//Katoomba->RestoreOriginalCoordinates();


	//UE_LOG(LogTemp, Warning, TEXT("Original coords %lf %lf %lf "), (Katoomba->OriginalCoordinates).X, (Katoomba->OriginalCoordinates).Y, (Katoomba->OriginalCoordinates).Z);

	//KatoombaActor->SetActorLocation(Katoomba->OriginalCoordinates);

	//KatoombaActor->GetPointCloudComponent()->ElevationColorBottom = FLinearColor(FColor(255, 0, 0));
	//KatoombaActor->GetPointCloudComponent()->ElevationColorTop = FLinearColor(FColor(0, 255, 0));

	
	

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




	//UE_LOG(LogTemp, Warning, TEXT("ACTOR LOC %lf"), Katoomba->OriginalCoordinates[2]);

	TArray< ULidarPointCloud* > LoadedPointCloudsCombined;
	LoadedPointCloudsCombined = LoadedPointClouds;
	LoadedPointCloudsCombined.Add(Katoomba);


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
	
	//UE_LOG(LogTemp, Warning, TEXT("%f"), KatoombaActor->GetPointCloudComponent()->IntensityInfluence);
	//KatoombaActor->GetPointCloudComponent()->IntensityInfluence -= 0.01 * DeltaSeconds;

	ClockTime += DeltaSeconds;

	//UE_LOG(LogTemp, Warning, TEXT("%f"), ClockTime);


	if (ClockTime > 0.1 && ScanIndex < filesInDirectory.Num())
	{


		ClockTime = 0;

		//KatoombaActor->Destroy();

		ULevel* Level = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();

		//const FString PathToFile = FString("C:\\Users\\admin\\Downloads\\TransformedCloud_katoomba2.las");
		const FString PathToFile = filesInDirectory[ScanIndex];

		UE_LOG(LogTemp, Warning, TEXT("DISPLAYING: %s"), *PathToFile);


		//ULidarPointCloud* Katoomba = ULidarPointCloud::CreateFromFile(PathToFile);


		//KatoombaActor = Cast<ALidarPointCloudActor>(GetWorld()->SpawnActor(ALidarPointCloudActor::StaticClass()));
		//DynamicActor->SetPointCloud(LoadedPointClouds[ScanIndex]);
		//DynamicActor->SetActorLocation(LoadedPointClouds[ScanIndex]->OriginalCoordinates + FVector(0, 0, 1500));

		//KatoombaActor->GetPointCloudComponent()->IntensityInfluence = 1;



		ScanIndex += 1;
	}
	else if (filesInDirectory.Num() == ScanIndex)
	{
		ScanIndex = 0;
	}
	
}

void ADynamicGameState::LoadNext( FVector CharacterLocation, int Index, FRotator LocalRotation )
{
	UE_LOG(LogTemp, Warning, TEXT("The number of points in Katoomba Octree is %i"), Katoomba->GetNumVisiblePoints());
	
	FColor AppliedColor = FColor(250, 0, 0, 100);
	bool ApplyLimited = false;
	//Katoomba->ApplyColorToAllPoints(AppliedColor, ApplyLimited);
	Katoomba->RefreshRendering();
	Katoomba->LoadAllNodes();
	UE_LOG(LogTemp, Warning, TEXT("FULLY LOADED: %i"), Katoomba->IsFullyLoaded());

	
	if (Katoomba->GetNumPoints() > 10000)
	{
		int counter = 0;

		//Katoomba->GetPoints(Points, 0, Katoomba->GetNumPoints());
		Katoomba->GetPoints(Points);

		
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

		//Katoomba->SetData(Points);

		

		Points.Empty();

	}

	
	KatoombaActor->SetPointCloud(Katoomba);
	//KatoombaActor->SetActorLocation(Katoomba->OriginalCoordinates);
	//Katoomba->RestoreOriginalCoordinates();

	DynamicActor->SetPointCloud(LoadedPointClouds[Index]);


	LoadedPointClouds[Index]->SetLocationOffset(LoadedPointClouds[Index]->OriginalCoordinates);
	Katoomba->SetLocationOffset(Katoomba->OriginalCoordinates);

	DynamicActor->GetPointCloudComponent()->IntensityInfluence = 0.9;
	DynamicActor->GetPointCloudComponent()->ColorSource = ELidarPointCloudColorationMode::None;
	
	//DynamicActor->SetActorLocation(LoadedPointClouds[Index]->OriginalCoordinates + CharacterLocation);
	//DynamicActor->AddActorLocalRotation(LocalRotation);

	DynamicActor->SetActorLocationAndRotation(CharacterLocation + FVector(0.0, 0.0, 150.0), LocalRotation, false, 0, ETeleportType::None);
	
	//LoadedPointClouds[Index]->RestoreOriginalCoordinates();
	//LoadedPointClouds[Index]->SetLocationOffset(CharacterLocation);

	//TArray< ULidarPointCloud* > PointCloudsToAlign;
	//PointCloudsToAlign.Add(Katoomba);
	//PointCloudsToAlign.Add(LoadedPointClouds[Index]);

	UE_LOG(LogTemp, Warning, TEXT("Original coords DYNAMIC %lf %lf %lf "), (LoadedPointClouds[Index]->OriginalCoordinates).X, (LoadedPointClouds[Index]->OriginalCoordinates).Y, (LoadedPointClouds[Index]->OriginalCoordinates).Z);
	UE_LOG(LogTemp, Warning, TEXT("Original coords STATIC %lf %lf %lf "), (Katoomba->OriginalCoordinates).X, (Katoomba->OriginalCoordinates).Y, (Katoomba->OriginalCoordinates).Z);


	

}