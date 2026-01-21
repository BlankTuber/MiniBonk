#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MinibonkPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UMinibonkHUD;
class UCoinComponent;

UCLASS()
class MINIBONK_API AMinibonkPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "UI")
	UMinibonkHUD* GetHUDWidget() const { return HUDWidget; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMinibonkHUD> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UMinibonkHUD> HUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> BackgroundMusic;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float MusicVolume = 0.5f;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void Dash();
	void Interact();

	void CreateHUD();

	FVector2D LastMovementInput;
};