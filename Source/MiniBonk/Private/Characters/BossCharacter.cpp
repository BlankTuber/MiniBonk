#include "Characters/BossCharacter.h"
#include "Components/HealthComponent.h"
#include "Actors/BossGroundAttack.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		float BoostedHP = HealthComponent->MaxHealth * HPMultiplier;
		HealthComponent->SetMaxHealth(BoostedHP, true);
		UE_LOG(LogTemp, Log, TEXT("BossCharacter: HP set to %.0f (%.1fx multiplier)"), BoostedHP, HPMultiplier);
	}

	float ElapsedMinutes = GetWorld()->GetTimeSeconds() / 60.f;
	float DamageMultiplier = 1.f + (ElapsedMinutes * AttackDamageGrowthPerMinute);
	CurrentAttackDamage = BaseAttackDamage * DamageMultiplier;

	UE_LOG(LogTemp, Log, TEXT("BossCharacter: Attack damage %.1f (%.2fx at %.1f min)"),
		CurrentAttackDamage, DamageMultiplier, ElapsedMinutes);

	MinCoinDrop = FMath::RoundToInt(MinCoinDrop * CoinDropMultiplier);
	MaxCoinDrop = FMath::RoundToInt(MaxCoinDrop * CoinDropMultiplier);
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TryPerformGroundAttack();
}

void ABossCharacter::TryPerformGroundAttack()
{
	if (!GroundAttackClass)
	{
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return;
	}

	if (!IsPlayerInAttackRange())
	{
		return;
	}

	ACharacter* Player = GetPlayerCharacter();
	if (!Player)
	{
		return;
	}

	FVector PlayerLocation = Player->GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* GroundAttack = GetWorld()->SpawnActor<AActor>(
		GroundAttackClass,
		PlayerLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (GroundAttack)
	{
		LastAttackTime = CurrentTime;
		UE_LOG(LogTemp, Log, TEXT("BossCharacter: Spawned ground attack at player position"));
	}
}

bool ABossCharacter::IsPlayerInAttackRange() const
{
	ACharacter* Player = GetPlayerCharacter();
	if (!Player)
	{
		return false;
	}

	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	return Distance <= AttackRange;
}

ACharacter* ABossCharacter::GetPlayerCharacter() const
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}