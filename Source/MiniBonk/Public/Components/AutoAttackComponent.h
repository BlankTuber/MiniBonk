#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/AbilityTypes.h"
#include "AutoAttackComponent.generated.h"

class UAbilityManagerComponent;

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UAutoAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAutoAttackComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Ability IDs")
	FName DamageAbilityID;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Stats")
	float BaseDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Stats")
	float BaseCooldown = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Stats")
	float BaseRange = 1500.f;

	UPROPERTY(VisibleAnywhere, Category = "Attack|Current Stats")
	float CurrentDamage;

	UPROPERTY(VisibleAnywhere, Category = "Attack|Current Stats")
	float CurrentCooldown;

	UPROPERTY(VisibleAnywhere, Category = "Attack|Current Stats")
	float CurrentRange;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Targeting")
	int32 TargetCount = 1;

	UPROPERTY()
	TObjectPtr<UAbilityManagerComponent> AbilityManager;

	TArray<AActor*> FindTargets() const;

	virtual void PerformAttack(const TArray<AActor*>& Targets) PURE_VIRTUAL(UAutoAttackComponent::PerformAttack, );

	UFUNCTION()
	virtual void OnPassiveCardApplied(FName CardAbilityID, EModifierType ModifierType, float Value);

	void RestartAttackTimer();

private:
	FTimerHandle AttackTimerHandle;

	void StartAttackTimer();
	void OnAttackTimerExpired();
};