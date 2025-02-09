// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile/StraightProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Effect/EffectWithDecal.h"
#include "Misc/Utils.h"
#include "Components/StatusComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Actors/Player/BasicPlayer.h"
#include "Data/SkillData.h"
#include "Actors/Monster/DefaultMonster/DefaultMonster.h"


// JM@TODO : DetectDamageTarget로 Projectile을 쏘는 순간 범위 내에 몬스터가 있는지 감지
// 감지했다면 감지한 액터의 위치를 얻어와서 계속 따라감

void AStraightProjectile::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
	Super::SetData(InDataTableRowHandle);

	// 이동 거리 계산 : 
	// Distance = Speed × Time, 
	//	-> Distance = InitialSpeed × ProjectileLifeSpan
	Speed = ProjectileData->InitialSpeed;
	LifeSpan = ProjectileData->ProjectileLifeSpan;
	Distance = ProjectileData->InitialSpeed * ProjectileData->ProjectileLifeSpan;
	DetectionRange = ProjectileData->InitialSpeed * ProjectileData->ProjectileLifeSpan;
}

void AStraightProjectile::BeginPlay()
{
	Super::BeginPlay();

	DetectActor = DetectDamageTarget();
}

void AStraightProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DetectActor)
	{
		FollowDamageTarget(DetectActor);
	}
}

AActor* AStraightProjectile::DetectDamageTarget()
{
	FHitResult DetectResult;
	{
	TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());

		FVector TraceStartLocation = GetActorLocation();  // Trace 시작 위치
		FVector TraceDirection = GetActorForwardVector();  // 예: 전방 벡터 (정확한 방향은 상황에 따라 다를 수 있음)

		// 새로운 위치 계산
		FVector TraceEndLocation = TraceStartLocation + (TraceDirection * Distance);

		// StraightProjectile의 크기를 얻어와서 
		FVector Origin;
		FVector BoxExtent;
		GetActorBounds(false, Origin, BoxExtent);
		
		// Projectile의 자식으로 붙어있는 파티클 시스템의 크기는 빼준다.
		if (ProjectileMeshEffectComponent)
		{
			// 파티클 시스템의 크기 계산
			FBox ParticleBounds = ProjectileMeshEffectComponent->Bounds.GetBox();
			FVector ParticleExtent = ParticleBounds.GetExtent();

			// 파티클 시스템 크기를 반영하지 않으려면 BoxExtent에서 빼기
			BoxExtent -= ParticleExtent;
		}

		// 감지 범위 조절
		BoxExtent.Y += 150;
		BoxExtent.Z += 150;
		FVector DetectRange = BoxExtent;

		// Projectile을 기준으로 BoxTrace를 발사하여 
		// 가장 가까운 위치에 존재하는 오브젝트와 충돌하는 순간의 위치를 얻어옴
		// ECollisionChannel::ECC_GameTraceChannel8 은 MonsterDetectTraceChannel 이다.
		// 즉 해당 Trace는 MonsterDetectTraceChannel로 발사되는 Trace이다. 
		// 발사된 해당 Trace는 Collision이 Monster로 설정된 오브젝트가 감지하여 Block 처리를 한다.
		// GroundProjectile 스킬을 사용했을 때 Monster로 설정된 오브젝트를 감지하여 데미지를 주는 것이 목적.
		const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel8);
		const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(),
			TraceStartLocation, TraceEndLocation, DetectRange, GetOwner()->GetActorRotation(), TraceTypeQuery,
			false, IgnoreActors, EDrawDebugTrace::ForDuration, DetectResult, true);	// Owner의 Rotation을 넣어주는 이유는 Projectile의 방향이 Owner Rotation의 영향을 받기 때문이다.
		if (bHit)
		{
			AActor* TraceDetectActor = DetectResult.GetActor();

			if (TraceDetectActor)
			{
				return TraceDetectActor;
			}
		}
	}
	return nullptr;
}

void AStraightProjectile::FollowDamageTarget(AActor* TargetActor)
{
	// 감지된 Actor를 따라가는 함수
	//FollowDamageTarget(DetectActor);
	// 타겟 방향을 계산.
	FVector DirectionToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	// 발사체를 타겟 방향으로 회전시킴.
	FRotator NewRotation = DirectionToTarget.Rotation();
	SetActorRotation(NewRotation);

	// 속도에 방향을 반영해서 이동.
	ProjectileMovementComponent->Velocity = DirectionToTarget * ProjectileData->InitialSpeed;
}

void AStraightProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(this)) { return; }

	// StatusComponent가 존재한다는 것은 플레이어일 수도 있다는 것
	// StatusComponent가 존재한다면 Projectile과 충돌한 것이 자기 자신,
	// 또는 다른 플레이어인지 체크한다.
	UStatusComponent* OtherActorStatusComponent = OtherActor->FindComponentByClass<UStatusComponent>();
	if (OtherActorStatusComponent)
	{
		if (OtherActor == GetOwner() || OtherActorStatusComponent->IsPlayer()) { return; }
	}

	FTransform NewTransform = GetActorTransform();

	// JM : DecalEffect 출력
	{
		FProjectileTableRow* EffectTableRow = DataTableRowHandle.GetRow<FProjectileTableRow>(TEXT("Effect"));
		FEffectDecalTableRow* DecalEffectTableRow = EffectTableRow->EffectTableRowHandle.GetRow<FEffectDecalTableRow>(TEXT("DecalEffect"));

		AEffectWithDecal* DecalEffect = GetWorld()->SpawnActorDeferred<AEffectWithDecal>(DecalEffectTableRow->EffectWithDecalClass,
			FTransform::Identity);

		FDataTableRowHandle DecalTableRowHandle = EffectTableRow->EffectTableRowHandle;

		DecalEffect->SetData(DecalTableRowHandle);
		NewTransform.SetScale3D(DecalEffectTableRow->OverlapParticleTransform.GetScale3D());
		DecalEffect->FinishSpawning(NewTransform);
		// DecalEffect의 위치를 FinishSpawning 함수를 통해 조정하는 것일 뿐,
		// Play 함수를 호출하면 원점에 무조건 DecalEffect가 출력된다.
		DecalEffect->Play();
	}

	ABasicPlayer* OwningCharacter = Cast<ABasicPlayer>(GetOwner());
	check(OwningCharacter);

	// Skill Data Table 얻어오기
	const FSkillTableRow* SkillTableRow = OwningCharacter->GetSkillTableRow();
	ensure(SkillTableRow);

	// Skill Data Table 얻은 후 Projectile은 제거
	Destroy();

	// 데미지 적용
	UGameplayStatics::ApplyDamage(OtherActor, SkillTableRow->SkillDamage, GetInstigator()->GetController(), this, nullptr);
}
