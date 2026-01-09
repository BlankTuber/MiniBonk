#include "Components/LevelComponent.h"
#include "Components/CoinComponent.h"
#include "Kismet/GameplayStatics.h"

ULevelComponent::ULevelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULevelComponent::BeginPlay()
{
	Super::BeginPlay();

	UCoinComponent* CoinComp = GetOwner()->FindComponentByClass<UCoinComponent>();
	if (ensure(CoinComp))
	{
		CoinComp->OnCoinsChanged.AddDynamic(this, &ULevelComponent::OnCoinsChanged);
	}
}

void ULevelComponent::OnCoinsChanged(int32 TotalCoins, int32 Delta)
{
	float ElapsedSeconds = GetWorld()->GetTimeSeconds();
	float XPMultiplier = FMath::Pow(1.f + XPGrowthPerSecond, ElapsedSeconds);

	int32 XPGained = FMath::RoundToInt(Delta * XPPerCoin * XPMultiplier);

	UE_LOG(LogTemp, Verbose, TEXT("LevelComponent: Coins %d -> XP %d (%.2fx multiplier at %.0fs)"),
		Delta, XPGained, XPMultiplier, ElapsedSeconds);

	AddXP(XPGained);
}

void ULevelComponent::AddXP(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentXP += Amount;
	OnXPChanged.Broadcast(CurrentXP, GetXPForNextLevel(), CurrentLevel);

	CheckLevelUp();
}

void ULevelComponent::CheckLevelUp()
{
	int32 XPRequired = GetXPForNextLevel();

	while (CurrentXP >= XPRequired)
	{
		CurrentXP -= XPRequired;
		CurrentLevel++;

		UE_LOG(LogTemp, Log, TEXT("LevelComponent: Level Up! Now level %d"), CurrentLevel);

		// Pause game for card selection
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		OnLevelUp.Broadcast(CurrentLevel, GetXPForNextLevel());

		XPRequired = GetXPForNextLevel();
	}

	OnXPChanged.Broadcast(CurrentXP, GetXPForNextLevel(), CurrentLevel);
}

int32 ULevelComponent::CalculateXPForLevel(int32 Level) const
{
	float XP = BaseXPRequirement * FMath::Pow(GrowthRate, static_cast<float>(Level - 1));
	return FMath::RoundToInt(XP);
}

int32 ULevelComponent::GetXPForNextLevel() const
{
	return CalculateXPForLevel(CurrentLevel);
}

float ULevelComponent::GetLevelProgressPercent() const
{
	int32 XPRequired = GetXPForNextLevel();
	if (XPRequired <= 0)
	{
		return 0.f;
	}

	return static_cast<float>(CurrentXP) / static_cast<float>(XPRequired);
}