#include "Core/MainMenuGameMode.h"
#include "UI/StartScreenWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	DefaultPawnClass = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());

		if (StartScreenWidgetClass)
		{
			UStartScreenWidget* StartScreen = CreateWidget<UStartScreenWidget>(PC, StartScreenWidgetClass);
			if (StartScreen)
			{
				StartScreen->AddToViewport();
			}
		}
	}
}