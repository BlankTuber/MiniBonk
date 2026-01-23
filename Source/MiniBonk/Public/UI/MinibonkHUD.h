#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinibonkHUD.generated.h"

class UCardSelectionWidget;
class UGameOverWidget;
class UAbilityCardLibrary;
class UAbilityManagerComponent;
class UDataTable;
class UProgressBar;
class UTextBlock;
struct FGeneratedCard;

UCLASS()
class MINIBONK_API UMinibonkHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void InitializeHUD(UAbilityManagerComponent* InAbilityManager);

	UFUNCTION()
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void UpdateCoins(int32 CurrentCoins, int32 Delta);

	UFUNCTION()
	void UpdateXP(int32 CurrentXP, int32 XPForNextLevel, int32 CurrentLevel);

	UFUNCTION()
	void OnLevelUp(int32 NewLevel, int32 XPForNextLevel);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void TriggerCardSelection(const FText& Title);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowInteractionPrompt(const FText& PromptText, bool bCanAfford);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideInteractionPrompt();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameOver(int32 Kills, float TimeSurvived, int32 LevelReached, int32 CoinsCollected);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCardSelectionWidget> CardSelectionWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinExplainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> XPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractionPromptText;

	UPROPERTY(EditDefaultsOnly, Category = "Cards")
	TObjectPtr<UDataTable> CardDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FLinearColor CanAffordColor = FLinearColor::Green;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FLinearColor CannotAffordColor = FLinearColor::Red;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGameOverWidget> GameOverWidget;

private:
	UPROPERTY()
	TObjectPtr<UAbilityCardLibrary> CardLibrary;

	UPROPERTY()
	TObjectPtr<UAbilityManagerComponent> AbilityManager;

	int32 CurrentPlayerLevel = 1;

	UFUNCTION()
	void OnCardSelected(const FGeneratedCard& SelectedCard);

	void ShowCardSelection(const FText& Title);
	void HideCardSelection();
};