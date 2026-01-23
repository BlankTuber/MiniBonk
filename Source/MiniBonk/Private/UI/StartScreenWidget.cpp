#include "UI/StartScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UStartScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UStartScreenWidget::OnPlayClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UStartScreenWidget::OnQuitClicked);
	}
}

void UStartScreenWidget::OnPlayClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), GameLevelName);
}

void UStartScreenWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}