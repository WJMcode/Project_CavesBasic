// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapon/Staff.h"

void AStaff::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
	Super::SetData(InDataTableRowHandle);
	check(WeaponTableRow);
}
