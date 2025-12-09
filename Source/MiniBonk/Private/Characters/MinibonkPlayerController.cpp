#include "Characters/MinibonkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"

void AMinibonkPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetControlRotation(FRotator(-25.0f, 0.0f, 0.0f));

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

	if (ensure(LookAction))
	{
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMinibonkPlayerController::Look);
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

void AMinibonkPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	FRotator CurrentRotation = GetControlRotation();
	float CurrentPitch = FRotator::NormalizeAxis(CurrentRotation.Pitch);

	const float MinPitch = -75.f;
	const float MaxPitch = 75.f;
	const float ComfortZoneMin = -40.f;
	const float ComfortZoneMax = 40.f;

	float PitchInputScale = 1.f;

	if (CurrentPitch > ComfortZoneMin && CurrentPitch < ComfortZoneMax)
	{
		PitchInputScale = 0.35f;
	}
	else
	{
		float DistanceIntoEdgeZone = 0.f;

		if (CurrentPitch >= ComfortZoneMax)
		{
			DistanceIntoEdgeZone = (CurrentPitch - ComfortZoneMax) / (MaxPitch - ComfortZoneMax);
		}
		else if (CurrentPitch <= ComfortZoneMin)
		{
			DistanceIntoEdgeZone = (ComfortZoneMin - CurrentPitch) / (ComfortZoneMin - MinPitch);
		}

		PitchInputScale = FMath::Lerp(0.3f, 0.8f, DistanceIntoEdgeZone);
	}

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y * PitchInputScale);

	CurrentRotation = GetControlRotation();
	CurrentPitch = FRotator::NormalizeAxis(CurrentRotation.Pitch);
	if (CurrentPitch > MaxPitch || CurrentPitch < MinPitch)
	{
		CurrentRotation.Pitch = FMath::Clamp(CurrentPitch, MinPitch, MaxPitch);
		SetControlRotation(CurrentRotation);
	}
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