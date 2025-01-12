// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Actors/Projectile/Projectile.h"
#include "AnimNotify_SpawnSkillProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API UAnimNotify_SpawnSkillProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_SpawnSkillProjectile();

protected:
	virtual FRotator GetPlayerProjectileRotation(const FVector& InProjectSpawnLocation, USceneComponent* InSceneComp, UAnimSequenceBase* Animation);

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	FProjectileTableRow* ProjectileTableRow;
};
