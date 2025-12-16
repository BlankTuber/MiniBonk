#include "Components/MovementStatsComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Systems/AbilityManagerComponent.h"

UMovementStatsComponent::UMovementStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentMoveSpeed = BaseMoveSpeed;
	CurrentJumpHeight = BaseJumpHeight;
}

void UMovementStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (ensure(OwnerCharacter))
	{
		MovementComponent = OwnerCharacter->GetCharacterMovement();
		if (ensure(MovementComponent))
		{
			// Set base values to MovementComponent's current values
			BaseMoveSpeed = MovementComponent->MaxWalkSpeed;
			BaseJumpHeight = MovementComponent->JumpZVelocity;

			CurrentMoveSpeed = BaseMoveSpeed;
			CurrentJumpHeight = BaseJumpHeight;

			UE_LOG(LogTemp, Log, TEXT("MovementStatsComponent: Initialized with base speed %f, jump %f"), BaseMoveSpeed, BaseJumpHeight);
		}

		AbilityManager = OwnerCharacter->FindComponentByClass<UAbilityManagerComponent>();
		if (ensure(AbilityManager))
		{
			// Register that these abilities have limits
			AbilityManager->RegisterLimit("MoveSpeed");
			AbilityManager->RegisterLimit("JumpHeight");
		}
	}
}

void UMovementStatsComponent::OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value)
{
	bool bStatsChanged = false;

	if (AbilityID == "MoveSpeed")
	{
		if (CurrentMoveSpeed >= MaxMoveSpeed)
		{
			UE_LOG(LogTemp, Warning, TEXT("MovementStatsComponent: MoveSpeed already at cap (%f)"), MaxMoveSpeed);
			return;
		}

		if (ModifierType == EModifierType::Flat)
		{
			CurrentMoveSpeed += Value;
		}
		else if (ModifierType == EModifierType::Percentage)
		{
			CurrentMoveSpeed *= (1.f + Value);
		}

		CurrentMoveSpeed = FMath::Min(CurrentMoveSpeed, MaxMoveSpeed);
		bStatsChanged = true;

		UE_LOG(LogTemp, Log, TEXT("MovementStatsComponent: MoveSpeed upgraded to %f (%s) [Cap: %f]"), CurrentMoveSpeed, ModifierType == EModifierType::Flat ? TEXT("Flat") : TEXT("Percentage"), MaxMoveSpeed);

		// Notify manager if limit has been reached
		if (CurrentMoveSpeed >= MaxMoveSpeed && AbilityManager)
		{
			AbilityManager->NotifyLimitReached("MoveSpeed");
		}
	}
	else if (AbilityID == "JumpHeight")
	{
		if (CurrentJumpHeight >= MaxJumpHeight)
		{
			UE_LOG(LogTemp, Warning, TEXT("MovementStatsComponent: JumpHeight already at cap (%f)"), MaxJumpHeight);
			return;
		}

		if (ModifierType == EModifierType::Flat)
		{
			CurrentJumpHeight += Value;
		}
		else if (ModifierType == EModifierType::Percentage)
		{
			CurrentJumpHeight *= (1.f + Value);
		}

		CurrentJumpHeight = FMath::Min(CurrentJumpHeight, MaxJumpHeight);
		bStatsChanged = true;

		UE_LOG(LogTemp, Log, TEXT("MovementStatsComponent: JumpHeight upgraded to %f (%s) [Cap: %f]"), CurrentJumpHeight, ModifierType == EModifierType::Flat ? TEXT("Flat") : TEXT("Percentage"), MaxJumpHeight);

		// Notify manager if limit has been reached
		if (CurrentJumpHeight >= MaxJumpHeight && AbilityManager)
		{
			AbilityManager->NotifyLimitReached("JumpHeight");
		}
	}

	if (bStatsChanged)
	{
		ApplyStatsToMovementComponent();
	}
}

void UMovementStatsComponent::ApplyStatsToMovementComponent()
{
	if (!ensure(MovementComponent))
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = CurrentMoveSpeed;
	MovementComponent->JumpZVelocity = CurrentJumpHeight;
}