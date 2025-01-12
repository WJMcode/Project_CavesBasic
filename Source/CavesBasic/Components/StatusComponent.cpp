// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StatusComponent.h"
#include "Misc/Utils.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	AActor* a = GetOwner();
}


// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());

	// 캐릭터가 피격 당하거나 무적 버프 등을 받았을 때 무적 관련 CollisionProfileName으로 세팅이 되는데,
	// 무적 시간이 끝난 후 원래 CollisionProfileName으로 돌아오기 위해 기존 것을 저장해 둠
	OriginalCollisionProfileName = OwningCharacter->GetCapsuleComponent()->GetCollisionProfileName();
}


// Called every frame
void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatusComponent::ActivateInvincibility(const float Duration)
{
	// Example of checking for a nullptr
	if (OwningCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UStatusComponent::ActivateInvincibility <OwningCharacter> is nullptr in ActivateInvincibility"));
		return;
	}

	bIsInvincible = true;
	OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName(CollisionProfileName::Invincible);

	FTimerHandle InvincibilityTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &ThisClass::DeactivateInvincibility, Duration, false);
}

void UStatusComponent::DeactivateInvincibility()
{
	bIsInvincible = false;
	 
	OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName(OriginalCollisionProfileName);
}

float UStatusComponent::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDie) { return 0.f; }
	if (FMath::IsNearlyZero(Damage)) { return 0.0; }
	float NewDamage = Damage;

	// Ex. NewDamage -= Armor;
	NewDamage = FMath::Clamp(NewDamage, 0.f, NewDamage);

	HP -= NewDamage;
	HP = FMath::Clamp(HP, 0.f, HP);

	LastInstigator = EventInstigator;
	OnHPChanged.Broadcast(HP, MaxHP);

	if (HP == 0.f)
	{
		bDie = true;
		OnDie.Broadcast();
	}

	return NewDamage;
}

void UStatusComponent::RespawnStatus()
{
	SetHP(MaxHP, 1);
}

