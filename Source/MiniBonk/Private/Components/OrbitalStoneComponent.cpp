#include "Components/OrbitalStoneComponent.h"
#include "Systems/AbilityManagerComponent.h"
#include "Systems/AbilityMath.h"
#include "Characters/EnemyCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UOrbitalStoneComponent::UOrbitalStoneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	CurrentDamage = BaseDamage;
	CurrentOrbitSpeed = BaseOrbitSpeed;
	CurrentOrbitRadius = BaseOrbitRadius;
	CurrentStoneCount = BaseStoneCount;
}

void UOrbitalStoneComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentDamage = BaseDamage;
	CurrentOrbitSpeed = BaseOrbitSpeed;
	CurrentOrbitRadius = BaseOrbitRadius;
	CurrentStoneCount = BaseStoneCount;

	AbilityManager = GetOwner()->FindComponentByClass<UAbilityManagerComponent>();
	if (ensure(AbilityManager))
	{
		AbilityManager->RegisterUnlock(UnlockID);

		AbilityManager->OnUnlockAcquired.AddDynamic(this, &UOrbitalStoneComponent::OnUnlockAcquired);

		AbilityManager->OnPassiveCardApplied.AddDynamic(this, &UOrbitalStoneComponent::OnPassiveCardApplied);

		AbilityManager->RegisterLimit(DamageAbilityID);
		AbilityManager->RegisterLimit(SpeedAbilityID);
		AbilityManager->RegisterLimit(RadiusAbilityID);
		AbilityManager->RegisterLimit(CountAbilityID);
	}

	UE_LOG(LogTemp, Log, TEXT("OrbitalStoneComponent: Initialized (locked). Unlock ID: %s"), *UnlockID.ToString());
}

void UOrbitalStoneComponent::OnUnlockAcquired(FName AcquiredUnlockID)
{
	if (AcquiredUnlockID != UnlockID)
	{
		return;
	}

	ActivateAbility();
}

void UOrbitalStoneComponent::ActivateAbility()
{
	if (bIsUnlocked)
	{
		return;
	}

	bIsUnlocked = true;
	SetComponentTickEnabled(true);

	SpawnStoneVisuals();

	if (OrbitLoopSound)
	{
		OrbitLoopAudioComponent = UGameplayStatics::SpawnSoundAttached(
			OrbitLoopSound,
			GetOwner()->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			false,
			OrbitLoopVolumeMultiplier,
			1.f,
			0.f,
			nullptr,
			nullptr,
			true
		);
	}

	UE_LOG(LogTemp, Log, TEXT("OrbitalStoneComponent: UNLOCKED! %d stones orbiting at %.0f radius"),
		CurrentStoneCount, CurrentOrbitRadius);
}

void UOrbitalStoneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsUnlocked)
	{
		return;
	}

	UpdateStonePositions(DeltaTime);
	CheckForEnemyCollisions();

	float CurrentTime = GetWorld()->GetTimeSeconds();
	TArray<AActor*> ExpiredCooldowns;
	for (auto& Pair : EnemyDamageCooldowns)
	{
		if (CurrentTime >= Pair.Value)
		{
			ExpiredCooldowns.Add(Pair.Key);
		}
	}
	for (AActor* Actor : ExpiredCooldowns)
	{
		EnemyDamageCooldowns.Remove(Actor);
	}
}

void UOrbitalStoneComponent::UpdateStonePositions(float DeltaTime)
{
	CurrentOrbitAngle += CurrentOrbitSpeed * DeltaTime;
	if (CurrentOrbitAngle >= 360.f)
	{
		CurrentOrbitAngle -= 360.f;
	}

	for (int32 i = 0; i < StoneVisuals.Num(); ++i)
	{
		if (StoneVisuals[i])
		{
			FVector NewPosition = CalculateStonePosition(i);
			StoneVisuals[i]->SetActorLocation(NewPosition);
		}
	}
}

FVector UOrbitalStoneComponent::CalculateStonePosition(int32 StoneIndex) const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	// Evenly distribute stones around the circle
	float AngleOffset = (360.f / CurrentStoneCount) * StoneIndex;
	float StoneAngle = CurrentOrbitAngle + AngleOffset;
	float StoneAngleRad = FMath::DegreesToRadians(StoneAngle);

	FVector OwnerLocation = Owner->GetActorLocation();
	FVector Offset = FVector(
		FMath::Cos(StoneAngleRad) * CurrentOrbitRadius,
		FMath::Sin(StoneAngleRad) * CurrentOrbitRadius,
		50.f
	);

	return OwnerLocation + Offset;
}

