// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Effect/Effect.h"
#include "Kismet/GameplayStatics.h"
#include "ParticleEmitterInstances.h"

FEffectTableRow::FEffectTableRow()
	: EffectClass(AEffect::StaticClass())
{
}


// Sets default values
AEffect::AEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	
	SpawnParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnParticleSystemComponent"));
	SpawnParticleSystemComponent->SetupAttachment(DefaultSceneRoot);
	SpawnParticleSystemComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnParticleSystemComponent->bAutoActivate = false;
	RootComponent = DefaultSceneRoot;
}

void AEffect::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
	DataTableRowHandle = InDataTableRowHandle;
	if (DataTableRowHandle.IsNull()) { return; }
	FEffectTableRow* Data = DataTableRowHandle.GetRow<FEffectTableRow>(TEXT("Effect"));
	if (!Data) { ensure(false); return; }

	EffectData = Data;

	Sound = Data->Sound;
	VolumeMultiplier = Data->VolumeMultiplier;

	SpawnParticleSystemComponent->SetTemplate(Data->ProjectileSpawnParticle);
	SpawnParticleSystemComponent->SetRelativeTransform(Data->SpawnParticleTransform); // 파티클 시스템 컴포넌트에 변환 적용

	SetLifeSpan(0);
	SetLifeSpan(5.f);

}

void AEffect::Play()
{
	PlaySound();
	PlayParticle();
}

// Called when the game starts or when spawned
void AEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEffect::PlaySound()
{
	const FVector Location = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, VolumeMultiplier);
}

void AEffect::PlayParticle()
{
	SpawnParticleSystemComponent->ActivateSystem(true);
}

// Called every frame
void AEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

