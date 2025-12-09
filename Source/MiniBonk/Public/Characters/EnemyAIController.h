#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class MINIBONK_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PathUpdateInterval = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ContactDistance = 25.f;

	float TimeSinceLastPathUpdate = 0.f;

	void UpdatePathToPlayer();
};
