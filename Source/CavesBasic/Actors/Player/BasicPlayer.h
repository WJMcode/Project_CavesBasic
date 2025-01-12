// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Actors/Weapon/WeaponBase.h"
#include "Actors/Effect/CharacterMeshEffect/CharacterMeshEffect.h"
#include "Components/WeaponChildActorComponent.h"
#include "Components/StatusComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/SkillData.h"
#include "Data/PawnData.h"
#include "BasicPlayer.generated.h"

UCLASS()
class CAVESBASIC_API ABasicPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicPlayer();
	const TObjectPtr<UWeaponChildActorComponent> GetWeaponChildActorComponent() const { return WeaponChildActorComponent; }
	UAnimMontage* GetPlayingMontage() const;
	FSkillTableRow* GetSkillTableRow() const;
	UStatusComponent* GetStatusComponent() const { return StatusComponent; }

	const void SetPlayingMontage(UAnimMontage* InPlayingMontage) { CurrentActiveMontage = InPlayingMontage; };

	virtual void ResetCharacterStatus();

protected:
	UFUNCTION()
	virtual void OnDie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void  SetData(FDataTableRowHandle InDataTableRowHandle);

protected:	// 웅크리면 메시가 위로 올라가는 문제를 해결하는 함수들
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// 피격 시 무적 시간
	float InvincibilityTime = 2.5f;

protected:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	// 무기를 직접 멤버로 두지 않고 어떤 무기든 관리할 수 있는 ChildActorComponent를
	// 멤버로 두는 이유는 확장성을 위해서이다.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWeaponChildActorComponent> WeaponChildActorComponent;

	UPROPERTY(VisibleAnywhere)
	UStatusComponent* StatusComponent;

protected:
	//UPROPERTY()
	// 캐릭터 메쉬에 각종 효과를 부여하기 위한 멤버
	//UCharacterMeshEffect* CharacterMeshEffectMgr;
	TObjectPtr<UCharacterMeshEffect> CharacterMeshEffectMgr;

protected:
	UPROPERTY()
	UAnimMontage* CurrentActiveMontage = nullptr;
	UPROPERTY()
	UAnimMontage* CurrentDieMontage = nullptr;

protected:
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CAVESBASIC.PawnTableRow"))
	FDataTableRowHandle DataTableRowHandle;
	
	FPawnTableRow* CharacterData = nullptr;
};
