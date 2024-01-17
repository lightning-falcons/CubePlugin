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
	
	UPROPERTY()
	TArray<UTexture2D*> ImageTextures;
	
	TArray<double> TimeStamp;
	int ImageIndex;

	double ClockTime;

	ESlateVisibility CurrentVisibility = ESlateVisibility::Visible;

public:
	void SetTime(double Time);

	void ToggleVisibility();

};