// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Effect/CharacterMeshEffect/CharacterMeshEffect.h"
#include "Actors/Player/BasicPlayer.h"

void UCharacterMeshEffect::ApplyHitMaterial(const float Duration)
{
    if (!OwningPlayer || !TargetMeshComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("OwningPlayer or MeshComponent is nullptr."));
        return;
    }

    // 1. Overlay Material을 가져오기
    OriginalOverlayMaterial = TargetMeshComponent->GetOverlayMaterial();

    if (!OriginalOverlayMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("OriginalOverlayMaterial is nullptr, 플레이어 Mesh에는 오버레이 머티리얼이 존재해야 합니다."));
        return;
    }
    
    // 2. Overlay Material을 동적 머티리얼 인스턴스로 변환
    UMaterialInstanceDynamic* DynOverlayMaterial = UMaterialInstanceDynamic::Create(OriginalOverlayMaterial, this);

    if (DynOverlayMaterial)
    {
        DynOverlayMaterial->SetScalarParameterValue("HitOverlayOpacity", 0.6f);
        TargetMeshComponent->SetOverlayMaterial(DynOverlayMaterial);

        // BlinkTimerHandle가 작동하고 있지 않을 때에만 아래 코드 실행
        if (!GetWorld()->GetTimerManager().IsTimerActive(BlinkTimerHandle))
        {
            // (Duration / x.f)초마다 BlinkMaterial 함수를 호출함
            GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, [this, DynOverlayMaterial]()
                {
                    BlinkMaterial(DynOverlayMaterial);
                }, Duration / 30.f, true);
        }

        // RestoreTimerHandle가 작동하고 있지 않을 때에만 아래 코드 실행
        if (!GetWorld()->GetTimerManager().IsTimerActive(RestoreTimerHandle))
        {
            // OwningPlayer가 사망 상태라면 덜 깜빡임
            if (OwningPlayer && OwningPlayer->GetStatusComponent()->IsDie())
            {
                // Duration / x초 후에 타이머를 멈추도록 설정
                GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, [this, DynOverlayMaterial]()
                    {
                        RestoreOriginalMaterial(DynOverlayMaterial);

                        // 타이머 정지
                        GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
                        GetWorld()->GetTimerManager().ClearTimer(RestoreTimerHandle);

                        InitializeMembers();

                    }, Duration / 3.f , false);
            }
            else
            {
                // Duration초 후에 타이머를 멈추도록 설정
                GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, [this, DynOverlayMaterial]()
                    {
                        RestoreOriginalMaterial(DynOverlayMaterial);

                        // 타이머 정지
                        GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
                        GetWorld()->GetTimerManager().ClearTimer(RestoreTimerHandle);

                        InitializeMembers();

                    }, Duration, false);
            }
        }
    }
}

void UCharacterMeshEffect::BlinkMaterial(UMaterialInstanceDynamic* OutDynOverlayMaterial)
{
    if (BlinkCount % 2 == 0)
    {
        // HitOverlayOpacity값을 0.0f로 설정, 기본 상태랑 같음
        OutDynOverlayMaterial->SetScalarParameterValue("HitOverlayOpacity", 0.0f);
    }
    else
    {
        // HitOverlayOpacity값을 0.6f로 설정, OverlayMaterial이 캐릭터에게 반투명 형태로 나타남
        OutDynOverlayMaterial->SetScalarParameterValue("HitOverlayOpacity", 0.6f);
    }

    // 파라미터 변경 후 Overlay Material 재설정
    TargetMeshComponent->SetOverlayMaterial(OutDynOverlayMaterial);

    // BlinkCount 증가
    ++BlinkCount;
}

void UCharacterMeshEffect::RestoreOriginalMaterial(UMaterialInstanceDynamic* OutDynOverlayMaterial)
{
    TargetMeshComponent->SetOverlayMaterial(OriginalOverlayMaterial);
}

void UCharacterMeshEffect::InitializeMembers()
{

    OriginalOverlayMaterial = nullptr;

    BlinkCount = 0;
    BlinkTimerHandle.Invalidate();
    RestoreTimerHandle.Invalidate(); 
}