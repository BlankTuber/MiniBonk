#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Systems/AbilityTypes.h"
#include "Components/TextBlock.h"
#include "CardSelectionWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelected, const FGeneratedCard&, SelectedCard);

UCLASS()
class MINIBONK_API UCardSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Card Selection")
	void SetCardChoices(const TArray<FGeneratedCard>& Cards);

	UPROPERTY(BlueprintAssignable, Category = "Card Selection")
	FOnCardSelected OnCardSelected;

	UFUNCTION(BlueprintCallable, Category = "Card Selection")
	void SetTitle(const FText& Title);

protected:
	virtual void NativeConstruct() override;

	// Card buttons
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Card0Button;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Card1Button;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Card2Button;

	// Card name labels
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card0Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card1Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card2Name;

	// Card description labels
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card0Description;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card1Description;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card2Description;

	// Card value labels
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card0Value;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card1Value;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Card2Value;

	// Title text
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;

private:
	UPROPERTY()
	TArray<FGeneratedCard> CurrentCards;

	UFUNCTION()
	void OnCard0Clicked();

	UFUNCTION()
	void OnCard1Clicked();

	UFUNCTION()
	void OnCard2Clicked();

	void SelectCard(int32 Index);
	void UpdateCardDisplay(int32 Index, const FGeneratedCard& Card);
	FString FormatCardValue(const FGeneratedCard& Card) const;
};