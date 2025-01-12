// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapon/WeaponBase.h"
#include "Abilities/GameplayAbility.h"
#include "Staff.generated.h"

/**
 * 
 */

UCLASS()
class CAVESBASIC_API AStaff : public AWeaponBase
{
	GENERATED_BODY()

public:	
	virtual void SetData(const FDataTableRowHandle& InDataTableRowHandle) override;
};