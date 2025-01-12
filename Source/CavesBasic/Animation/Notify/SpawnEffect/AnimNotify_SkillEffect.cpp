// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/SpawnEffect/AnimNotify_SkillEffect.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Effect/EffectWithDecal.h"
#include "Actors/Weapon/WeaponBase.h"
#include "Components/WeaponChildActorComponent.h"
#include "Actors/Player/BasicPlayer.h"
#include "Actors/Monster/DefaultMonster/DefaultMonster.h"

UAnimNotify_SkillEffect::UAnimNotify_SkillEffect()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(100, 180, 10, 255);
#endif // WITH_EDITORONLY_DATA
}

void UAnimNotify_SkillEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

#if WITH_EDITOR
	if (GIsEditor && MeshComp->GetWorld() != GWorld) { return; } // 에디터 프리뷰
#endif

	APawn* OwningPawn = Cast<APawn>(MeshComp->GetOwner());
	check(OwningPawn);

	UWeaponChildActorComponent* WeaponChildActorComponent = OwningPawn->GetComponentByClass<UWeaponChildActorComponent>();
	check(WeaponChildActorComponent);
	AActor* WeaponActor = WeaponChildActorComponent->GetChildActor();
	check(WeaponActor);
	USkeletalMeshComponent* WeaponSkeletalMeshComponent = WeaponActor->GetComponentByClass<USkeletalMeshComponent>();
	check(WeaponSkeletalMeshComponent);

	const FTransform ProjectileStartTransform = WeaponSkeletalMeshComponent->GetSocketTransform(SocketName::ProjectileStart);
	FTransform ProjectileStartNoScaleTransform;
	ProjectileStartNoScaleTransform.SetLocation(ProjectileStartTransform.GetLocation());
	// ProjectileStart Socket의 회전값을 그대로 반영하면 Effect가 이상하게 회전하므로
	// Effect를 발생시킨 OwningPawn의 회전값을 얻어와 Effect를 회전시킨다.
	// 이러면 OwningPawn이 바라보는 쪽으로 Effect가 발생함
	ProjectileStartNoScaleTransform.SetRotation(OwningPawn->GetActorRotation().Quaternion());
	

	const FTransform EffectTransform = ProjectileStartNoScaleTransform;

	//==============================================================
	AWeaponBase* Weapon = Cast<AWeaponBase>(WeaponActor);
	const FWeaponTableRow* WeaponTableRow = Weapon->GetWeaponTableRow();
	ensure(WeaponTableRow);

	// SkillEffect를 발생시킨 객체가 플레이어인지, 몬스터인지 
	// 파악한 후 해당 객체의 스킬 데이터 테이블을 불러온다.
	FSkillTableRow* SkillTableRow = nullptr;
	ABasicPlayer* OwningPlayer = nullptr;
	ADefaultMonster* OwningMonster = nullptr;
	if (OwningPawn->IsA<ABasicPlayer>())
	{
		OwningPlayer = Cast<ABasicPlayer>(OwningPawn);
		check(OwningPlayer);
		SkillTableRow = OwningPlayer->GetSkillTableRow();
		ensure(SkillTableRow);
	}
	else if (OwningPawn->IsA<ADefaultMonster>())
	{
		OwningMonster = Cast<ADefaultMonster>(OwningPawn);
		check(OwningMonster);
		SkillTableRow = OwningPlayer->GetSkillTableRow();
		ensure(SkillTableRow);
	}
	else
	{
		checkf(false, TEXT("It's neither BasicCharacter nor MonsterCharacter"))
	}

	// Projectile 데이터 테이블의 Row가 하나만 지정되어 있으므로
	// 해당 Projectile 데이터 테이블의 Row(행)를 알아서 가져온다.
	const FProjectileTableRow* ProjectileTableRow = SkillTableRow->ProjectileRowHandle.GetRow<FProjectileTableRow>(TEXT("Auto"));
	check(ProjectileTableRow);

	// Effect 데이터 테이블의 Row가 하나만 지정되어 있으므로
	// 해당 Effect 데이터 테이블의 Row(행)를 알아서 가져온다.
	const FEffectDecalTableRow* EffectTableRow = ProjectileTableRow->EffectTableRowHandle.GetRow<FEffectDecalTableRow>(TEXT("Auto"));
	check(EffectTableRow);
	//==============================================================		


	AEffect* Effect = MeshComp->GetWorld()->SpawnActorDeferred<AEffect>(EffectTableRow->EffectClass,
		FTransform::Identity, Weapon);
	Effect->SetData(ProjectileTableRow->EffectTableRowHandle);
	Effect->FinishSpawning(EffectTransform);
	Effect->Play();

}
