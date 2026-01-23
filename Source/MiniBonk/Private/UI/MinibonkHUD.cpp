#include "UI/MinibonkHUD.h"
#include "UI/CardSelectionWidget.h"
#include "UI/GameOverWidget.h"
#include "Systems/AbilityCardLibrary.h"
#include "Systems/AbilityManagerComponent.h"
#include "Systems/AbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMinibonkHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (CardSelectionWidget)
	{
		CardSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
		CardSelectionWidget->OnCardSelected.RemoveDynamic(this, &UMinibonkHUD::OnCardSelected);
		CardSelectionWidget->OnCardSelected.AddDynamic(this, &UMinibonkHUD::OnCardSelected);
	}

	if (GameOverWidget)
	{
		GameOverWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InteractionPromptText)
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
	}

	CardLibrary = NewObject<UAbilityCardLibrary>(this);
}

void UMinibonkHUD::InitializeHUD(UAbilityManagerComponent* InAbilityManager)
{
	AbilityManager = InAbilityManager;

	if (CardLibrary && CardDataTable && AbilityManager)
	{
		CardLibrary->Initialize(CardDataTable, AbilityManager);
		UE_LOG(LogTemp, Log, TEXT("MinibonkHUD: Initialized with CardDataTable and AbilityManager"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MinibonkHUD: Missing CardDataTable or AbilityManager"));
	}
}

void UMinibonkHUD::OnLevelUp(int32 NewLevel, int32 XPForNextLevel)
{
	CurrentPlayerLevel = NewLevel;
	UE_LOG(LogTemp, Log, TEXT("MinibonkHUD: Level up to %d!"), NewLevel);

	ShowCardSelection(FText::FromString(TEXT("Level Up!")));
}

void UMinibonkHUD::TriggerCardSelection(const FText& Title)
{
	ShowCardSelection(Title);
}

void UMinibonkHUD::ShowCardSelection(const FText& Title)
{
	if (!CardLibrary || !CardSelectionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("MinibonkHUD: Cannot show card selection - missing CardLibrary or Widget"));
		return;
	}

	TArray<FGeneratedCard> Cards = CardLibrary->GenerateCardChoices(CurrentPlayerLevel);

	if (Cards.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MinibonkHUD: No cards generated"));
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		return;
	}

	CardSelectionWidget->SetCardChoices(Cards);
	CardSelectionWidget->SetTitle(Title);
	CardSelectionWidget->SetVisibility(ESlateVisibility::Visible);

	HideInteractionPrompt();

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void UMinibonkHUD::HideCardSelection()
{
	if (CardSelectionWidget)
	{
		CardSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void UMinibonkHUD::OnCardSelected(const FGeneratedCard& SelectedCard)
{
	UE_LOG(LogTemp, Log, TEXT("MinibonkHUD: Card selected - %s"), *SelectedCard.CardName.ToString());

	if (AbilityManager)
	{
		AbilityManager->CollectCard(SelectedCard);
	}

	HideCardSelection();
}

void UMinibonkHUD::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.f)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void UMinibonkHUD::UpdateCoins(int32 CurrentCoins, int32 Delta)
{
	if (CoinText)
	{
		CoinText->SetText(FText::AsNumber(CurrentCoins));
	}
}

void UMinibonkHUD::UpdateXP(int32 CurrentXP, int32 XPForNextLevel, int32 CurrentLevel)
{
	if (XPBar && XPForNextLevel > 0)
	{
		XPBar->SetPercent(static_cast<float>(CurrentXP) / static_cast<float>(XPForNextLevel));
	}

	if (LevelText)
	{
		LevelText->SetText(FText::Format(INVTEXT("Lv {0}"), CurrentLevel));
	}
}

void UMinibonkHUD::ShowInteractionPrompt(const FText& PromptText, bool bCanAfford)
{
	if (!InteractionPromptText)
	{
		return;
	}

	InteractionPromptText->SetText(PromptText);
	InteractionPromptText->SetColorAndOpacity(FSlateColor(bCanAfford ? CanAffordColor : CannotAffordColor));
	InteractionPromptText->SetVisibility(ESlateVisibility::Visible);
}

void UMinibonkHUD::HideInteractionPrompt()
{
	if (InteractionPromptText)
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMinibonkHUD::ShowGameOver(int32 Kills, float TimeSurvived, int32 LevelReached, int32 CoinsCollected)
{
	// Hide regular HUD elements
	if (CardSelectionWidget)
	{
		CardSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (HealthBar)
	{
		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CoinExplainer)
	{
		CoinExplainer->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CoinText)
	{
		CoinText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (XPBar)
	{
		XPBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (LevelText)
	{
		LevelText->SetVisibility(ESlateVisibility::Collapsed);
	}

	HideInteractionPrompt();

	// Show game over
	if (GameOverWidget)
	{
		GameOverWidget->SetStats(Kills, TimeSurvived, LevelReached, CoinsCollected);
		GameOverWidget->SetVisibility(ESlateVisibility::Visible);
	}

	// Enable mouse cursor
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}
