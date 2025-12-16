#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/AbilityTypes.h"
#include "MovementStatsComponent.generated.h"

class UCharacterMovementComponent;
class UAbilityManagerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIBONK_API UMovementStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMovementStatsComponent();

protected:
	virtual void BeginPlay() override;

	// Base values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Stats")
	float BaseMoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Stats")
	float BaseJumpHeight = 700.f;

	// Current stat values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Stats")
	float CurrentMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Stats")
	float CurrentJumpHeight;


private:
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY()
	TObjectPtr<UAbilityManagerComponent> AbilityManager;

	// Maximum allowed values
	UPROPERTY(EditAnywhere, Category = "Movement Stats|Limits")
	float MaxMoveSpeed = 1100.f;

	UPROPERTY(EditAnywhere, Category = "Movement Stats|Limits")
	float MaxJumpHeight = 1500.f;

	void ApplyStatsToMovementComponent();

public:
	UFUNCTION()
	void OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value);
};
