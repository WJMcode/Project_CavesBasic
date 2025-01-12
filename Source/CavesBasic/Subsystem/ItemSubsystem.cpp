// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ItemSubsystem.h"

UItemSubsystem::UItemSubsystem()
{
	// GameInstanceSubsystem에서 게임 전반에 걸쳐 사용될 수 있는
	// 오브젝트를 미리 찾아 놓는다. 
	//jmpretest
	/*{
		static ConstructorHelpers::FObjectFinder<UDataTable> Asset(TEXT("/Script/Engine.DataTable'/Game/Data/WeaponData/DT_WeaponSword.DT_WeaponSword'"));
		check(Asset.Object);
		WeaponBaseDataTable = Asset.Object;
	}
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> Asset(TEXT("/Script/Engine.DataTable'/Game/Data/WeaponData/DT_WeaponStaff.DT_WeaponStaff'"));
		check(Asset.Object);
		StaffDataTable = Asset.Object;
	}*/

	//jmtest
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> Asset(TEXT("/Script/Engine.DataTable'/Game/Data/WeaponData/DT_Weapon.DT_Weapon'"));
		check(Asset.Object);
		WeaponDataTable = Asset.Object;
	}
}

FDataTableRowHandle UItemSubsystem::FindItem(const FName& InKey)
{
	FDataTableRowHandle DataTableRowHandle;

	// 에디터에서는 서로 다른 타입의 무기 테이블에 
	// 같은 이름을 가진 Row가 있을 때, 중복된 이름이 있다고 check를 통해 알려줌.
#if WITH_EDITOR
	bool bFind = false;
	if (WeaponDataTable->GetRowMap().Find(InKey))
	{
		check(!bFind);
		bFind = true;
		DataTableRowHandle.DataTable = WeaponDataTable;
		DataTableRowHandle.RowName = InKey;
	}
	/*if (StaffDataTable->GetRowMap().Find(InKey))
	{
		check(!bFind);
		bFind = true;
		DataTableRowHandle.DataTable = StaffDataTable;
		DataTableRowHandle.RowName = InKey;
	}*/
	// 인게임에서는 에디터에서 이미 중복 방지를 했다고 가정하고
	// 먼저 검사한 무기 테이블에 해당하는 무기 이름이 존재하면
	// 그 무기를 얻어오고 다른 테이블은 확인하지 않음
#else
	if (WeaponDataTable->GetRowMap().Find(InKey))
	{
		DataTableRowHandle.DataTable = WeaponDataTable;
		DataTableRowHandle.RowName = InKey;
	}
	/*else if (GunDataTable->GetRowMap().Find(InKey))
	{
		DataTableRowHandle.DataTable = GunDataTable;
		DataTableRowHandle.RowName = InKey;
	}*/
#endif

	return DataTableRowHandle;
}
