#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/AbilityTypes.h"
#include "DashComponent.generated.h"

class UAbilityManagerComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDashComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Dash")
	bool TryDash(const FVector& Direction);

	UFUNCTION(BlueprintPure, Category = "Dash")
	bool CanDash() const;

	UFUNCTION(BlueprintPure, Category = "Dash")
	float GetCooldownRemaining() const;

	UFUNCTION(BlueprintPure, Category = "Dash")
	float GetCooldownPercent() const;

	UFUNCTION(BlueprintPure, Category = "Dash")
	bool IsDashing() const { return bIsDashing; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Ability IDs")
	FName CooldownAbilityID = "DashCooldown";

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Ability IDs")
	FName DistanceAbilityID = "DashDistance";

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Ability IDs")
	FName DamageAbilityID = "DashDamage";

	// Base stats
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Stats")
	float BaseCooldown = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Stats")
	float BaseDistance = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Stats")
	float DashDuration = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Stats")
	float BaseDamage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Stats")
	float DamageRadius = 250.f;

	// Limits
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Limits")
	float MinCooldown = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Limits")
	float MaxDistance = 1500.f;

private:
	UPROPERTY()
	TObjectPtr<UAbilityManagerComponent> AbilityManager;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	float LastDashTime = -100.f;
	bool bIsDashing = false;
	float DashEndTime = 0.f;
	float CurrentDashDamage = 0.f;

	// Track enemies already hit during this dash to avoid double damage
	UPROPERTY()
	TSet<AActor*> HitEnemiesThisDash;

	// Calculate current stats from base + cards
	float GetCurrentCooldown() const;
	float GetCurrentDistance() const;
	float GetCurrentDamage() const;

	void PerformDash(const FVector& Direction);
	void EndDash();
	void CheckForEnemies();
};