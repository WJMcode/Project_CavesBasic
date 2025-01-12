// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Player/Input/BasicPlayerController.h"
#include "Actors/Player/BasicPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameframeWork/PawnMovementComponent.h"

ABasicPlayerController::ABasicPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> Asset(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Actors/Player/Input/IMC_Player.IMC_Player'"));
	check(Asset.Object);
	
	IMC_Stage = Asset.Object;
	
}

void ABasicPlayerController::BeginPlay()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	Subsystem->AddMappingContext(IMC_Stage, 0);

	// 게임 시작 시 마우스 커서를 보이게 설정
	bShowMouseCursor = true;          // 마우스 커서 보이기
	
	// UI 모드와 게임 모드 활성화
	// UI 모드로 설정하여 마우스를 UI 요소와 상호작용할 수 있도록 만듦.
	SetInputMode(FInputModeGameAndUI());

	// 게임 화면의 크기 가져와서 게임창 크기의 변경이 감지되면 
	// 마우스 커서를 중앙으로 위치시키기. 
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient)
	{
		FViewport* Viewport = ViewportClient->Viewport;
		if (Viewport)
		{
			// FViewport::ViewportResizedEvent 델리게이트에 함수 바인딩
			// 창 크기의 변경을 감지할 수 있도록 ViewportResizedEvent를 구독
			// OnViewportResized 함수를 호출하여 마우스 커서를 화면 중앙으로 위치시킨다.
			Viewport->ViewportResizedEvent.AddUObject(this, &ABasicPlayerController::OnViewportResized);

			// 게임 시작 시에도 일단 마우스를 중앙으로 이동시킨다.
			// SetTimer를 통해 약간의 지연을 주어야 정상적으로 마우스가 중앙으로 이동함.
			// ABasicPlayerController의 BeginPlay 함수가 막 호출되는 시점에는
			// 게임창의 크기가 제대로 설정되지 않았기 때문으로 추측됨.
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, 
				[this, Viewport]()
				{
					OnViewportResized(Viewport, 0);
				}
				, 0.2f, false);
		}
	}
}

void ABasicPlayerController::OnViewportResized(FViewport* InViewPort, uint32 Unknown)
{
	FIntPoint ViewportSize = InViewPort->GetSizeXY();

	SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
}

void ABasicPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	ensure(EnhancedInputComponent);

	if (const UInputAction* InputAction = FUtils::GetInputActionFromName(IMC_Stage, TEXT("IA_Move")))
	{
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnMove);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IA_Move is disabled"));
	}

	if (const UInputAction* InputAction = FUtils::GetInputActionFromName(IMC_Stage, TEXT("IA_Jump")))
	{
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnJump);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IA_Jump is disabled"));
	}

	if (const UInputAction* InputAction = FUtils::GetInputActionFromName(IMC_Stage, TEXT("IA_Crouch")))
	{
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnCrouch);
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &ThisClass::OnUnCrouch);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IA_Crouch is disabled"));
	}
}

void ABasicPlayerController::OnMove(const FInputActionValue& InputActionValue)
{
	const FVector2D ActionValue = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = K2_GetActorRotation();
	const FRotator RotationYaw = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(RotationYaw);
	const FVector RightVector = UKismetMathLibrary::GetRightVector(RotationYaw);

	APawn* ControlledPawn = GetPawn();
	ControlledPawn->AddMovementInput(ForwardVector, ActionValue.X);
	ControlledPawn->AddMovementInput(RightVector, ActionValue.Y);
}

void ABasicPlayerController::OnJump(const FInputActionValue& InputActionValue)
{
	ABasicPlayer* ControlledCharacter = Cast<ABasicPlayer>(GetPawn());
	if (ControlledCharacter->GetStatusComponent()->IsDie()) { return; }
	ControlledCharacter->Jump();
}

void ABasicPlayerController::OnCrouch(const FInputActionValue& InputActionValue)
{
	ABasicPlayer* ControlledCharacter = Cast<ABasicPlayer>(GetPawn());
	if (ControlledCharacter->GetStatusComponent()->IsDie() || ControlledCharacter->GetMovementComponent()->IsFalling()) { return; }
	ControlledCharacter->Crouch();
}

void ABasicPlayerController::OnUnCrouch(const FInputActionValue& InputActionValue)
{	
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
	ControlledCharacter->UnCrouch();
}

