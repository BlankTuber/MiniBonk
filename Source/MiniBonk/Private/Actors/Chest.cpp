#include "Actors/Chest.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/MinibonkPlayerController.h"
#include "Components/CoinComponent.h"
#include "Components/LevelComponent.h"
#include "UI/MinibonkHUD.h"
#include "Kismet/GameplayStatics.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->InitSphereRadius(InteractionRadius);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = InteractionSphere;

	InteractionLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("InteractionLight"));
	InteractionLight->SetupAttachment(RootComponent);
	InteractionLight->SetIntensity(LightIntensity);
	InteractionLight->SetAttenuationRadius(LightRadius);
	InteractionLight->SetVisibility(false);
}

void AChest::BeginPlay()
{
	Super::BeginPlay();

	CalculateCost();

	if (ensure(InteractionSphere))
	{
		InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnInteractionBeginOverlap);
		InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AChest::OnInteractionEndOverlap);
	}

	if (InteractionLight)
	{
		InteractionLight->SetIntensity(LightIntensity);
		InteractionLight->SetAttenuationRadius(LightRadius);
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	int32 PlayerLevel = (Player && Player->LevelComponent) ? Player->LevelComponent->GetCurrentLevel() : 1;
	UE_LOG(LogTemp, Log, TEXT("Chest spawned - Cost: %d (Player Level: %d)"), CurrentCost, PlayerLevel);
}

void AChest::CalculateCost()
{
	int32 PlayerLevel = 1;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player && Player->LevelComponent)
	{
		PlayerLevel = Player->LevelComponent->GetCurrentLevel();
	}

	float CostMultiplier = FMath::Pow(CostGrowthRate, static_cast<float>(PlayerLevel - 1));
	CurrentCost = FMath::RoundToInt(BaseCost * CostMultiplier);
}

bool AChest::CanPlayerAfford() const
{
	if (!OverlappingPlayer.IsValid())
	{
		return false;
	}

	UCoinComponent* CoinComp = OverlappingPlayer->CoinComponent;
	if (!CoinComp)
	{
		return false;
	}

	return CoinComp->GetCurrentCoins() >= CurrentCost;
}

bool AChest::TryPurchase()
{
	if (!OverlappingPlayer.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Chest: No player in range"));
		return false;
	}

	if (!CanPlayerAfford())
	{
		UE_LOG(LogTemp, Warning, TEXT("Chest: Player cannot afford (Cost: %d)"), CurrentCost);
		return false;
	}

	UCoinComponent* CoinComp = OverlappingPlayer->CoinComponent;
	if (!CoinComp)
	{
		return false;
	}

	CoinComp->SpendCoins(CurrentCost);

	UE_LOG(LogTemp, Log, TEXT("Chest: Purchased for %d coins!"), CurrentCost);

	UGameplayStatics::SetGamePaused(GetWorld(), true);

	AMinibonkPlayerController* PC = Cast<AMinibonkPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		UMinibonkHUD* HUD = PC->GetHUDWidget();
		if (HUD)
		{
			HUD->TriggerCardSelection(FText::FromString(TEXT("Chest Reward")));
		}
	}

	Destroy();
	return true;
}

void AChest::UpdateInteractionPrompt()
{
	AMinibonkPlayerController* PC = Cast<AMinibonkPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PC)
	{
		return;
	}

	UMinibonkHUD* HUD = PC->GetHUDWidget();
	if (!HUD)
	{
		return;
	}

	FText PromptText = FText::Format(INVTEXT("Press E - {0} coins"), CurrentCost);
	HUD->ShowInteractionPrompt(PromptText, CanPlayerAfford());
}

void AChest::UpdateVisualFeedback()
{
	if (!InteractionLight)
	{
		return;
	}

	bool bInRange = OverlappingPlayer.IsValid();
	InteractionLight->SetVisibility(bInRange);

	if (bInRange)
	{
		FLinearColor LightColor = CanPlayerAfford() ? CanAffordColor : CannotAffordColor;
		InteractionLight->SetLightColor(LightColor);
	}
}

void AChest::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	OverlappingPlayer = Player;
	Player->CurrentInteractable = this;

	UpdateInteractionPrompt();
	UpdateVisualFeedback();

	if (Player->CoinComponent)
	{
		Player->CoinComponent->OnCoinsChanged.AddDynamic(this, &AChest::OnPlayerCoinsChanged);
	}

	UE_LOG(LogTemp, Log, TEXT("Chest: Player entered interaction range. Cost: %d"), CurrentCost);
}

void AChest::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (Player->CoinComponent)
	{
		Player->CoinComponent->OnCoinsChanged.RemoveDynamic(this, &AChest::OnPlayerCoinsChanged);
	}

	if (Player->CurrentInteractable == this)
	{
		Player->CurrentInteractable = nullptr;
	}

	OverlappingPlayer = nullptr;

	UpdateVisualFeedback();

	AMinibonkPlayerController* PC = Cast<AMinibonkPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		UMinibonkHUD* HUD = PC->GetHUDWidget();
		if (HUD)
		{
			HUD->HideInteractionPrompt();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Chest: Player left interaction range"));
}

void AChest::OnPlayerCoinsChanged(int32 CurrentCoins, int32 Delta)
{
	UpdateInteractionPrompt();
	UpdateVisualFeedback();
}