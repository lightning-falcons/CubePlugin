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
	filesInDirectory = ADynamicGameState::ExtractEvery(filesInDirectory, 5, 200);

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
	double ClockTime = GetWorld()->GetGameState<ADynamicGameState>()->ClockTime;

	// Change the image shown only if the NEXT image time has been reached
	if (Image && ClockTime > TimeStamp[ImageIndex + 1])
	{
		UE_LOG(LogTemp, Warning, TEXT("TICKING..."));

		// Increment the image used
		ImageIndex += 1;

		// Set the image
		Image->SetVisibility(ESlateVisibility::Visible);
		Image->SetBrushFromTexture(ImageTextures[ImageIndex]);
		Image->SetOpacity(50.5f);
		


	}


}

