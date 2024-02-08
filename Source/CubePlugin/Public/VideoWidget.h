#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ImageUtils.h"
#include "DynamicGameState.h"
#include "VideoWidget.generated.h"

enum VisibilityType {HIDDEN, VISIBLE};

UCLASS(Abstract)
class UVideoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//class UTextBlock* ItemTitle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* Image;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
	// Array of 2D textures, which are the images to be played over time
	UPROPERTY()
	TArray<UTexture2D*> ImageTextures;
	
	// Array of the timestamps corresponding to those images
	TArray<double> TimeStamp;

	// The array index we are up to currently for the images
	int ImageIndex;

	// Current time
	double ClockTime;

public:

	// Set to store the current visibility status of video
	ESlateVisibility CurrentVisibility = ESlateVisibility::Visible;

	// Set the playback time and adjust the scene to match that time
	void SetTime(double Time);

	// Toggle between video hidden and visible
	void ToggleVisibility();

	// Flag to reset the current image, usually after changing the image index
	bool ImmediateReload = false;

};