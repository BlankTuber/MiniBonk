#include "Systems/AbilityManagerComponent.h"

UAbilityManagerComponent::UAbilityManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAbilityManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAbilityManagerComponent::RegisterUnlock(FName UnlockID)
{
	if (!ensure(!UnlockID.IsNone()))
	{
		return;
	}

	Unlocks.Add(UnlockID, false);
}

void UAbilityManagerComponent::RegisterLimit(FName AbilityID)
{
	if (!ensure(!AbilityID.IsNone()))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("AbilityManager: Registered limit tracking for %s"), *AbilityID.ToString());
}

void UAbilityManagerComponent::NotifyLimitReached(FName AbilityID)
{
	if (!ensure(!AbilityID.IsNone()))
	{
		return;
	}

	LimitedAbilities.Add(AbilityID);
	UE_LOG(LogTemp, Warning, TEXT("AbilityManager: %s has reached its limit"), *AbilityID.ToString());
}

void UAbilityManagerComponent::CollectCard(const FGeneratedCard& Card)
{
	if (Card.CardType == ECardType::Unlock)
	{
		if (bool* IsAcquired = Unlocks.Find(Card.AffectsAbility))
		{
			*IsAcquired = true;
			OnUnlockAcquired.Broadcast(Card.AffectsAbility);
			UE_LOG(LogTemp, Log, TEXT("AbilityManager: Unlock acquired - %s"), *Card.CardName.ToString());
		}
	}

	if (Card.AbilityType == EAbilityType::Passive || Card.AbilityType == EAbilityType::Auto)
	{
		ApplyPassiveCard(Card);
	}
	else if (Card.AbilityType == EAbilityType::Active)
	{
		StoreActiveCard(Card);
	}

	if (Card.bIsCurse)
	{
		ProcessCurseEffect(Card);
	}

	if (Card.bLocksUnlock)
	{
		ProcessUnlockLock(Card.LockedUnlockID);
	}
}

void UAbilityManagerComponent::ApplyPassiveCard(const FGeneratedCard& Card)
{
	OnPassiveCardApplied.Broadcast(Card.AffectsAbility, Card.ModifierType, Card.Value);
	UE_LOG(LogTemp, Log, TEXT("AbilityManager: Passive card applied - %s: %f (%s)"), *Card.AffectsAbility.ToString(), Card.Value, Card.ModifierType == EModifierType::Flat ? TEXT("Flat") : TEXT("Percentage"));
}

void UAbilityManagerComponent::StoreActiveCard(const FGeneratedCard& Card)
{
	FCollectedCard CollectedCard;
	CollectedCard.AbilityID = Card.AffectsAbility;
	CollectedCard.ModifierType = Card.ModifierType;
	CollectedCard.Value = Card.Value;

	FAbilityCardCollection& Collection = ActiveAbilityCards.FindOrAdd(Card.AffectsAbility);
	Collection.Cards.Add(CollectedCard);

	OnActiveCardCollected.Broadcast(Card.AffectsAbility, Card.ModifierType, Card.Value);
	UE_LOG(LogTemp, Log, TEXT("AbilityManager: Active card stored - %s: %f"), *Card.AffectsAbility.ToString(), Card.Value);
}

void UAbilityManagerComponent::ProcessCurseEffect(const FGeneratedCard& Card)
{
	FGeneratedCard CurseCard;
	CurseCard.AffectsAbility = Card.CurseAffectsAbility;
	CurseCard.AbilityType = Card.CurseAbilityType;
	CurseCard.ModifierType = Card.CurseModifierType;
	CurseCard.Value = Card.CurseValue;

	if (CurseCard.AbilityType == EAbilityType::Passive || CurseCard.AbilityType == EAbilityType::Auto)
	{
		ApplyPassiveCard(CurseCard);
	}
	else if (CurseCard.AbilityType == EAbilityType::Active)
	{
		StoreActiveCard(CurseCard);
	}

	UE_LOG(LogTemp, Warning, TEXT("AbilityManager: Curse effect applied to %s"), *Card.CurseAffectsAbility.ToString());
}

void UAbilityManagerComponent::ProcessUnlockLock(FName LockedUnlockID)
{
	LockedUnlocks.Add(LockedUnlockID);
	UE_LOG(LogTemp, Warning, TEXT("AbilityManager: Unlock permanently locked - %s"), *LockedUnlockID.ToString());
}

float UAbilityManagerComponent::CalculateActiveAbilityValue(FName AbilityID, float BaseValue) const
{
	const FAbilityCardCollection* Collection = ActiveAbilityCards.Find(AbilityID);
	if (!Collection)
	{
		return BaseValue;
	}

	// (Base + AllFlats) * (1 + Sum(Percentages))
	float FlatBonus = 0.f;
	float PercentageBonus = 0.f;

	for (const FCollectedCard& Card : Collection->Cards)
	{
		if (Card.ModifierType == EModifierType::Flat)
		{
			FlatBonus += Card.Value;
		}
		else if (Card.ModifierType == EModifierType::Percentage)
		{
			PercentageBonus += Card.Value;
		}
	}

	float FinalValue = (BaseValue + FlatBonus) * (1.f + PercentageBonus);
	return FinalValue;
}

bool UAbilityManagerComponent::IsUnlockAvailable(FName UnlockID) const
{
	return Unlocks.Contains(UnlockID) && !IsUnlockLocked(UnlockID);
}

bool UAbilityManagerComponent::IsUnlockAcquired(FName UnlockID) const
{
	const bool* IsAcquired = Unlocks.Find(UnlockID);
	return IsAcquired && *IsAcquired;
}

bool UAbilityManagerComponent::IsAbilityLimited(FName AbilityID) const
{
	return LimitedAbilities.Contains(AbilityID);
}

bool UAbilityManagerComponent::IsUnlockLocked(FName UnlockID) const
{
	return LockedUnlocks.Contains(UnlockID);
}