#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/AbilityTypes.h"
#include "OrbitalStoneComponent.generated.h"

class UAbilityManagerComponent;
class UAudioComponent;

// Orbiting stones that damage enemies on contact - starts disabled until unlocked
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UOrbitalStoneComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOrbitalStoneComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Orbital")
	bool IsUnlocked() const { return bIsUnlocked; }

	UFUNCTION(BlueprintPure, Category = "Orbital")
	int32 GetStoneCount() const { return CurrentStoneCount; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Unlock")
	FName UnlockID = "OrbitalStones";

	// Ability IDs for upgrades
	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Ability IDs")
	FName DamageAbilityID = "OrbitalDamage";

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Ability IDs")
	FName SpeedAbilityID = "OrbitalSpeed";

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Ability IDs")
	FName RadiusAbilityID = "OrbitalRadius";

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Ability IDs")
	FName CountAbilityID = "OrbitalCount";

	// Base stats
	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Stats")
	float BaseDamage = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Stats")
	float BaseOrbitSpeed = 180.f;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Stats")
	float BaseOrbitRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Stats")
	int32 BaseStoneCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Stats")
	float StoneCollisionRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Stats")
	float DamageCooldown = 0.5f;

	// Limits
	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Limits")
	float MaxOrbitSpeed = 720.f;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Limits")
	float MaxOrbitRadius = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Limits")
	int32 MaxStoneCount = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Visual")
	TSubclassOf<AActor> StoneVisualClass;

	// Audio
	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Audio")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Audio")
	TObjectPtr<USoundBase> OrbitLoopSound;

	UPROPERTY(EditDefaultsOnly, Category = "Orbital|Audio")
	float OrbitLoopVolumeMultiplier = 0.5f;

private:
	UPROPERTY()
	TObjectPtr<UAbilityManagerComponent> AbilityManager;

	bool bIsUnlocked = false;

	float CurrentDamage;
	float CurrentOrbitSpeed;
	float CurrentOrbitRadius;
	int32 CurrentStoneCount;

	float CurrentOrbitAngle = 0.f;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> StoneVisuals;

	UPROPERTY()
	TMap<AActor*, float> EnemyDamageCooldowns;

	UPROPERTY()
	TObjectPtr<UAudioComponent> OrbitLoopAudioComponent;

	UFUNCTION()
	void OnUnlockAcquired(FName AcquiredUnlockID);

	UFUNCTION()
	void OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value);

	void ActivateAbility();
	void UpdateStonePositions(float DeltaTime);
	void CheckForEnemyCollisions();
	void SpawnStoneVisuals();
	void UpdateStoneVisualCount();
	FVector CalculateStonePosition(int32 StoneIndex) const;
};