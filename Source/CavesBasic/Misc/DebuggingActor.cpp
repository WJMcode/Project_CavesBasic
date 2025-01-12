// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/DebuggingActor.h"

// Sets default values
ADebuggingActor::ADebuggingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    RootComponent = CubeMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Demo/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'"));
    if (CubeMeshAsset.Succeeded())
    {
        CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
        CubeMesh->SetWorldScale3D(FVector(1.0f)); // 큐브 크기 조정
        CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    SetLifeSpan(10.f);
}

// Called when the game starts or when spawned
void ADebuggingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADebuggingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

