// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/BasicPlayerAnimInstance.h"
//#include "Abilities/GameplayAbility.h"
//#include "Actors/Weapon/WeaponSkill/Skill.h"
#include "Misc/Utils.h"
#include "Data/SkillData.h"
#include "WeaponBase.generated.h"

class AWeaponBase;

USTRUCT()
struct CAVESBASIC_API FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()
	FWeaponTableRow();

public: // Base, SkeletalMesh 또는 StaticMeshComponent 둘 중 하나를 Mesh로 사용
	UPROPERTY(EditAnywhere, Category = "Weapon")
	USkeletalMesh* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FTransform Transform = FTransform::Identity;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

public: // Animation
	UPROPERTY(EditAnywhere, Category = "Weapon|Animation")
	TSubclassOf<UBasicPlayerAnimInstance> AnimClass;

	UPROPERTY(EditAnywhere, Category = "Weapon|Animation")
	UAnimMontage* WeaponAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy|Animation")
	TArray<UAnimMontage*> HitReactMontage;

public:
	// 스킬을 사용 시 Projectile과 Effect를 Spawn하고 싶다면...
	// 스킬 애니메이션을 지정하고 스킬 애니메이션에 Notify를 추가해야 함
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CavesBasic.SkillTableRow"), Category = "Weapon|Skill")
	TArray<FDataTableRowHandle> SkillRowHandle;
};

UCLASS()
class CAVESBASIC_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	virtual void SetData(const FDataTableRowHandle& InDataTableRowHandle);
	const FWeaponTableRow* GetWeaponTableRow() const { return WeaponTableRow; }

	virtual void OnAttack(const FInputActionInstance& Instance);
	void OnSkill(const FInputActionInstance& Instance);
	UFUNCTION()
	void OnSkillMotionMontageEnded(UAnimMontage* Montage, bool bInterrupted);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:	// this
	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	int32 SkillRowHandleNum = 0;

protected: // Owner
	// 이 무기의 소유권자 Pawn
	APawn* OwningPawn = nullptr;
	// 이 무기의 소유권자 Pawn의 SkeletalMeshComponent로 캐릭터의 MeshComponent 같은 것을 의미
	USkeletalMeshComponent* OwningPawnBodyMesh = nullptr;

	UBasicPlayerAnimInstance* BasicAnimInstance = nullptr;

protected:
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CavesBasic.WeaponTableRow"))
	FDataTableRowHandle DataTableRowHandle;

	FWeaponTableRow* WeaponTableRow = nullptr;

protected:
	UInputMappingContext* IMC_Weapon = nullptr;
};
