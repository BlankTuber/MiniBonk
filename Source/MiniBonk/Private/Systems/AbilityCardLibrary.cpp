#include "Systems/AbilityCardLibrary.h"
#include "Systems/AbilityManagerComponent.h"
#include "Engine/DataTable.h"

void UAbilityCardLibrary::Initialize(UDataTable* InCardDataTable, UAbilityManagerComponent* InAbilityManager)
{
	CardDataTable = InCardDataTable;
	AbilityManager = InAbilityManager;

	if (!ensure(CardDataTable))
	{
		UE_LOG(LogTemp, Error, TEXT("AbilityCardLibrary: CardDataTable is null"));
	}

	if (!ensure(AbilityManager))
	{
		UE_LOG(LogTemp, Error, TEXT("AbilityCardLibrary: AbilityManager is null"));
	}
}

TArray<FGeneratedCard> UAbilityCardLibrary::GenerateCardChoices(int32 PlayerLevel)
{
	if (!ensure(CardDataTable && AbilityManager))
	{
		return TArray<FGeneratedCard>();
	}

	TArray<FGeneratedCard> GeneratedCards;
	TSet<FName> UsedAbilityTypes;

	for (int32 i = 0; i < 3; ++i)
	{
		FGeneratedCard Card = GenerateSingleCard(PlayerLevel, UsedAbilityTypes);

		if (!Card.CardID.IsNone())
		{
			GeneratedCards.Add(Card);
			UsedAbilityTypes.Add(Card.AffectsAbility);
		}
	}

	return GeneratedCards;
}

FGeneratedCard UAbilityCardLibrary::GenerateSingleCard(int32 PlayerLevel, const TSet<FName>& UsedAbilityTypes)
{
	TArray<FAbilityCard*> AvailableCards = GetAvailableCardTemplates(UsedAbilityTypes);

	if (AvailableCards.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityCardLibrary: No available cards to generate"));
		return FGeneratedCard();
	}

	// Roll for rare
	bool bIsRare = FMath::FRand() * 100.f < CurrentRareChance;

	if (bIsRare)
	{
		// 50/50: Boosted buff / Curse
		bool bIsCurse = FMath::RandBool();

		TArray<FAbilityCard*> FilteredCards;
		for (FAbilityCard* Card : AvailableCards)
		{
			if (bIsCurse && Card->bIsCurse)
			{
				FilteredCards.Add(Card);
			}
			else if (!bIsCurse && !Card->bIsCurse)
			{
				FilteredCards.Add(Card);
			}
		}

		if (FilteredCards.Num() > 0)
		{
			AvailableCards = FilteredCards;
		}

		CurrentRareChance = FMath::Max(MinRareChance, CurrentRareChance - RareChanceDecrement);
	}
	else
	{
		// Only select non-curse cards for common
		TArray<FAbilityCard*> NonCurseCards;
		for (FAbilityCard* Card : AvailableCards)
		{
			if (!Card->bIsCurse)
			{
				NonCurseCards.Add(Card);
			}
		}

		if (NonCurseCards.Num() > 0)
		{
			AvailableCards = NonCurseCards;
		}

		CurrentRareChance = FMath::Min(MaxRareChance, CurrentRareChance + RareChanceIncrement);
	}

	// Select random card
	int32 RandomIndex = FMath::RandRange(0, AvailableCards.Num() - 1);
	FAbilityCard* SelectedTemplate = AvailableCards[RandomIndex];

	// Get card ID from data table
	FName CardID = NAME_None;
	TArray<FName> RowNames = CardDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FAbilityCard* Row = CardDataTable->FindRow<FAbilityCard>(RowName, TEXT(""));
		if (Row == SelectedTemplate)
		{
			CardID = RowName;
			break;
		}
	}

	return ScaleCardTemplate(SelectedTemplate, CardID, PlayerLevel, bIsRare);
}

FGeneratedCard UAbilityCardLibrary::ScaleCardTemplate(const FAbilityCard* Template, FName CardID, int32 PlayerLevel, bool bIsRare)
{
	FGeneratedCard Generated;
	Generated.CardID = CardID;
	Generated.CardName = Template->CardName;
	Generated.Description = Template->Description;
	Generated.CardType = Template->CardType;
	Generated.AffectsAbility = Template->AffectsAbility;
	Generated.AbilityType = Template->AbilityType;
	Generated.ModifierType = Template->ModifierType;

	// Scale value
	float BaseScaleValue = Template->BaseValue + (Template->ScalingPerLevel * PlayerLevel);
	float VarianceRange = 0.2f;
	float VarianceMultiplier = FMath::FRandRange(1.f - VarianceRange, 1.f + VarianceRange);

	if (Template->ModifierType == EModifierType::Flat)
	{
		Generated.Value = FMath::RoundToFloat(BaseScaleValue * VarianceMultiplier);
	}
	else
	{
		Generated.Value = FMath::RoundToFloat(BaseScaleValue * VarianceMultiplier * 100.f) / 100.f;
	}


	// Apply rare boost for non-curse cards
	if (bIsRare && !Template->bIsCurse)
	{
		float BaseBoost = FMath::RandRange(2.f, 3.f);
		float BoostMultiplier = BaseBoost * Template->RareBoostMultiplier;
		Generated.Value *= BoostMultiplier;

		UE_LOG(LogTemp, Log, TEXT("AbilityCardLibrary: RARE card! %s boosted by %.1fx"),
			*Template->CardName.ToString(), BoostMultiplier);
	}

	// Copy curse properties
	Generated.bIsCurse = Template->bIsCurse;
	Generated.CurseAffectsAbility = Template->CurseAffectsAbility;
	Generated.CurseAbilityType = Template->CurseAbilityType;
	Generated.CurseModifierType = Template->CurseModifierType;
	Generated.CurseValue = Template->CurseBaseValue + (Template->CurseScalingPerLevel * PlayerLevel);
	Generated.bLocksUnlock = Template->bLocksUnlock;
	Generated.LockedUnlockID = Template->LockedUnlockID;

	return Generated;
}

TArray<FAbilityCard*> UAbilityCardLibrary::GetAvailableCardTemplates(const TSet<FName>& UsedAbilityTypes) const
{
	TArray<FAbilityCard*> Available;
	TArray<FName> RowNames = CardDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FAbilityCard* Card = CardDataTable->FindRow<FAbilityCard>(RowName, TEXT(""));
		if (Card && IsCardAvailable(Card) && !UsedAbilityTypes.Contains(Card->AffectsAbility))
		{
			Available.Add(Card);
		}
	}

	return Available;
}

bool UAbilityCardLibrary::IsCardAvailable(const FAbilityCard* Card) const
{
	// Check if unlock type and already acquired
	if (Card->CardType == ECardType::Unlock)
	{
		if (AbilityManager->IsUnlockAcquired(Card->AffectsAbility))
		{
			return false;
		}

		if (AbilityManager->IsUnlockLocked(Card->AffectsAbility))
		{
			return false;
		}
	}

	// Check if this card requires an unlock first
	if (!Card->RequiredUnlock.IsNone())
	{
		if (!AbilityManager->IsUnlockAcquired(Card->RequiredUnlock))
		{
			return false;
		}
	}

	// Check if ability is limited
	if (AbilityManager->IsAbilityLimited(Card->AffectsAbility))
	{
		return false;
	}

	return true;
}