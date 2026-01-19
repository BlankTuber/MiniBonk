#include "Components/CoinComponent.h"
#include "Systems/AbilityMath.h"

UCoinComponent::UCoinComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MagnetRadius = BaseMagnetRadius;
}

void UCoinComponent::BeginPlay()
{
	Super::BeginPlay();

	MagnetRadius = BaseMagnetRadius;
}

void UCoinComponent::AddCoins(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentCoins += Amount;
	OnCoinsChanged.Broadcast(CurrentCoins, Amount);

	UE_LOG(LogTemp, Log, TEXT("CoinComponent: Collected %d coins. Total: %d"), Amount, CurrentCoins);
}

bool UCoinComponent::SpendCoins(int32 Amount)
{
	if (Amount <= 0 || Amount > CurrentCoins)
	{
		return false;
	}

	CurrentCoins -= Amount;
	OnCoinsChanged.Broadcast(CurrentCoins, -Amount);

	UE_LOG(LogTemp, Log, TEXT("CoinComponent: Spent %d coins. Remaining: %d"), Amount, CurrentCoins);
	return true;
}

void UCoinComponent::SetMagnetRadius(float NewRadius)
{
	MagnetRadius = FMath::Clamp(NewRadius, 0.f, MaxMagnetRadius);
	UE_LOG(LogTemp, Log, TEXT("CoinComponent: Magnet radius set to %.0f"), MagnetRadius);
}

void UCoinComponent::OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value)
{
	if (AbilityID != "MagnetRadius")
	{
		return;
	}

	if (MagnetRadius >= MaxMagnetRadius)
	{
		UE_LOG(LogTemp, Warning, TEXT("CoinComponent: MagnetRadius already at max (%.0f)"), MaxMagnetRadius);
		return;
	}

	float NewRadius = AbilityMath::ApplyModifier(MagnetRadius, ModifierType, Value, MaxMagnetRadius);
	SetMagnetRadius(NewRadius);

	UE_LOG(LogTemp, Log, TEXT("CoinComponent: MagnetRadius upgraded to %.0f [Max: %.0f]"), MagnetRadius, MaxMagnetRadius);
}