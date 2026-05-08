#include "MainHUD.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentLevelName = GetWorld()->GetMapName();
	if (CurrentLevelName.Contains(TEXT("TitleLevel")) ||
		CurrentLevelName.Contains(TEXT("MainMenuLevel")))
	{
		return;
	}

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}

