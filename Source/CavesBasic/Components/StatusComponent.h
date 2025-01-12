// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDie);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAVESBASIC_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

	void SetIsPlayer(const bool bFlag) { bIsPlayer = bFlag; }
	void SetAttack(const bool bFlag) { bAttack = bFlag; }
	void SetInvincible(const bool bFlag) { bIsInvincible = bFlag; }
	void SetHP(const float InMaxHP, const float InHP) { MaxHP = InMaxHP; HP = InHP; }
	void SetBodyCollisionDamage(const float InCollisionDmg) { BodyCollisionDamage = InCollisionDmg; }
	
	bool CanMove() const { return !bAttack && !bDie; }
	bool IsDie() const { return bDie; }
	bool IsPlayer() const { return bIsPlayer; }
	bool IsInvincible() const { return bIsInvincible; }
	
	float GetMaxHP() const { return MaxHP; }
	float GetHP() const { return HP; }
	float GetBodyCollisionDamage() const { return BodyCollisionDamage; }

	AController* GetLastInstigator() const { return LastInstigator; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void ActivateInvincibility(const float Duration);
	virtual void DeactivateInvincibility();
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

public:
	virtual void RespawnStatus();

protected:
	bool bIsPlayer = true;
	bool bAttack = false;
	bool bIsInvincible = false;
	bool bDie = false;
	float MaxHP = 0.f;
	float HP = 0.f;
	float BodyCollisionDamage = 0.f;

	FName OriginalCollisionProfileName;

protected:
	UPROPERTY()
	ACharacter* OwningCharacter = nullptr;

public:
	UPROPERTY()
	AController* LastInstigator = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnHPChanged OnHPChanged;
	FOnDie OnDie;
};
