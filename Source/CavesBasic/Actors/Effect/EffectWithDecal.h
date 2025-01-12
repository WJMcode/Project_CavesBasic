// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "CoreMinimal.h"
#include "Actors/Effect/Effect.h"
#include "Components/DecalComponent.h"
#include "EffectWithDecal.generated.h"

/**
 * 
 */
USTRUCT()
struct CAVESBASIC_API FEffectDecalTableRow : public FEffectTableRow
{
	GENERATED_BODY()
	FEffectDecalTableRow();

public:
	UPROPERTY(EditAnywhere, Category = "DecalEffect")
	TSubclassOf<AEffectWithDecal> EffectWithDecalClass;

public:
	UPROPERTY(EditAnywhere, Category = "DecalEffect|Sound")
	TObjectPtr<USoundBase> DecalSound;
	UPROPERTY(EditAnywhere, Category = "DecalEffect|Sound")
	float DecalVolumeMultiplier = 1.f;

public:
	UPROPERTY(EditAnywhere, Category = "DecalEffect|OverlapParticle")
	TObjectPtr<UParticleSystem> ProjectileOverlapParticle;

	UPROPERTY(EditAnywhere, Category = "DecalEffect|OverlapParticle")
	FTransform OverlapParticleTransform = FTransform::Identity;
};

UCLASS()
class CAVESBASIC_API AEffectWithDecal : public AEffect
{
	GENERATED_BODY()
	
public:
	AEffectWithDecal();
	virtual void SetData(const FDataTableRowHandle& InDataTableRowHandle) override;

	virtual void Play() override;
	virtual void PlaySound() override;
	virtual void PlayParticle() override;

protected:
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* DecalParticleSystemComponent = nullptr;

protected: // Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecalEffect|Sound")
	TObjectPtr<USoundBase> DecalSound;

	// Volume Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecalEffect|Sound")
	float DecalVolumeMultiplier = 1.f;

protected:
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CavesBasic.EffectDecalTableRow"))
	FDataTableRowHandle DecalDataTableRowHandle;

	FEffectDecalTableRow* EffectDecalData;
};
