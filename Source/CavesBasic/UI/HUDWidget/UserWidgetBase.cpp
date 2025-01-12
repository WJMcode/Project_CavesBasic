// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDWidget/UserWidgetBase.h"

void UUserWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UUserWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UUserWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	ensure(OwningPawn);
}
