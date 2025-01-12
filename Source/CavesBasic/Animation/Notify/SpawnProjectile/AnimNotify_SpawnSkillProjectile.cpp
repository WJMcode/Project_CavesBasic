// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/SpawnProjectile/AnimNotify_SpawnSkillProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/Utils.h"
#include "Data/SkillData.h"
#include "Actors/Player/BasicPlayer.h"

UAnimNotify_SpawnSkillProjectile::UAnimNotify_SpawnSkillProjectile()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 255, 255, 255);
#endif // WITH_EDITORONLY_DATA
}

FRotator UAnimNotify_SpawnSkillProjectile::GetPlayerProjectileRotation(const FVector& InProjectSpawnLocation, USceneComponent* InSceneComp, UAnimSequenceBase* Animation)
{
	// 이동 거리 계산 : 
	// Distance = Speed × Time, 
	//	-> Distance = InitialSpeed × ProjectileLifeSpan
	const float Speed = ProjectileTableRow->InitialSpeed;
	const float LifeSpan = ProjectileTableRow->ProjectileLifeSpan;
	const float Distance = Speed * LifeSpan;

	// RootComponent의 회전값을 통해 Projectile 발사
	// Mesh는 x축을 바라보도록 이미 돌아가 있어서 Projectile의 방향이 이상함.
	// 그래서 RootComponent의 회전값을 이용함.
	const FVector InstigatorForwardVector = InSceneComp->GetForwardVector();
	// 맨 뒤에 곱해주는 숫자는, Projectile이 캐릭터를 기준으로 어느정도의 거리를 목표 지점으로 정하고 날아갈 것인지 정해줌.	
	// Projectile에 있는 ProjectileGravityScale값의 영향을 받음
	const FVector DestinationLocation = InSceneComp->GetComponentLocation() + InstigatorForwardVector * Distance;

	TArray<AActor*> IgnoreActors; IgnoreActors.Add(InSceneComp->GetOwner());
	FHitResult HitResult;

	const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4);
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(InSceneComp->GetWorld(),
		InSceneComp->GetComponentLocation(), DestinationLocation, TraceTypeQuery,
		false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);

	FRotator Rotator;
	if (bHit)
	{
		Rotator = UKismetMathLibrary::FindLookAtRotation(InProjectSpawnLocation, HitResult.ImpactPoint);
	}
	else
	{
		Rotator = UKismetMathLibrary::FindLookAtRotation(InProjectSpawnLocation, DestinationLocation);
	}
	// Yaw만 필요함
	// JM : 오브젝트에 붙어 스킬을 사용하면 Projectile의 Yaw값이 반전되어
	// 반대로 발사되는 문제는 에디터 내의 Collision 파일 수정을 통해 해결
	Rotator.Roll = 0;
	Rotator.Pitch = 0;
	return Rotator;
}

void UAnimNotify_SpawnSkillProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

#if WITH_EDITOR
	if (GIsEditor && MeshComp->GetWorld() != GWorld) { return; } // 에디터 프리뷰
#endif

	ABasicPlayer* OwningCharacter = Cast<ABasicPlayer>(MeshComp->GetOwner());
	check(OwningCharacter);

	// 무기 얻어오기
	UWeaponChildActorComponent* WeaponChildActorComponent = OwningCharacter->GetComponentByClass<UWeaponChildActorComponent>();
	check(WeaponChildActorComponent);
	AActor* WeaponActor = WeaponChildActorComponent->GetChildActor();
	check(WeaponActor);

	// Skill Data Table 얻어오기
	const FSkillTableRow* SkillTableRow = OwningCharacter->GetSkillTableRow();
	ensure(SkillTableRow);

	// Projectile 데이터 테이블의 Row가 하나만 지정되어 있으므로
	// 해당 Projectile 데이터 테이블의 Row(행)를 알아서 가져온다.
	ProjectileTableRow = SkillTableRow->ProjectileRowHandle.GetRow<FProjectileTableRow>(TEXT("Auto"));
	check(ProjectileTableRow);
	
	FVector ProjectileStartLocation = FVector::ZeroVector;

	UStaticMeshComponent* WeaponStaticMeshComponent = WeaponActor->GetComponentByClass<UStaticMeshComponent>();
	check(WeaponStaticMeshComponent);
	if (WeaponStaticMeshComponent->GetStaticMesh())
	{
		ProjectileStartLocation = WeaponStaticMeshComponent->GetSocketLocation(SocketName::ProjectileStart);
#if WITH_EDITOR
		UStaticMeshSocket const* StaticMeshSocket = WeaponStaticMeshComponent->GetSocketByName(SocketName::ProjectileStart);
		check(StaticMeshSocket);
#endif
	}

	USkeletalMeshComponent* WeaponSkeletalMeshComponent = WeaponActor->GetComponentByClass<USkeletalMeshComponent>();
	check(WeaponSkeletalMeshComponent);
	if (WeaponSkeletalMeshComponent->GetSkeletalMeshAsset())
	{
		ProjectileStartLocation = WeaponSkeletalMeshComponent->GetSocketLocation(SocketName::ProjectileStart);
#if WITH_EDITOR
		USkeletalMeshSocket const* SkeletalMeshSocket = WeaponSkeletalMeshComponent->GetSocketByName(SocketName::ProjectileStart);
		check(SkeletalMeshSocket);
#endif
	}

	FRotator ProjectileRotator = FRotator::ZeroRotator;
	bool bIsPlayer = false;

	if (USceneComponent* SceneComponent = OwningCharacter->GetRootComponent())
	{
		bIsPlayer = true;
		ProjectileRotator = GetPlayerProjectileRotation(ProjectileStartLocation, SceneComponent, Animation);
	}
	else
	{
		// 구현 필요
		check(false);
	}

	UWorld* World = OwningCharacter->GetWorld();
	AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(ProjectileTableRow->ProjectileClass,
		FTransform::Identity, OwningCharacter, OwningCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Projectile->SetData(SkillTableRow->ProjectileRowHandle);

	FTransform NewTransform;
	// ProjectileStartLocation는 무기의 ProjectileStart Socket의 위치임
	NewTransform.SetLocation(ProjectileStartLocation);
	NewTransform.SetRotation(ProjectileRotator.Quaternion());
	Projectile->SetAdjustLocation(NewTransform);
	Projectile->FinishSpawning(NewTransform);
}
