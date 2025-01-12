// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "WeaponChildActorComponent.generated.h"

/**
 * 
 */
// 해당 클래스는 WeaponBase의 부모 클래스와 같은 역할임
UCLASS()
class CAVESBASIC_API UWeaponChildActorComponent : public UChildActorComponent
{
	GENERATED_BODY()
	
public:
	UWeaponChildActorComponent();
	void SetData(FDataTableRowHandle InDataTableRowHandle);
};
