// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API UUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void SetOwningPawn(APawn* NewPawn) { OwningPawn = NewPawn; }
	APawn* GetOwningPawn() const { return OwningPawn; }

protected:
	UPROPERTY()
	APawn* OwningPawn = nullptr;
};
