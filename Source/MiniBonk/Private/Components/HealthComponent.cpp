#include "Components/HealthComponent.h"
#include "Systems/AbilityTypes.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
	bIsDead = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize health
	CurrentHealth = MaxHealth;
	bIsDead = false;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
	if (bIsDead)
	{
		return;
	}

	if (DamageAmount <= 0.f)
	{
		return;
	}

	CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.f);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	// Check for death
	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	if (bIsDead)
	{
		return;
	}

	if (HealAmount <= 0.f)
	{
		return;
	}

	CurrentHealth = FMath::Min(CurrentHealth + HealAmount, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

bool UHealthComponent::isDead() const
{
	return bIsDead;
}

float UHealthComponent::GetHealthPrecent() const
{
	if (MaxHealth <= 0.f)
	{
		return 0.f;
	}

	return CurrentHealth / MaxHealth;
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth, bool bHealToFull)
{
	if (NewMaxHealth <= 0.f)
	{
		return;
	}

	float OldMaxHealth = MaxHealth;
	MaxHealth = NewMaxHealth;

	if (bHealToFull)
	{
		CurrentHealth = MaxHealth;
	}
	else
	{
		// Maintain health percentage
		float HealthPrecent = (OldMaxHealth > 0.f) ? (CurrentHealth / OldMaxHealth) : 1.f;
		CurrentHealth = MaxHealth * HealthPrecent;
	}

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value)
{
	if (AbilityID != "MaxHealth")
	{
		return;
	}

	float NewMaxHealth = MaxHealth;

	if (ModifierType == EModifierType::Flat)
	{
		NewMaxHealth += Value;
	}
	else if (ModifierType == EModifierType::Percentage)
	{
		NewMaxHealth *= (1.f + Value);
	}
	
	SetMaxHealth(NewMaxHealth, true);

	UE_LOG(LogTemp, Log, TEXT("HealthComponent: MaxHealth upgraded to %f (%s)"), NewMaxHealth, ModifierType == EModifierType::Flat ? TEXT("Flat") : TEXT("Percentage"));
}