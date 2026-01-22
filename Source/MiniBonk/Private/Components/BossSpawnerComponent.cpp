#include "Components/BossSpawnerComponent.h"
#include "Characters/BossCharacter.h"
#include "Systems/KillTrackingSubsystem.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

UBossSpawnerComponent::UBossSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBossSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UKillTrackingSubsystem* KillTracker = GI->GetSubsystem<UKillTrackingSubsystem>())
		{
			KillTracker->OnEnemyKilled.AddDynamic(this, &UBossSpawnerComponent::OnEnemyKilled);
		}
	}

	if (!ensure(BossClass))
	{
		UE_LOG(LogTemp, Error, TEXT("BossSpawnerComponent: No BossClass set!"));
	}
}

void UBossSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UKillTrackingSubsystem* KillTracker = GI->GetSubsystem<UKillTrackingSubsystem>())
		{
			KillTracker->OnEnemyKilled.RemoveDynamic(this, &UBossSpawnerComponent::OnEnemyKilled);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UBossSpawnerComponent::OnEnemyKilled()
{
	KillsSinceLastBoss++;

	int32 KillsRequired = GetCurrentKillsRequired();

	if (KillsSinceLastBoss >= KillsRequired)
	{
		SpawnBoss();
		KillsSinceLastBoss = 0;
		BossesSpawned++;

		UE_LOG(LogTemp, Log, TEXT("BossSpawnerComponent: Boss spawned! Next boss requires %d kills"), GetCurrentKillsRequired());
	}
}

void UBossSpawnerComponent::SpawnBoss()
{
	if (!BossClass)
	{
		return;
	}

	FVector SpawnLocation = GetSpawnLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABossCharacter* SpawnedBoss = GetWorld()->SpawnActor<ABossCharacter>(
		BossClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpawnedBoss)
	{
		UE_LOG(LogTemp, Log, TEXT("BossSpawnerComponent: Spawned boss at %s"), *SpawnLocation.ToString());
	}
}

int32 UBossSpawnerComponent::GetCurrentKillsRequired() const
{
	float ElapsedMinutes = GetWorld()->GetTimeSeconds() / 60.f;
	float ReductionMultiplier = 1.f - (ElapsedMinutes * KillReductionPerMinute);
	ReductionMultiplier = FMath::Max(ReductionMultiplier, static_cast<float>(MinKillsRequired) / static_cast<float>(BaseKillsRequired));

	return FMath::Max(MinKillsRequired, FMath::RoundToInt(BaseKillsRequired * ReductionMultiplier));
}

FVector UBossSpawnerComponent::GetSpawnLocation() const
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossSpawnerComponent: No navigation system found"));
		return GetOwner()->GetActorLocation();
	}

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector Origin = Player ? Player->GetActorLocation() : GetOwner()->GetActorLocation();

	for (int32 Attempt = 0; Attempt < 10; ++Attempt)
	{
		float RandomAngle = FMath::RandRange(0.f, 360.f);
		float RandomRadius = FMath::RandRange(MinSpawnRadius, MaxSpawnRadius);

		FVector Offset = FVector(
			FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
			FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
			0.f
		);

		FVector TestLocation = Origin + Offset;

		FNavLocation NavLocation;
		if (NavSystem->ProjectPointToNavigation(TestLocation, NavLocation, FVector(500.f, 500.f, 500.f)))
		{
			return NavLocation.Location + FVector(0.f, 0.f, 100.f);
		}
	}

	FNavLocation RandomNavLocation;
	if (NavSystem->GetRandomPoint(RandomNavLocation))
	{
		return RandomNavLocation.Location + FVector(0.f, 0.f, 100.f);
	}

	return Origin;
}