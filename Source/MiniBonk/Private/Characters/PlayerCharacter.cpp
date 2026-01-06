#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/HealthComponent.h"
#include "Components/MovementStatsComponent.h"
#include "Components/StoneThrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/AbilityManagerComponent.h"
#include "Components/CoinComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Character doesn't control rotation - controller handles camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Movement setup
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	MovementComp->bOrientRotationToMovement = true;
	MovementComp->RotationRate = FRotator(0.f, 500.f, 0.f);

	// Movement values
	MovementComp->MaxWalkSpeed = 500.f;
	MovementComp->MaxAcceleration = 2048.f;
	MovementComp->BrakingDecelerationWalking = 2048.f;
	MovementComp->GroundFriction = 8.f;
	MovementComp->BrakingFrictionFactor = 2.f;

	// Jump values
	MovementComp->JumpZVelocity = 700.f;
	MovementComp->GravityScale = 1.75f;
	MovementComp->AirControl = 0.2f;

	// Camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);
	CameraBoom->bUsePawnControlRotation = true;

	// Follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Health component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeath.AddDynamic(this, &APlayerCharacter::HandleDeath);

	// Movement Stats Component
	MovementStatsComponent = CreateDefaultSubobject<UMovementStatsComponent>(TEXT("MovementStatsComponent"));

	// Ability Manager Component
	AbilityManagerComponent = CreateDefaultSubobject<UAbilityManagerComponent>(TEXT("AbilityManagerComponent"));

	// Stone Throw Component
	StoneThrowComponent = CreateDefaultSubobject<UStoneThrowComponent>(TEXT("StoneThrowComponent"));

	// Coin Component
	CoinComponent = CreateDefaultSubobject<UCoinComponent>(TEXT("CoinComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilityManagerComponent)
	{
		if (HealthComponent)
		{
			AbilityManagerComponent->OnPassiveCardApplied.AddDynamic(
				HealthComponent, &UHealthComponent::OnPassiveCardApplied);
		}

		if (MovementStatsComponent)
		{
			AbilityManagerComponent->OnPassiveCardApplied.AddDynamic(
				MovementStatsComponent, &UMovementStatsComponent::OnPassiveCardApplied);
		}
	}
}


void APlayerCharacter::HandleDeath()
{
	// TODO: Disable all abilities/attacks (when attack system exists)
	// TODO: Show Game Over UI with stats (when UI system exists)
	// TODO: Stop coin/XP collection (when economy system exists)
	// TODO: Save run statistics (when save system exists)

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}