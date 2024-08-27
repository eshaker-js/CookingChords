// Fill out your copyright notice in the Description page of Project Settings.


#include "SliceableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"


DEFINE_LOG_CATEGORY_STATIC(LogSliceableObject, Log, All);


// Sets default values
ASliceableObject::ASliceableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->OnComponentHit.AddDynamic(this, &ASliceableObject::OnHit);
}

// Called when the game starts or when spawned
void ASliceableObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASliceableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASliceableObject::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogSliceableObject, Warning, TEXT("Object Hit Detected!"));

	// Despawn logic will go here in the next step
}
