#include "Characters/EnemyCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Characters/EnemyAIController.h"
#include "Actors/Coin.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

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

	// Scale stats based on elapsed time
	float ElapsedSeconds = GetWorld()->GetTimeSeconds();
	float ElapsedMinutes = ElapsedSeconds / 60.f;

	// HP scales exponentially per second
	float HPMultiplier = FMath::Pow(1.f + HPGrowthPerSecond, ElapsedSeconds);
	if (HealthComponent)
	{
		HealthComponent->SetMaxHealth(HealthComponent->MaxHealth * HPMultiplier, true);
		UE_LOG(LogTemp, Verbose, TEXT("Enemy spawned: HP %.0f (%.2fx at %.0fs)"),
			HealthComponent->MaxHealth, HPMultiplier, ElapsedSeconds);
	}

	// Damage scales linearly per minute
	float DamageMultiplier = 1.f + (ElapsedMinutes * DamageGrowthPerMinute);
	ContactDamage = ContactDamage * DamageMultiplier;
	UE_LOG(LogTemp, Verbose, TEXT("Enemy spawned: Damage %.1f (%.2fx at %.1f min)"),
		ContactDamage, DamageMultiplier, ElapsedMinutes);
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

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastDamageTime >= DamageCooldown)
	{
		DealDamageToPlayer();
	}

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

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastDamageTime < DamageCooldown)
	{
		return;
	}

	UHealthComponent* PlayerHealth = OverlappingPlayer->FindComponentByClass<UHealthComponent>();
	if (!ensure(PlayerHealth))
	{
		return;
	}

	PlayerHealth->TakeDamage(ContactDamage);
	LastDamageTime = CurrentTime;
}

void AEnemyCharacter::HandleDeath()
{
	// Spawn coins
	if (CoinClass)
	{
		int32 CoinCount = FMath::RandRange(MinCoinDrop, MaxCoinDrop);
		FVector SpawnLocation = GetActorLocation();

		for (int32 i = 0; i < CoinCount; ++i)
		{
			// Slight random offset so coins don't stack perfectly
			FVector Offset = FVector(
				FMath::RandRange(-50.f, 50.f),
				FMath::RandRange(-50.f, 50.f),
				50.f
			);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			GetWorld()->SpawnActor<ACoin>(CoinClass, SpawnLocation + Offset, FRotator::ZeroRotator, SpawnParams);
		}
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	if (ensure(AIController))
	{
		AIController->StopMovement();
	}

	Destroy();
}