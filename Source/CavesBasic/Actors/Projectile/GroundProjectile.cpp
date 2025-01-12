// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile/GroundProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Effect/EffectWithDecal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Misc/DebuggingActor.h"
#include "Misc/Utils.h"
#include "Actors/Player/BasicPlayer.h"
#include "Data/SkillData.h"

AGroundProjectile::AGroundProjectile()
{
	StaticMeshComponent->SetCollisionProfileName(CollisionProfileName::GroundProjectile);
}

void AGroundProjectile::SetAdjustLocation(FTransform& OutTransform)
{
	// GroundProjectile은 캐릭터를 기준으로 Spawn되게 할 것이므로 이 함수에서 위치를 조정해줌	

	ACharacter* OwningActor = Cast<ACharacter>(GetOwner());
	UCharacterMovementComponent* OwningMovementComponent = OwningActor->GetComponentByClass<UCharacterMovementComponent>();

	// GroundProjectile은 캐릭터를 기준으로 Spawn되므로 캐릭터가 갖고 있는 CharacterCenter 소켓을 가져옴
	FVector GroundProjectileStartLocation = FVector::ZeroVector;
	USkeletalMeshComponent* CharacterSkeletalMeshComponent = OwningActor->GetComponentByClass<USkeletalMeshComponent>();
	check(CharacterSkeletalMeshComponent);
	if (CharacterSkeletalMeshComponent->GetSkeletalMeshAsset())
	{
		GroundProjectileStartLocation = CharacterSkeletalMeshComponent->GetSocketLocation(SocketName::CharacterCenter);
#if WITH_EDITOR
		USkeletalMeshSocket const* SkeletalMeshSocket = CharacterSkeletalMeshComponent->GetSocketByName(SocketName::CharacterCenter);
		check(SkeletalMeshSocket);
#endif
	}

	FRotator GroundProjectileRotator = OwningActor->GetActorRotation();

	FTransform NewTransform;
	GroundProjectileStartLocation.Z = OwningMovementComponent->GetActorFeetLocation().Z;
	NewTransform.SetLocation(GroundProjectileStartLocation);
	NewTransform.SetRotation(GroundProjectileRotator.Quaternion());

	OutTransform = NewTransform;
}

void AGroundProjectile::BeginPlay()
{
	Super::BeginPlay();

	// GroundProjectile은 생성 시 
	// GroundProjectile의 위와 아래에 오브젝트가 
	// 존재하는지 감지하고 감지된 지점에 옮겨지도록 한다.
	// 위와 아래 모두 오브젝트가 감지된 경우 아래쪽을 우선으로 하여
	// 아래쪽으로 GroundProjectile을 이동시킨다.

	// GroundProjectile의 위치 얻어오기
	FVector GroundProjectileLocation = GetActorLocation();

	FHitResult DownHitResult;
	{
		TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());

		// GroundProjectile을 기준으로 LineTrace를 발사하여 
		// 위쪽 기준 가장 가까운 위치에 존재하는 오브젝트와 충돌하는 순간의 위치를 얻어옴
		// ECollisionChannel::ECC_GameTraceChannel5 은 FloorDetectTraceChannel 이다.
		// 즉 해당 Trace는 FloorDetectTraceChannel로 발사되는 Trace이다. 
		// 발사된 해당 Trace는 Collision이 Floor로 설정된 오브젝트를 감지한다.
		// Floor로 설정된 오브젝트에만 GroundProjectile 스킬을 스폰시키는 것이 목적.
		const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5);
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
			GetActorLocation(), GetActorLocation() + FVector(0, 0, -350), TraceTypeQuery,
			false, IgnoreActors, EDrawDebugTrace::ForDuration, DownHitResult, true);
		// 만약 Hit가 발생했다면 그 위치로 GroundProjectile을 옮김
		if (bHit)
		{
			GroundProjectileLocation.Z = DownHitResult.ImpactPoint.Z;
			SetActorLocation(GroundProjectileLocation);
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("finalProjectileLocation: X: %.2f      Y : %.2f     Z : %.2f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z), true, true, FLinearColor::Green, 5.f);

			return;
		}
	}

	FHitResult UpHitResult;
	{
		TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());

		// GroundProjectile을 기준으로 LineTrace를 발사하여 
		// 위쪽 기준 가장 가까운 위치에 존재하는 오브젝트와 충돌하는 순간의 위치를 얻어옴
		// ECollisionChannel::ECC_GameTraceChannel5 은 FloorDetectTraceChannel 이다.
		// 즉 해당 Trace는 FloorDetectTraceChannel로 발사되는 Trace이다. 
		// 발사된 해당 Trace는 Collision이 Floor로 설정된 오브젝트를 감지한다.
		// Floor로 설정된 오브젝트에만 GroundProjectile 스킬을 스폰시키는 것이 목적.
		const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5);
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
			GetActorLocation(), GetActorLocation() + FVector(0, 0, 200), TraceTypeQuery,
			false, IgnoreActors, EDrawDebugTrace::ForDuration, UpHitResult, true);

		if (bHit)
		{
			GroundProjectileLocation.Z = UpHitResult.ImpactPoint.Z;
			SetActorLocation(GroundProjectileLocation);
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("finalProjectileLocation: X: %.2f      Y : %.2f     Z : %.2f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z), true, true, FLinearColor::Green, 5.f);

			return;
		}
	}
	
	// Floor가 감지되지 않으면 GroundProjectile을 그냥 제거한다.
	Destroy();
}

AActor* AGroundProjectile::DetectDamageTarget()
{
	FHitResult DetectResult;
	{
		TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());

		FVector Origin;
		FVector BoxExtent;
		GetActorBounds(false, Origin, BoxExtent);

		// Projectile을 기준으로 BoxTrace를 발사하여 
		// 위쪽 기준 가장 가까운 위치에 존재하는 오브젝트와 충돌하는 순간의 위치를 얻어옴
		// ECollisionChannel::ECC_GameTraceChannel8 은 MonsterDetectTraceChannel 이다.
		// 즉 해당 Trace는 MonsterDetectTraceChannel로 발사되는 Trace이다. 
		// 발사된 해당 Trace는 Collision이 Monster로 설정된 오브젝트가 감지하여 Block 처리를 한다.
		// GroundProjectile 스킬을 사용했을 때 Monster로 설정된 오브젝트를 감지하여 데미지를 주는 것이 목적.
		const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel8);
		const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(),
			GetActorLocation(), GetActorLocation() + FVector(0, 0, 5), BoxExtent, FRotator::ZeroRotator, TraceTypeQuery,
			false, IgnoreActors, EDrawDebugTrace::ForDuration, DetectResult, true);

		if (bHit)
		{
			AActor* DetectActor = DetectResult.GetActor();
			if (DetectActor)
			{
				return DetectActor;
			}
		}
	}

	return nullptr;
}

void AGroundProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(this)) { return; }
	
	// BeginPlay 시점에 Overlapped 되면 들어 옴,
	// 즉 GroundProjectile이 생성되자마자 오버랩된 상황
	//if (!bFromSweep)
	//{
	//	Destroy();
	//	return;
	//}

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

	// GroundProjectile이 Floor와 Overlap되면 
	// 그곳에서 데미지를 줄 타겟을 찾는다. 
	AActor* DetectActor = DetectDamageTarget();

	Destroy();

	// 타겟이 감지되었다면 데미지 주기
	if(DetectActor)
	{
		UGameplayStatics::ApplyDamage(DetectActor, SkillTableRow->SkillDamage, GetInstigator()->GetController(), this, nullptr);
	}
}
