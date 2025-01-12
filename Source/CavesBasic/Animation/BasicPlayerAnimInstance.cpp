// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/BasicPlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "Actors/Player/BasicPlayer.h"

void UBasicPlayerAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
	
	APawn* OwnerPawn = TryGetPawnOwner();
	
	if (GIsEditor && FApp::IsGame() && !OwnerPawn)
	{
		return;
	}
	else if (!OwnerPawn) { return; }
	else if (ABasicPlayer* OwnerPlayer = Cast<ABasicPlayer>(OwnerPawn))
	{
		// 플레이어가 사망한 상태에서도 NativeUninitializeAnimation은 호출이 되기 때문에 문제가 발생할 수 있음.
		// 사망한 상태라면 UBasicPlayerAnimInstance의 멤버 MovementComponent를 초기화하지 않고 return한다.
		if (OwnerPlayer && OwnerPlayer->GetStatusComponent()->IsDie()) { return; }
	}
	
	// JM@TODO : 에디터 화면을 보고 있지 않은 상태에서 캐릭터가 사망하면
	// RespawnPanel은 뜨지만, 플레이어의 사망 모션이 나오자 마자 다시 일어남
	// 이 과정에서 문제가 발생하는 듯
	MovementComponent = OwnerPawn->GetMovementComponent();
	check(MovementComponent);
}

void UBasicPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!MovementComponent)
	{
		return;
	}
	
	Speed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);

	// 애니메이션 블루 프린트에서 사용할 변수 업데이트
	bShouldMove = !FMath::IsNearlyZero(Speed);
	bIsFalling = MovementComponent->IsFalling();
	bIsCrouch = MovementComponent->IsCrouching();
}
