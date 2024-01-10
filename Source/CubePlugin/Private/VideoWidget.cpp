#include "VideoWidget.h"
#include "Components/TextBlock.h"

void UVideoWidget::NativeConstruct()
{
	
	Super::NativeConstruct();

	// ItemTitle can be nullptr if we haven't created it in the
	// Blueprint subclass
	/*
	if (ItemTitle)
	{
		ItemTitle->SetText(FText::FromString(TEXT("Hello world!")));
	}
	*/
	
}