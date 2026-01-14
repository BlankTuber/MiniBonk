#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinibonkHUD.generated.h"

class UCardSelectionWidget;
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

protected:
	virtual void NativeConstruct() override;

	// Card selection widget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCardSelectionWidget> CardSelectionWidget;

	// HUD Elements
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> XPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	// Data table containing card templates
	UPROPERTY(EditDefaultsOnly, Category = "Cards")
	TObjectPtr<UDataTable> CardDataTable;

private:
	UPROPERTY()
	TObjectPtr<UAbilityCardLibrary> CardLibrary;

	UPROPERTY()
	TObjectPtr<UAbilityManagerComponent> AbilityManager;

	int32 CurrentPlayerLevel = 1;

	UFUNCTION()
	void OnCardSelected(const FGeneratedCard& SelectedCard);

	void ShowCardSelection();
	void HideCardSelection();
};