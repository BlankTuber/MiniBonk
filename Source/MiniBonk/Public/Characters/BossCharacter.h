#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacter.h"
#include "BossCharacter.generated.h"

class ABossGroundAttack;

UCLASS()
class MINIBONK_API ABossCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ABossCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Stats")
	float HPMultiplier = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Loot")
	float CoinDropMultiplier = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	TSubclassOf<ABossGroundAttack> GroundAttackClass;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float AttackCooldown = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float AttackRange = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float BaseAttackDamage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Scaling")
	float AttackDamageGrowthPerMinute = 0.2f;

private:
	float LastAttackTime = -100.f;
	float CurrentAttackDamage = 0.f;

	void TryPerformGroundAttack();
	bool IsPlayerInAttackRange() const;
	class ACharacter* GetPlayerCharacter() const;
};