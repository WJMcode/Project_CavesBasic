// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUDWidget/HPBar/HPBarWidgetBase.h"
#include "HPBarWidgetDefaultMonster.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API UHPBarWidgetDefaultMonster : public UHPBarWidgetBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	virtual void OnHPChanged(float CurrentHP, float MaxHP) override;
	virtual void OnDie() override;
};
