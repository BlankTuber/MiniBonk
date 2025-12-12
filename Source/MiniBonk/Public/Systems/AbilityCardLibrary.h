#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Systems/AbilityTypes.h"
#include "AbilityCardLibrary.generated.h"

class UDataTable;
class UAbilityManagerComponent;

// Generates scaled cards from templates with progressive chance of "rare card"
UCLASS()
class MINIBONK_API UAbilityCardLibrary : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UDataTable* InCardDataTable, UAbilityManagerComponent* InAbilityManager);

	// Generate cards for upgrade selection
	TArray<FGeneratedCard> GenerateCardChoices(int32 PlayerLevel);

protected:
	UPROPERTY()
	TObjectPtr<UDataTable> CardDataTable;

	UPROPERTY()
	TObjectPtr<UAbilityManagerComponent> AbilityManager;

	// Progressive chance for "rare card"
	float CurrentRareChance = 0.5f;
	const float RareChanceIncrement = 0.1f;
	const float RareChanceDecrement = 5.f;
	const float MinRareChance = 0.5f;
	const float MaxRareChance = 30.f;

private:
	FGeneratedCard GenerateSingleCard(int32 PlayerLevel, const TSet<FName>& UsedAbilityTypes);
	FGeneratedCard ScaleCardTemplate(const FAbilityCard* Template, FName CardID, int32 PlayerLevel, bool bIsRare);
	TArray<FAbilityCard*> GetAvailableCardTemplates(const TSet<FName>& UsedAbilityTypes) const;
	bool IsCardAvailable(const FAbilityCard* Card) const;
};