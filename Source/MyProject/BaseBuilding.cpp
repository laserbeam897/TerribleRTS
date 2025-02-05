// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BaseBuilding.h"
#include "UnrealNetwork.h"


// Sets default values
ABaseBuilding::ABaseBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();

	bReplicates = true;
	if (Role == ROLE_Authority)
	{
		SetStartingHealth();
	}
	SetupHealthBar();
}

void ABaseBuilding::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseBuilding, Health);
	DOREPLIFETIME(ABaseBuilding, StartingHealth);
}

// Called every frame
void ABaseBuilding::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

float ABaseBuilding::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0)
	{
		IsDead = true;
		DeathAnimationMulticast();
		GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ABaseBuilding::DestroyActor, DestroyActorDelay, false);
	}
	return DamageAmount;

}

void ABaseBuilding::DestroyActor()
{
	Destroy();
}

void ABaseBuilding::SetStartingHealth_Implementation()
{
	StartingHealth = Health;
	Health = 0;
}
bool ABaseBuilding::SetStartingHealth_Validate()
{
	return true;
}

void ABaseBuilding::DeathAnimationMulticast_Implementation()
{
	DeathAnimations();
}

bool ABaseBuilding::DeathAnimationMulticast_Validate()
{
	return true;
}

void ABaseBuilding::Repair_Implementation(float MaxAmount)
{
	float CostHPRatio = StartingHealth / Cost;
	float HPIncrease = MaxAmount * CostHPRatio;
	if (!IsDead)
	{
		if (Health + HPIncrease <= StartingHealth)
		{
			Health += HPIncrease;
		}
		else if (Health < StartingHealth)
		{
			Health = StartingHealth;
		}
		if (!ConstructionFinished && Health == StartingHealth)
		{
			ConstructionFinished = true;
		}

	}
}

bool ABaseBuilding::GetIsDead_Implementation()
{
	return IsDead;
}

int32 ABaseBuilding::GetTeamNumber_Implementation()
{
	return TeamNumber;
}

void ABaseBuilding::SetTeamNumber_Implementation(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}

bool ABaseBuilding::NeedsRepair_Implementation()
{
	return Health < StartingHealth;
}