void UOrbitalStoneComponent::CheckForEnemyCollisions()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), AllEnemies);

	for (int32 i = 0; i < CurrentStoneCount; ++i)
	{
		FVector StonePosition = CalculateStonePosition(i);

		for (AActor* Enemy : AllEnemies)
		{
			if (!Enemy)
			{
				continue;
			}

			if (float* CooldownEnd = EnemyDamageCooldowns.Find(Enemy))
			{
				if (CurrentTime < *CooldownEnd)
				{
					continue;
				}
			}

			float Distance = FVector::Dist(StonePosition, Enemy->GetActorLocation());
			if (Distance <= StoneCollisionRadius + 50.f)
			{
				AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(Enemy);
				if (EnemyChar && EnemyChar->HealthComponent)
				{
					EnemyChar->HealthComponent->TakeDamage(CurrentDamage);
					EnemyDamageCooldowns.Add(Enemy, CurrentTime + DamageCooldown);

					if (HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitSound, StonePosition);
					}

					UE_LOG(LogTemp, Verbose, TEXT("OrbitalStoneComponent: Hit %s for %.0f damage"),
						*Enemy->GetName(), CurrentDamage);
				}
			}
		}
	}
}

void UOrbitalStoneComponent::SpawnStoneVisuals()
{
	if (!StoneVisualClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("OrbitalStoneComponent: No StoneVisualClass set - stones will be invisible"));
		return;
	}

	for (int32 i = 0; i < CurrentStoneCount; ++i)
	{
		FVector SpawnLocation = CalculateStonePosition(i);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* StoneVisual = GetWorld()->SpawnActor<AActor>(
			StoneVisualClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (StoneVisual)
		{
			StoneVisuals.Add(StoneVisual);
		}
	}
}

void UOrbitalStoneComponent::UpdateStoneVisualCount()
{
	while (StoneVisuals.Num() > CurrentStoneCount)
	{
		AActor* Stone = StoneVisuals.Pop();
		if (Stone)
		{
			Stone->Destroy();
		}
	}

	if (StoneVisualClass)
	{
		while (StoneVisuals.Num() < CurrentStoneCount)
		{
			FVector SpawnLocation = CalculateStonePosition(StoneVisuals.Num());
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* StoneVisual = GetWorld()->SpawnActor<AActor>(
				StoneVisualClass,
				SpawnLocation,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (StoneVisual)
			{
				StoneVisuals.Add(StoneVisual);
			}
		}
	}
}

void UOrbitalStoneComponent::OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value)
{
	if (!bIsUnlocked)
	{
		return;
	}

	if (AbilityID == DamageAbilityID)
	{
		CurrentDamage = AbilityMath::ApplyModifier(CurrentDamage, ModifierType, Value);
		UE_LOG(LogTemp, Log, TEXT("OrbitalStoneComponent: Damage upgraded to %.1f"), CurrentDamage);
	}
	else if (AbilityID == SpeedAbilityID)
	{
		if (CurrentOrbitSpeed >= MaxOrbitSpeed)
		{
			UE_LOG(LogTemp, Warning, TEXT("OrbitalStoneComponent: OrbitSpeed at max (%.0f)"), MaxOrbitSpeed);
			return;
		}

		CurrentOrbitSpeed = AbilityMath::ApplyModifier(CurrentOrbitSpeed, ModifierType, Value, MaxOrbitSpeed);
		UE_LOG(LogTemp, Log, TEXT("OrbitalStoneComponent: OrbitSpeed upgraded to %.0f [Max: %.0f]"),
			CurrentOrbitSpeed, MaxOrbitSpeed);

		if (CurrentOrbitSpeed >= MaxOrbitSpeed && AbilityManager)
		{
			AbilityManager->NotifyLimitReached(SpeedAbilityID);
		}
	}
	else if (AbilityID == RadiusAbilityID)
	{
		if (CurrentOrbitRadius >= MaxOrbitRadius)
		{
			UE_LOG(LogTemp, Warning, TEXT("OrbitalStoneComponent: OrbitRadius at max (%.0f)"), MaxOrbitRadius);
			return;
		}

		CurrentOrbitRadius = AbilityMath::ApplyModifier(CurrentOrbitRadius, ModifierType, Value, MaxOrbitRadius);
		UE_LOG(LogTemp, Log, TEXT("OrbitalStoneComponent: OrbitRadius upgraded to %.0f [Max: %.0f]"),
			CurrentOrbitRadius, MaxOrbitRadius);

		if (CurrentOrbitRadius >= MaxOrbitRadius && AbilityManager)
		{
			AbilityManager->NotifyLimitReached(RadiusAbilityID);
		}
	}
	else if (AbilityID == CountAbilityID)
	{
		if (CurrentStoneCount >= MaxStoneCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("OrbitalStoneComponent: StoneCount at max (%d)"), MaxStoneCount);
			return;
		}

		int32 OldCount = CurrentStoneCount;
		CurrentStoneCount = FMath::Min(CurrentStoneCount + FMath::RoundToInt(Value), MaxStoneCount);

		if (CurrentStoneCount != OldCount)
		{
			UpdateStoneVisualCount();
			UE_LOG(LogTemp, Log, TEXT("OrbitalStoneComponent: StoneCount upgraded to %d [Max: %d]"),
				CurrentStoneCount, MaxStoneCount);
		}

		if (CurrentStoneCount >= MaxStoneCount && AbilityManager)
		{
			AbilityManager->NotifyLimitReached(CountAbilityID);
		}
	}
}