// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Effect/EffectWithDecal.h"
#include "Kismet/GameplayStatics.h"


FEffectDecalTableRow::FEffectDecalTableRow()
	: EffectWithDecalClass(AEffectWithDecal::StaticClass())
{
}

AEffectWithDecal::AEffectWithDecal()	
{
	DecalParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DecalParticleSystemComponent"));
	DecalParticleSystemComponent->SetupAttachment(RootComponent);
	DecalParticleSystemComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DecalParticleSystemComponent->bAutoActivate = false;
}

void AEffectWithDecal::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
	Super::SetData(InDataTableRowHandle);
	
	DataTableRowHandle = InDataTableRowHandle;
	if (DataTableRowHandle.IsNull()) { return; }

	EffectDecalData = static_cast<FEffectDecalTableRow*>(EffectData);

	DecalSound = EffectDecalData->DecalSound;
	DecalVolumeMultiplier = EffectDecalData->DecalVolumeMultiplier;

	DecalParticleSystemComponent->SetRelativeTransform(EffectDecalData->OverlapParticleTransform);
	DecalParticleSystemComponent->SetTemplate(EffectDecalData->ProjectileOverlapParticle);
	SetLifeSpan(0);
	SetLifeSpan(5.f);
}

void AEffectWithDecal::Play()
{
	PlaySound();
	PlayParticle();
}

void AEffectWithDecal::PlaySound()
{	
	const FVector Location = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DecalSound, Location, DecalVolumeMultiplier);
}

void AEffectWithDecal::PlayParticle()
{
	DecalParticleSystemComponent->ActivateSystem(true);
}

