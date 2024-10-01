// Fill out your copyright notice in the Description page of Project Settings.


#include "SliceableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"
#include "Engine/StaticMeshActor.h"


DEFINE_LOG_CATEGORY_STATIC(LogSliceableObject, Log, All);


// Sets default values
ASliceableObject::ASliceableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

    Mesh->SetSimulatePhysics(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetEnableGravity(false);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ASliceableObject::OnOverlapBegin);
    is_sliced = false;
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


void ASliceableObject::OnOverlapBegin(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogSliceableObject, Warning, TEXT("Object Hit Detected!"));

	//Destroy();
	SliceObject();
}


void ASliceableObject::SliceObject()
{
    // Store necessary information before destroying the original actor
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();
    UStaticMesh* OriginalMesh = Mesh->GetStaticMesh();

    is_sliced = true;

    OnSliced();
    // Destroy the original object before proceeding
    Destroy();

    UE_LOG(LogSliceableObject, Warning, TEXT("Slicing object"));

    // Offset for the spawn locations of the two halves
    FVector Offset = FVector(10.0f, 0.0f, 0.0f);  // Adjust the offset as needed

    // Spawn the first half slightly to the right
    FVector FirstHalfLocation = SpawnLocation + Offset;
    AStaticMeshActor* FirstHalf = GetWorld()->SpawnActor<AStaticMeshActor>(FirstHalfLocation, SpawnRotation);
    if (FirstHalf && FirstHalf->GetStaticMeshComponent())
    {
        FirstHalf->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        FirstHalf->GetStaticMeshComponent()->SetStaticMesh(OriginalMesh);  
        FirstHalf->GetStaticMeshComponent()->SetSimulatePhysics(true);
        FirstHalf->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        FVector FirstHalfDirection = FVector::UpVector + FVector::RightVector;
        FirstHalf->GetStaticMeshComponent()->AddImpulse(FirstHalfDirection * 500.0f);
    }

    // Spawn the second half slightly to the left
    FVector SecondHalfLocation = SpawnLocation - Offset;
    AStaticMeshActor* SecondHalf = GetWorld()->SpawnActor<AStaticMeshActor>(SecondHalfLocation, SpawnRotation);
    if (SecondHalf && SecondHalf->GetStaticMeshComponent())
    {
        SecondHalf->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        SecondHalf->GetStaticMeshComponent()->SetStaticMesh(OriginalMesh);  
        SecondHalf->GetStaticMeshComponent()->SetSimulatePhysics(true);
        SecondHalf->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        FVector SecondHalfDirection = FVector::UpVector + FVector::LeftVector;
        SecondHalf->GetStaticMeshComponent()->AddImpulse(SecondHalfDirection * 500.0f);
    }
    FTimerHandle timer1;
    GetWorld()->GetTimerManager().SetTimer(timer1, [FirstHalf, SecondHalf]() {
        if (FirstHalf) FirstHalf->Destroy();
        if (SecondHalf) SecondHalf->Destroy();
        }, 1.0f, false);
    
}


bool ASliceableObject::GetIsSliced() const
{
    return is_sliced;
}

void ASliceableObject::SetIsSliced(bool new_state)
{
    if (is_sliced != new_state)
    {
        is_sliced = new_state;
        //UE_LOG(LogTemp, Warning, TEXT("bIsSliced has been updated."));

        // Additional logic can go here if needed, like triggering events
    }
}
