#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MinibonkPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UMinibonkHUD;

// Handles all player input and camera control
UCLASS()
class MINIBONK_API AMinibonkPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Enhanced Input assets
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction;

	// HUD widget class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMinibonkHUD> HUDWidgetClass;

	// Active HUD widget instance
	UPROPERTY()
	TObjectPtr<UMinibonkHUD> HUDWidget;

private:
	// Input callbacks
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void Dash();

	void CreateHUD();

	// Cached movement input for dash direction
	FVector2D LastMovementInput;
};