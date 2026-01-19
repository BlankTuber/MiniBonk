#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHealthComponent;
class UMovementStatsComponent;
class UAbilityManagerComponent;
class UStoneThrowComponent;
class UCoinComponent;
class ULevelComponent;
class UDashComponent;

UCLASS()
class MINIBONK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UMovementStatsComponent> MovementStatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilityManagerComponent> AbilityManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UStoneThrowComponent> StoneThrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
	TObjectPtr<UCoinComponent> CoinComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progression")
	TObjectPtr<ULevelComponent> LevelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UDashComponent> DashComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TWeakObjectPtr<AActor> CurrentInteractable;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleDeath();
};