// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/SkillData.h"
#include "Skill.generated.h"

/**
 * 
 */

UCLASS()
class CAVESBASIC_API USkill : public UGameplayAbility
{
	GENERATED_BODY()

	friend class AWeaponBase;
public:
	USkill();
public:
	
	UAnimMontage* GetSkillMotion() const { return SkillData->SkillMotionMontage; }

protected:
	virtual void SetData(const FDataTableRowHandle& InDataTableRowHandle); // 스킬 데이터 초기화

protected:
	TObjectPtr<UAnimMontage> SkillMotionMontage = nullptr;
	TSubclassOf<AActor> ProjectileClass;
	USoundBase* FireSound;
	UParticleSystem* FireEffect;

protected:	
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CavesBasic.SkillTableRow"))
	FDataTableRowHandle DataTableRowHandle;

	const FSkillTableRow* SkillData = nullptr;

};
