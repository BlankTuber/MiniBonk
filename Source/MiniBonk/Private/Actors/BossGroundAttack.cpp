#include "Actors/BossGroundAttack.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

ABossGroundAttack::ABossGroundAttack()
{
	PrimaryActorTick.bCanEverTick = true;

	DamageArea = CreateDefaultSubobject<USphereComponent>(TEXT("DamageArea"));
	DamageArea->InitSphereRadius(DamageRadius);
	DamageArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = DamageArea;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetRelativeLocation(FVector(0.f, 0.f, 5.f));
}

void ABossGroundAttack::BeginPlay()
{
	Super::BeginPlay();

	DamageArea->SetSphereRadius(DamageRadius);

	float MeshScale = (DamageRadius / 50.f) * StartScale;
	VisualMesh->SetRelativeScale3D(FVector(MeshScale, MeshScale, 0.05f));

	if (WarningSound)
	{
		WarningAudioComponent = UGameplayStatics::SpawnSoundAttached(
			WarningSound,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			false,
			1.f,
			WarningPitchStart,
			0.f,
			nullptr,
			nullptr,
			true
		);
	}

	GetWorldTimerManager().SetTimer(
		DetonationTimerHandle,
		this,
		&ABossGroundAttack::Detonate,
		WarningDuration,
		false
	);
}

void ABossGroundAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	UpdateVisuals(DeltaTime);
}

void ABossGroundAttack::UpdateVisuals(float DeltaTime)
{
	float Progress = FMath::Clamp(ElapsedTime / WarningDuration, 0.f, 1.f);

	float BaseScale = FMath::Lerp(StartScale, EndScale, Progress);
	float Pulse = FMath::Sin(ElapsedTime * PulseSpeed) * PulseIntensity * Progress;
	float FinalScale = BaseScale + Pulse;

	float MeshScale = (DamageRadius / 50.f) * FinalScale;
	VisualMesh->SetRelativeScale3D(FVector(MeshScale, MeshScale, 0.05f));

	if (WarningAudioComponent)
	{
		float CurrentPitch = FMath::Lerp(WarningPitchStart, WarningPitchEnd, Progress);
		WarningAudioComponent->SetPitchMultiplier(CurrentPitch);
	}
}

void ABossGroundAttack::InitializeAttack(float InDamage)
{
	Damage = InDamage;
}

void ABossGroundAttack::Detonate()
{
	if (WarningAudioComponent)
	{
		WarningAudioComponent->Stop();
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player)
	{
		float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		if (Distance <= DamageRadius)
		{
			UHealthComponent* PlayerHealth = Player->FindComponentByClass<UHealthComponent>();
			if (PlayerHealth)
			{
				PlayerHealth->TakeDamage(Damage);
				UE_LOG(LogTemp, Log, TEXT("BossGroundAttack: Hit player for %.0f damage"), Damage);
			}
		}
	}

	Destroy();
}