#include "Characters/MinibonkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"

void AMinibonkPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetControlRotation(FRotator(-45.0f, 0.0f, 0.0f));

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ensure(DefaultMappingContext))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMinibonkPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensure(EnhancedInput))
	{
		return;
	}

	if (ensure(MoveAction))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMinibonkPlayerController::Move);
	}

	if (ensure(JumpAction))
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AMinibonkPlayerController::StartJump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMinibonkPlayerController::StopJump);
	}
}

void AMinibonkPlayerController::Move(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
	ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
}

void AMinibonkPlayerController::StartJump()
{
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		ControlledCharacter->Jump();
	}
}

void AMinibonkPlayerController::StopJump()
{
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		ControlledCharacter->StopJumping();
	}
}