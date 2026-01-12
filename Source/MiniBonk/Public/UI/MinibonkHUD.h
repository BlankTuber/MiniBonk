#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinibonkHUD.generated.h"

class UCardSelectionWidget;
class UAbilityCardLibrary;
class UAbilityManagerComponent;
class UDataTable;
struct FGeneratedCard;

UCLASS()
class MINIBONK_API UMinibonkHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void InitializeHUD(UAbilityManagerComponent* InAbilityManager);

	UFUNCTION()
	void OnLevelUp(int32 NewLevel, int32 XPForNextLevel);

protected:
	virtual void NativeConstruct() override;

	// Card selection widget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCardSelectionWidget> CardSelectionWidget;

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