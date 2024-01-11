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

	for (auto it : filesInDirectory)
	{
		UTexture2D* Texture = FImageUtils::ImportFileAsTexture2D(it);
		ImageTextures.Add(Texture);
		double OccurrenceTime = FCString::Atod(*FPaths::GetBaseFilename(it)) / 1000000000;
		TimeStamp.Add(OccurrenceTime);
	}

	// ItemTitle can be nullptr if we haven't created it in the
	// Blueprint subclass
	
	/*
	if (Image)
	{
		FString Path = FString("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Content\\VR.uasset");

		UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));

		Image->SetVisibility(ESlateVisibility::Visible);
		Image->SetBrushFromTexture(Texture);
		Image->SetOpacity(50.5f);

	}
	*/
	
	ImageIndex = 0;
	
}

void UVideoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	// Your Stuff Goes Here

	double ClockTime = GetWorld()->GetGameState<ADynamicGameState>()->ClockTime;

	if (Image && ClockTime > TimeStamp[ImageIndex + 1])
	{
		UE_LOG(LogTemp, Warning, TEXT("TICKING..."));

		ImageIndex += 1;

		//FString Path = FString("C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Content\\VR.uasset");
		//FString FilePath = "C:\\Users\\its\\Downloads\\VR.jpg";

		// UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));
		//UTexture2D* Texture = FImageUtils::ImportFileAsTexture2D(FilePath);

		Image->SetVisibility(ESlateVisibility::Visible);
		Image->SetBrushFromTexture(ImageTextures[ImageIndex]);
		Image->SetOpacity(50.5f);

	}


}

