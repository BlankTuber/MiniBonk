#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossSpawnerComponent.generated.h"

class ABossCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UBossSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossSpawnerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	TSubclassOf<ABossCharacter> BossClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Kills")
	int32 BaseKillsRequired = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Kills")
	float KillReductionPerMinute = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Kills")
	int32 MinKillsRequired = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Location")
	float MinSpawnRadius = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Location")
	float MaxSpawnRadius = 1500.f;

private:
	int32 KillsSinceLastBoss = 0;
	int32 BossesSpawned = 0;

	UFUNCTION()
	void OnEnemyKilled();

	void SpawnBoss();
	int32 GetCurrentKillsRequired() const;
	FVector GetSpawnLocation() const;
};