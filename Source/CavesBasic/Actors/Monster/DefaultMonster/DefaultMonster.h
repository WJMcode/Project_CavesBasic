// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/PawnData.h"
#include "Components/WeaponChildActorComponent.h"
#include "Components/StatusComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/WidgetComponent.h"
#include "Actors/Monster/AIController/DefaultMonsterAIController.h"
#include "Data/SkillData.h"
#include "DefaultMonster.generated.h"

UCLASS()
class CAVESBASIC_API ADefaultMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADefaultMonster();
	UAnimMontage* GetPlayingMontage() const { return CurrentActiveMontage; }
	const void SetPlayingMontage(UAnimMontage* InPlayingMontage) { CurrentActiveMontage = InPlayingMontage; };

	FSkillTableRow* GetSkillTableRow() const;

protected:
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnDie();
	UFUNCTION()
	virtual void OnDisappearMesh(float InDissolve);
	UFUNCTION()
	virtual void OnDisappearMeshEnd();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	virtual void  SetData(FDataTableRowHandle InDataTableRowHandle);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Player가 떨어지면 호출됨
	virtual void Falling() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    // AIController를 기본값으로 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TSubclassOf<AAIController> MonsterAIControllerClass;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HPWidgetComponent;

	// 무기를 직접 멤버로 두지 않고 어떤 무기든 관리할 수 있는 ChildActorComponent를
	// 멤버로 두는 이유는 확장성을 위해서이다.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWeaponChildActorComponent> WeaponChildActorComponent;

	UPROPERTY(VisibleAnywhere)
	UStatusComponent* StatusComponent;

protected:
	UAnimMontage* CurrentActiveMontage;
protected:
	UAnimMontage* CurrentDieMontage;


protected: // 몬스터 사망 시 점점 사라지는 효과를 주기 위한 멤버 변수들
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DisappearTimelineComponent;
	UMaterialInstanceDynamic* MaterialInstanceDynamics;
	
	UPROPERTY(VisibleAnywhere)
	UCurveFloat* DisappearCurve;


protected:
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CAVESBASIC.PawnTableRow"))
	FDataTableRowHandle DataTableRowHandle;

	FPawnTableRow* MonsterData;
};
