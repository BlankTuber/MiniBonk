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

// Player's physical body - handles camera setup and movement properties
UCLASS()
class MINIBONK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	// Camera setup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// Health system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UHealthComponent> HealthComponent;

	// Movement Stats System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UMovementStatsComponent> MovementStatsComponent;

	// Ability Manager System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilityManagerComponent> AbilityManagerComponent;

	// Stone Throw Ability
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UStoneThrowComponent> StoneThrowComponent;

	// Coin Collection System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
	TObjectPtr<UCoinComponent> CoinComponent;

protected:
	virtual void BeginPlay() override;


private:
	UFUNCTION()
	void HandleDeath();
};