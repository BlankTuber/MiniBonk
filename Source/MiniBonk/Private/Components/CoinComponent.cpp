#include "Components/CoinComponent.h"

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

void UCoinComponent::SetMagnetRadius(float NewRadius)
{
	MagnetRadius = FMath::Max(0.f, NewRadius);
	UE_LOG(LogTemp, Log, TEXT("CoinComponent: Magnet radius set to %.0f"), MagnetRadius);
}