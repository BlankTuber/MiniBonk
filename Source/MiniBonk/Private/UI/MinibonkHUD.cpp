#include "UI/MinibonkHUD.h"
#include "UI/CardSelectionWidget.h"
#include "Systems/AbilityCardLibrary.h"
#include "Systems/AbilityManagerComponent.h"
#include "Systems/AbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMinibonkHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// Start with card selection hidden
	if (CardSelectionWidget)
	{
		CardSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
		CardSelectionWidget->OnCardSelected.AddDynamic(this, &UMinibonkHUD::OnCardSelected);
	}

	// Create the card library
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

	ShowCardSelection();
}

void UMinibonkHUD::ShowCardSelection()
{
	if (!CardLibrary || !CardSelectionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("MinibonkHUD: Cannot show card selection - missing CardLibrary or Widget"));
		return;
	}

	// Generate card choices
	TArray<FGeneratedCard> Cards = CardLibrary->GenerateCardChoices(CurrentPlayerLevel);

	if (Cards.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MinibonkHUD: No cards generated"));
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		return;
	}

	// Populate and show the widget
	CardSelectionWidget->SetCardChoices(Cards);
	CardSelectionWidget->SetVisibility(ESlateVisibility::Visible);

	// Show mouse cursor and enable UI input
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

	// Restore game input
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	// Unpause the game
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