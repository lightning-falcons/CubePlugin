#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "VideoWidget.generated.h"

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
};