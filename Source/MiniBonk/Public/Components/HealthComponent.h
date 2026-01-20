#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/AbilityTypes.h"
#include "HealthComponent.generated.h"

// Events for health changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageTakenSignature, float, DamageAmount, FVector, HitLocation);

// Reusable health system for any actor
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDamageTakenSignature OnDamageTaken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> DamageSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> DeathSound;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool isDead() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPrecent() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(float NewMaxHealth, bool bHealToFull = true);

	UFUNCTION()
	void OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value);
};