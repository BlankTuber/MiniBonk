#include "UI/GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnQuitClicked);
	}
}

void UGameOverWidget::SetStats(int32 Kills, float TimeSurvived, int32 LevelReached, int32 CoinsCollected)
{
	if (KillsText)
	{
		KillsText->SetText(FText::AsNumber(Kills));
	}

	if (TimeText)
	{
		TimeText->SetText(FText::FromString(FormatTime(TimeSurvived)));
	}

	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(LevelReached));
	}

	if (CoinsText)
	{
		CoinsText->SetText(FText::AsNumber(CoinsCollected));
	}
}

FString UGameOverWidget::FormatTime(float Seconds) const
{
	int32 Minutes = FMath::FloorToInt(Seconds / 60.f);
	int32 Secs = FMath::FloorToInt(FMath::Fmod(Seconds, 60.f));
	return FString::Printf(TEXT("%d:%02d"), Minutes, Secs);
}

void UGameOverWidget::OnRestartClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()));
}

void UGameOverWidget::OnQuitClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}