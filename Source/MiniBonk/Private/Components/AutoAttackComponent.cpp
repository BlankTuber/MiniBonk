#include "Components/AutoAttackComponent.h"
#include "Systems/AbilityManagerComponent.h"
#include "Systems/AbilityMath.h"
#include "Characters/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

UAutoAttackComponent::UAutoAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentDamage = BaseDamage;
	CurrentCooldown = BaseCooldown;
	CurrentRange = BaseRange;

}


void UAutoAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentDamage = BaseDamage;
	CurrentCooldown = BaseCooldown;
	CurrentRange = BaseRange;

	AbilityManager = GetOwner()->FindComponentByClass<UAbilityManagerComponent>();
	if (AbilityManager)
	{
		AbilityManager->OnPassiveCardApplied.AddDynamic(this, &UAutoAttackComponent::OnPassiveCardApplied);
	}

	StartAttackTimer();
}

TArray<AActor*> UAutoAttackComponent::FindTargets() const
{
	TArray<AActor*> FoundTargets;

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FoundTargets;
	}

	FVector OwnerLocation = Owner->GetActorLocation();

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), AllEnemies);

	TArray<TPair<float, AActor*>> EnemiesInRange;
	for (AActor* Enemy : AllEnemies)
	{
		float Distance = FVector::Dist(OwnerLocation, Enemy->GetActorLocation());
		if (Distance <= CurrentRange)
		{
			EnemiesInRange.Add(TPair<float, AActor*>(Distance, Enemy));
		}
	}

	EnemiesInRange.Sort([](const TPair<float, AActor*>& A, const TPair<float, AActor*>& B)
	{
		return A.Key < B.Key;
	});

	int32 NumToReturn = FMath::Min(TargetCount, EnemiesInRange.Num());
	for (int32 i = 0; i < NumToReturn; ++i)
	{
		FoundTargets.Add(EnemiesInRange[i].Value);
	}

	return FoundTargets;
}

void UAutoAttackComponent::StartAttackTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&UAutoAttackComponent::OnAttackTimerExpired,
		CurrentCooldown,
		false
	);
}

void UAutoAttackComponent::OnAttackTimerExpired()
{
	TArray<AActor*> Targets = FindTargets();

	if (Targets.Num() > 0)
	{
		PerformAttack(Targets);
	}

	StartAttackTimer();
}

void UAutoAttackComponent::OnPassiveCardApplied(FName CardAbilityID, EModifierType ModifierType, float Value)
{
	if (CardAbilityID != AbilityID)
	{
		return;
	}

	CurrentDamage = AbilityMath::ApplyModifier(CurrentDamage, ModifierType, Value);

	UE_LOG(LogTemp, Log, TEXT("AutoAttackComponent %s: Damage upgraded to %f"), *AbilityID.ToString(), CurrentDamage);
}