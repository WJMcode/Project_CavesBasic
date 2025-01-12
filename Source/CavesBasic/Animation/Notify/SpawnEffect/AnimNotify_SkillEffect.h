// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Misc/Utils.h"
#include "AnimNotify_SkillEffect.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API UAnimNotify_SkillEffect : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_SkillEffect();

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	// SocketName to attach to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FName SocketName = SocketName::ProjectileStart;
};
