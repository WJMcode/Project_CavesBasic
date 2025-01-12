// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/BasicHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Player/Input/BasicPlayerController.h"
#include "Components/StatusComponent.h"

ABasicHUD::ABasicHUD()
{
	{
		static ConstructorHelpers::FObjectFinder<UInputMappingContext> Asset
		{ TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Actors/Player/Input/IMC_Item.IMC_Item'") };
		check(Asset.Object);

		IMC_Item = Asset.Object;
	}
}

void ABasicHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerController = Cast<ABasicPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	check(PlayerController);
}

void ABasicHUD::OnPlayerDeath()
{
	Widget->HandlePlayerDeath();
}

void ABasicHUD::ResetUI(APawn* InOwningPawn)
{
	Widget->InitializeWidgets(InOwningPawn);
	Widget->RefreshWidgetsForRespawn();
}

void ABasicHUD::AddDroppedItem(ADroppedItem* NewItem)
{
	Widget->AddDroppedItem(NewItem);
}

void ABasicHUD::RemoveDroppedItem(ADroppedItem* NewItem)
{
	Widget->RemoveDroppedItem(NewItem);
}

void ABasicHUD::OnPickupItem(const FInputActionValue& InputActionValue)
{
	if (OverlappedDroppedItems.IsEmpty()) { return; }

	ADroppedItem* DroppedItem = static_cast<ADroppedItem*>(*OverlappedDroppedItems.begin());
	DroppedItem->EquipItem(PlayerController);
	DroppedItem->Destroy();
}

void ABasicHUD::BeginPlay()
{
	Super::BeginPlay();
	
	AddRespawnHUDDelegate();

	// JM
	// 블루프린트에서 UBasicHUDWidget 클래스로 만든 위젯 안에는 
	// UHPBarWidgetBase와 URespawnPanelWidget가 있음
	
	// C++에서 UBasicHUDWidget 클래스 안에는
	// UHPBarWidgetBase 와 URespawnPanelWidget 클래스를 가리키는 포인터를 멤버로 들고 있다.
	// 해당 멤버들은 블루프린트에 있는 위젯을 바인딩하고 있다.


	UClass* WidgetClass = LoadClass<UBasicHUDWidget>(nullptr,
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_CavesBasic.UI_CavesBasic_C'"));
	
	if (!WidgetClass)
	{
		ensure(WidgetClass);
		UE_LOG(LogTemp, Error, TEXT("Failed to load WidgetClass!"));
		return;
	}
	
	Widget = CreateWidget<UBasicHUDWidget>(GetWorld(), WidgetClass);

	// UBasicHUDWidget가 멤버로 들고 있는 위젯들의 초기화 작업 진행
	Widget->InitializeWidgets(GetOwningPawn());
	
	// PIE 모드 외 다른 모드에서 실행할 때에는
	// PlayerController가 아직 없는 상태에서 아래 코드에 돌입할 수 있음.
	// 그런 경우 Crash가 발생하므로 PlayerController이 존재하는 경우 아래 코드를 실행하도록 함.
	if (IsValid(PlayerController))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		Subsystem->AddMappingContext(IMC_Item, 0);
		EnableInput(PlayerController);
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		ensure(EnhancedInputComponent);
		if (const UInputAction* InputAction = FUtils::GetInputActionFromName(IMC_Item, TEXT("IA_PickupItem")))
		{
			EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &ThisClass::OnPickupItem);
		}
		// UBasicHUDWidget의 Owner 세팅
		Widget->SetOwningPlayer(PlayerController);
	}
	Widget->AddToViewport();

}

void ABasicHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!PlayerController) { return; }
	
	if (APawn* Pawn = PlayerController->GetPawn())
	{
		TArray<AActor*> Temp;
		Pawn->GetOverlappingActors(Temp, ADroppedItem::StaticClass());

		if (Temp != OverlappedDroppedItems)
		{
			for (AActor* It : OverlappedDroppedItems)
			{
				RemoveDroppedItem(static_cast<ADroppedItem*>(It));
			}
			OverlappedDroppedItems = MoveTemp(Temp);

			for (AActor* It : OverlappedDroppedItems)
			{
				AddDroppedItem(static_cast<ADroppedItem*>(It));
			}
		}
	}
}

void ABasicHUD::RemoveHUDDelegate()
{
	APawn* OwningPawn = GetOwningPawn();

	if (OwningPawn)
	{
		UStatusComponent* StatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		check(StatusComponent);
		StatusComponent->OnDie.RemoveDynamic(this, &ThisClass::OnPlayerDeath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABasicHUD::AddRespawnHUDDelegate() : OwningPawn is nullptr"))
	}
}

void ABasicHUD::AddRespawnHUDDelegate()
{
	APawn* OwningPawn = GetOwningPawn();
	
	if (OwningPawn)
	{
		UStatusComponent* StatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		check(StatusComponent);
		StatusComponent->OnDie.AddDynamic(this, &ThisClass::OnPlayerDeath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABasicHUD::AddRespawnHUDDelegate() : OwningPawn is nullptr"))
	}
}
