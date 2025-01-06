// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"
#include "Engine/StaticMeshActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogShootableObject, Log, All);

// Sets default values
AShootableObject::AShootableObject()
{
	bMaterialChanged = false;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Mesh->SetGenerateOverlapEvents(true);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AShootableObject::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AShootableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShootableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShootableObject::OnOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogShootableObject, Warning, TEXT("Object Hit Detected! Attempting to change material"));
	if (OtherActor && OtherComp && !bMaterialChanged) // Ensure the other actor and component are valid
	{
		UE_LOG(LogShootableObject, Warning, TEXT("Other Actor is valid"));
		UStaticMeshComponent* OtherMesh = OtherActor->FindComponentByClass<UStaticMeshComponent>();
		if (OtherMesh) // Ensure the cast was successful
		{
			UE_LOG(LogShootableObject, Warning, TEXT("Other Mesh is valid"));
			UMaterialInterface* OtherMaterial = OtherMesh->GetMaterial(0); // Get the first material of the other mesh
			if (OtherMaterial) // Ensure the material is valid
			{
				UE_LOG(LogShootableObject, Warning, TEXT("Object Hit Detected! Attempting to change material"));
				Mesh->SetMaterial(0, OtherMaterial); // Set the first material of this object's mesh to match
				bMaterialChanged = true;
			}
		}
	}
}