#include "Characters/MinibonkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "UI/MinibonkHUD.h"
#include "Characters/PlayerCharacter.h"
#include "Systems/AbilityManagerComponent.h"
#include "Components/LevelComponent.h"

void AMinibonkPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set initial camera angle
	SetControlRotation(FRotator(-25.f, 0.f, 0.f));

	// Add Enhanced Input mapping context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ensure(DefaultMappingContext))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Create and initialize the HUD
	CreateHUD();
}

void AMinibonkPlayerController::CreateHUD()
{
	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MinibonkPlayerController: No HUDWidgetClass set"));
		return;
	}

	HUDWidget = CreateWidget<UMinibonkHUD>(this, HUDWidgetClass);
	if (!HUDWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("MinibonkPlayerController: Failed to create HUD widget"));
		return;
	}

	HUDWidget->AddToViewport();

	// Get the player character and connect systems
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn());
	if (!PlayerChar)
	{
		return;
	}

	// Initialize HUD with ability manager
	if (PlayerChar->AbilityManagerComponent)
	{
		HUDWidget->InitializeHUD(PlayerChar->AbilityManagerComponent);
	}

	// Bind to health changes
	if (PlayerChar->HealthComponent)
	{
		PlayerChar->HealthComponent->OnHealthChanged.AddDynamic(HUDWidget, &UMinibonkHUD::UpdateHealth);

		// Set initial health display
		HUDWidget->UpdateHealth(
			PlayerChar->HealthComponent->MaxHealth,
			PlayerChar->HealthComponent->MaxHealth
		);
	}

	// Bind to coin changes
	if (PlayerChar->CoinComponent)
	{
		PlayerChar->CoinComponent->OnCoinsChanged.AddDynamic(HUDWidget, &UMinibonkHUD::UpdateCoins);

		// Set initial coin display
		HUDWidget->UpdateCoins(PlayerChar->CoinComponent->GetCurrentCoins(), 0);
	}

	// Bind to level/XP changes
	if (PlayerChar->LevelComponent)
	{
		PlayerChar->LevelComponent->OnLevelUp.AddDynamic(HUDWidget, &UMinibonkHUD::OnLevelUp);
		PlayerChar->LevelComponent->OnXPChanged.AddDynamic(HUDWidget, &UMinibonkHUD::UpdateXP);

		// Set initial XP display
		HUDWidget->UpdateXP(
			PlayerChar->LevelComponent->GetCurrentXP(),
			PlayerChar->LevelComponent->GetXPForNextLevel(),
			PlayerChar->LevelComponent->GetCurrentLevel()
		);
	}

	UE_LOG(LogTemp, Log, TEXT("MinibonkPlayerController: HUD connected to player components"));
}

void AMinibonkPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensure(EnhancedInput))
	{
		return;
	}

	// Bind input actions
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

	// Calculate camera-relative movement directions
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

	// Pitch limits
	const float MinPitch = -75.f;
	const float MaxPitch = 75.f;
	const float ComfortZoneMin = -40.f;
	const float ComfortZoneMax = 40.f;

	// Slower pitch input in comfort zone for better control
	float PitchInputScale = 1.f;

	if (CurrentPitch > ComfortZoneMin && CurrentPitch < ComfortZoneMax)
	{
		PitchInputScale = 0.35f;
	}
	else
	{
		// Scale up input as we approach limits
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
	AddPitchInput(-LookAxisVector.Y * PitchInputScale);

	// Hard clamp pitch to prevent camera flip
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