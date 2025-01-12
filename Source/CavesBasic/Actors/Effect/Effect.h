// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Effect.generated.h"

USTRUCT()
struct CAVESBASIC_API FEffectTableRow : public FTableRowBase
{
	GENERATED_BODY()
	FEffectTableRow();

public:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TSubclassOf<AEffect> EffectClass;

public:
	UPROPERTY(EditAnywhere, Category = "Effect|Sound")
	TObjectPtr<USoundBase> Sound;
	UPROPERTY(EditAnywhere, Category = "Effect|Sound")
	float VolumeMultiplier = 1.f;

public:
	UPROPERTY(EditAnywhere, Category = "Effect|SpawnParticle")
	TObjectPtr<UParticleSystem> ProjectileSpawnParticle;

	// Projectile Spawn Particle에 설정된 Effect를 연 후에,
	// Emitter의 "필수" - "로컬 스페이스 사용"을 확인할 것. true여야 회전값이 적용됨
	UPROPERTY(EditAnywhere, Category = "Effect|SpawnParticle")
	FTransform SpawnParticleTransform = FTransform::Identity;
};

UCLASS()
class CAVESBASIC_API AEffect : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AEffect();
	FEffectTableRow* GetEffectTableRow() const { return EffectData; }
	virtual void SetData(const FDataTableRowHandle& InDataTableRowHandle);

	virtual void Play();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PlaySound();
	virtual void PlayParticle();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultSceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* SpawnParticleSystemComponent = nullptr;

protected: // Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Sound")
	TObjectPtr<USoundBase> Sound;

	// Volume Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Sound")
	float VolumeMultiplier = 1.f;

protected:
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CavesBasic.EffectTableRow"))
	FDataTableRowHandle DataTableRowHandle;

	FEffectTableRow* EffectData;
};
