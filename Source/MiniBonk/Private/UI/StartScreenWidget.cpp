#include "UI/StartScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UStartScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (PlayButton)
    {
        PlayButton->OnPressed.RemoveDynamic(this, &UStartScreenWidget::OnPlayPressed);
        PlayButton->OnPressed.AddDynamic(this, &UStartScreenWidget::OnPlayPressed);
    }

    if (QuitButton)
    {
        QuitButton->OnPressed.RemoveDynamic(this, &UStartScreenWidget::OnQuitPressed);
        QuitButton->OnPressed.AddDynamic(this, &UStartScreenWidget::OnQuitPressed);
    }
}

void UStartScreenWidget::OnPlayPressed()
{
    UE_LOG(LogTemp, Error, TEXT("########## OnPlayPressed FIRED ##########"));
    UGameplayStatics::OpenLevel(GetWorld(), GameLevelName);
}

void UStartScreenWidget::OnQuitPressed()
{
    UE_LOG(LogTemp, Error, TEXT("########## OnQuitPressed FIRED ##########"));
    UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}