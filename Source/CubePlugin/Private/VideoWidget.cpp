#include "VideoWidget.h"
#include "Components/TextBlock.h"

void UVideoWidget::NativeConstruct()
{
	
	Super::NativeConstruct();

	// Import all the textures
	FString directoryToSearch = TEXT("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Data\\Images");
	FString filesStartingWith = TEXT("");
	FString fileExtensions = TEXT("png");	

	// Get all the image files
	TArray<FString> filesInDirectory = ADynamicGameState::GetAllFilesInDirectory(directoryToSearch, true, filesStartingWith, fileExtensions);
	filesInDirectory = ADynamicGameState::ExtractEvery(filesInDirectory, 30, 300);

	// Iterate through the image files and extract them as 2D textures and add to an array
	// Also make sure to extract the time and add to an array
	for (auto it : filesInDirectory)
	{
		UTexture2D* Texture = FImageUtils::ImportFileAsTexture2D(it);
		ImageTextures.Add(Texture);

		// The time is divided by one billion to convert from nanoseconds to seconds
		double OccurrenceTime = FCString::Atod(*FPaths::GetBaseFilename(it)) / 1000000000;
		TimeStamp.Add(OccurrenceTime);
	}
	
	// We start from the first image
	ImageIndex = 0;
	
}

void UVideoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// Get the clock time as stored by the game state
	ClockTime = GetWorld()->GetGameState<ADynamicGameState>()->ClockTime;

	// Check if all the images have been exhausted
	if (ImageIndex >= TimeStamp.Num() - 2)
	{
		GetWorld()->GetGameState<ADynamicGameState>()->ResetSimulation();
	}

	// Change the image shown only if the NEXT image time has been reached
	if (Image && ClockTime > TimeStamp[ImageIndex + 1])
	{
		UE_LOG(LogTemp, Warning, TEXT("TICKING..."));

		// Increment the image used
		ImageIndex += 1;

		// Set the image
		Image->SetVisibility(CurrentVisibility);
		Image->SetBrushFromTexture(ImageTextures[ImageIndex]);
		Image->SetOpacity(50.5f);

	}
}

void UVideoWidget::SetTime(double Time)
{

	// Find out what the current time is
	double CurrentTime = ClockTime;

	// Need to change the actual time
	// This doesn't matter that much because we are pulling
	// the time from the game state anyways
	// ClockTime = Time;

	// Check whether we go back or forwards in time
	if (Time < CurrentTime)
	{
		// We need to go back in time

		// Need to change the ScanIndex (which will then set the position)
		while (Time < TimeStamp[ImageIndex] && ImageIndex > 0)
		{
			ImageIndex -= 1;
		}
	}
	else if (Time > CurrentTime)
	{
		while (Time > TimeStamp[ImageIndex] && ImageIndex < TimeStamp.Num() - 1)
		{
			ImageIndex += 1;
		}
	}

}

void UVideoWidget::ToggleVisibility()
{
	if (CurrentVisibility == ESlateVisibility::Hidden)
	{
		CurrentVisibility = ESlateVisibility::Visible;
	} 
	else if (CurrentVisibility == ESlateVisibility::Visible)
	{
		CurrentVisibility = ESlateVisibility::Hidden;
	}
}