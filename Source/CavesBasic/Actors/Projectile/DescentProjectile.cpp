// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile/DescentProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Effect/EffectWithDecal.h"

void ADescentProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FVector Location = GetActorLocation();
	if (!IsValid(this)) { return; }

	// BeginPlay 시점에 Overlapped 되면 들어 옴
	//if (!bFromSweep)
	//{
	//	Destroy();
	//	check(false);
	//	return;
	//}

	FHitResult HitResult = SweepResult;

	// JM
	// Projectile이 정상적인 위치에서 Spawn되지 않은 상태에서 Overlap될 때 
	// SweepResult.ImpactPoint가 0이다.
	if (SweepResult.ImpactPoint.IsNearlyZero())
	{
		// JM@TODO : DecalEffect가 정상적인 위치에서 Spawn될 수 있도록 위치를 조정
		const FVector InstigatorForwardVector = GetInstigator()->GetActorForwardVector();
		const FVector DestinationLocation = GetActorLocation() + InstigatorForwardVector * (ProjectileMovementComponent->MaxSpeed * InitialLifeSpan);

		TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());

		// Projectile의 크기를 얻어오는 코드
		FBoxSphereBounds ProjectileBounds = StaticMeshComponent->GetStaticMesh()->GetBounds();
		FVector ProjectileBoxExtent = ProjectileBounds.BoxExtent; // 바운딩 박스의 반경
		// 전체 사이즈는 Projectile 데이터 테이블에 설정된 Scale값을 반영한다.
		FVector ProjectileSize = ProjectileBoxExtent * ProjectileData->Transform.GetScale3D();

		const FVector InstigatorLocation = GetInstigator()->GetActorLocation();

		// 캐릭터를 기준으로 BoxTrace를 발사하여 
		// 가장 가까운 위치에 존재하는 오브젝트와 충돌하는 순간의 위치를 얻어옴
		const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4);
		const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(),
			GetInstigator()->GetActorLocation(), DestinationLocation, ProjectileSize, FRotator::ZeroRotator, TraceTypeQuery,
			false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);

		HitResult.ImpactPoint.X = HitResult.ImpactPoint.X - InstigatorLocation.X;
	}
	else
	{
		HitResult.ImpactPoint.X = Location.X;
	}
	// DecalEffect 위치 세팅 
	// JM@TODO : DecalEffect의 위치가 Projectile 위치와 정확히 일치하지 않는 문제
	// 캐릭터가 월드상에서 y축으로 멀리 간 상태에서 스킬을 사용하면 DecalEffect의 위치가 그만큼 따라오지 못함
	FTransform NewTransform;
	HitResult.ImpactPoint.Y = Location.Y;
	HitResult.ImpactPoint.Z = 0.0;
	NewTransform.SetLocation(HitResult.ImpactPoint);
	/*FRotator Rotation = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
	NewTransform.SetRotation(Rotation.Quaternion());*/

	//// DecalEffect 위치 세팅
	//FTransform NewTransform;
	//NewTransform.SetLocation(HitResult.ImpactPoint);
	//FRotator Rotation = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
	//NewTransform.SetRotation(Rotation.Quaternion());

	// JM@TODO : DecalEffect 출력
	{
		FProjectileTableRow* EffectTableRow = DataTableRowHandle.GetRow<FProjectileTableRow>(TEXT("Effect"));
		FEffectDecalTableRow* DecalEffectTableRow = EffectTableRow->EffectTableRowHandle.GetRow<FEffectDecalTableRow>(TEXT("DecalEffect"));

		AEffectWithDecal* DecalEffect = GetWorld()->SpawnActorDeferred<AEffectWithDecal>(DecalEffectTableRow->EffectWithDecalClass,
			FTransform::Identity);

		FDataTableRowHandle DecalTableRowHandle = EffectTableRow->EffectTableRowHandle;

		DecalEffect->SetData(DecalTableRowHandle);
		// DecalEffect가 오브젝트에 너무 붙어 있으면
		// 깜빡거리기 때문에 살짝 위로 올려줌
		FVector NewZAxis = NewTransform.GetTranslation();
		NewZAxis.Z = NewZAxis.Z + 1.0;
		NewTransform.SetTranslation(NewZAxis);
		DecalEffect->FinishSpawning(NewTransform);
		// DecalEffect의 위치를 FinishSpawning 함수를 통해 조정하는 것일 뿐,
		// Play 함수를 호출하면 원점에 무조건 DecalEffect가 출력된다.
		DecalEffect->Play();
	}

	Destroy();

	UGameplayStatics::ApplyDamage(OtherActor, 1.f, GetInstigator()->GetController(), this, nullptr);
}
