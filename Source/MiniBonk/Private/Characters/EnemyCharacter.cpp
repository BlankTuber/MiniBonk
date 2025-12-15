#include "Characters/EnemyCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Movement setup
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	MovementComp->MaxWalkSpeed = 450.f;
	MovementComp->bOrientRotationToMovement = true;
	MovementComp->RotationRate = FRotator(0.f, 540.f, 0.f);

	// Health component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleDeath);

	// Damage sphere - triggers when player gets close
	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
	DamageSphere->SetupAttachment(RootComponent);
	DamageSphere->SetSphereRadius(100.f);
	DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap events
	if (ensure(DamageSphere))
	{
		DamageSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnDamageSphereBeginOverlap);
		DamageSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnDamageSphereEndOverlap);
	}

	// Don't block camera
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
}

void AEnemyCharacter::OnDamageSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	OverlappingPlayer = Player;

	// Deal damage immediately
	DealDamageToPlayer();

	// Start repeating damage timer
	GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &AEnemyCharacter::DealDamageToPlayer, DamageCooldown, true);
}

void AEnemyCharacter::OnDamageSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	// Stop damaging when player leaves range
	OverlappingPlayer = nullptr;
	GetWorldTimerManager().ClearTimer(DamageTimerHandle);
}

void AEnemyCharacter::DealDamageToPlayer()
{
	if (!OverlappingPlayer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DamageTimerHandle);
		return;
	}

	UHealthComponent* PlayerHealth = OverlappingPlayer->FindComponentByClass<UHealthComponent>();
	if (!ensure(PlayerHealth))
	{
		return;
	}

	PlayerHealth->TakeDamage(ContactDamage);
}

void AEnemyCharacter::HandleDeath()
{
	// TODO: Spawn coins/XP at death location (when economy system exists)
	// TODO: Grant XP to player (when progression system exists)
	// TODO: Notify spawner of death (when spawner system exists)

	AAIController* AIController = Cast<AAIController>(GetController());
	if (ensure(AIController))
	{
		AIController->StopMovement();
	}
	Destroy();
	Destroy();
}