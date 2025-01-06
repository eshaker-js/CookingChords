


#include "CircularableObject.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

void ACircularableObject::BeginPlay()
{
    Super::BeginPlay();

    // Ensure the mesh generates overlap events
    if (Mesh)
    {

        bTestedCircle = false;
        Mesh->SetGenerateOverlapEvents(true);

        // Bind overlap events
        Mesh->OnComponentBeginOverlap.AddDynamic(this, &ACircularableObject::OnOverlapBegin);
        Mesh->OnComponentEndOverlap.AddDynamic(this, &ACircularableObject::OnOverlapEnd);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Mesh component is null in CircularMotionObject!"));
    }
}

void ACircularableObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsOverlapping)
    {
        // Increment overlap time
        OverlapTime += DeltaTime;

        // Check if conditions are met for analysis
        if (!bTestedCircle)
        {
            if (TrackedPositions.Num() >= RequiredPoints || OverlapTime >= 0.5f)
            {
                bTestedCircle = true;
                AnalyzeTrackedPositions();
            }
        }
    }
}




void ACircularableObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("OnOverlapBegin triggered."));

    // Ensure we are detecting a valid actor
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    bIsOverlapping = true;

    // Start tracking the motion controller periodically
    GetWorld()->GetTimerManager().SetTimer(QueryTimerHandle, this, &ACircularableObject::QueryMotionController, QueryInterval, true);
}


void ACircularableObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    UE_LOG(LogTemp, Log, TEXT("OnOverlapEnd triggered."));

    // Ensure we are detecting a valid actor
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    bIsOverlapping = false;
    OverlapTime = 0.0f;
    // Stop tracking the motion controller
    GetWorld()->GetTimerManager().ClearTimer(QueryTimerHandle);

    // Clear tracked positions (if used)
    TrackedPositions.Empty();
}

void ACircularableObject::QueryMotionController()
{
    if (!bIsOverlapping)
    {
        return; // Ensure we only track during overlaps
    }

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();
        if (PlayerPawn)
        {
            UMotionControllerComponent* MotionController = PlayerPawn->FindComponentByClass<UMotionControllerComponent>();
            if (MotionController)
            {
                // Track the current position of the motion controller
                FVector CurrentPosition = MotionController->GetComponentLocation();
                if (TrackedPositions.Num() < 200) // Adjust as needed
                {
                    TrackedPositions.Add(CurrentPosition);
                }
            }
        }
    }
}


void ACircularableObject::AnalyzeTrackedPositions()
{
    if (TrackedPositions.Num() < 3)
    {
        return; // Not enough points to analyze
    }

    TArray<float> Radii; // Store computed radii

    while (TrackedPositions.Num() >= 3)
    {
        // Pick 3 unique random indices
        int32 Index1 = FMath::RandRange(0, TrackedPositions.Num() - 1);
        FVector PointA = TrackedPositions[Index1];
        TrackedPositions.RemoveAt(Index1); // Remove the selected position

        int32 Index2 = FMath::RandRange(0, TrackedPositions.Num() - 1);
        FVector PointB = TrackedPositions[Index2];
        TrackedPositions.RemoveAt(Index2); // Remove the selected position

        int32 Index3 = FMath::RandRange(0, TrackedPositions.Num() - 1);
        FVector PointC = TrackedPositions[Index3];
        TrackedPositions.RemoveAt(Index3); // Remove the selected position

        // Calculate the radius of the circle passing through these points
        float Radius = CalculateCircleRadius(PointA, PointB, PointC);
        if (Radius > 0) // Ignore invalid results
        {
            Radii.Add(Radius);
        }
    }

    // Compute variance
    if (Radii.Num() > 0)
    {
        float Variance = ComputeVariance(Radii);
        if (Variance < ThresholdVariance) // ThresholdVariance is a predefined acceptable limit
        {
            SliceObject(); // Perform slicing
        }
    }

    // Clear positions (optional, as they should already be empty)
    TrackedPositions.Empty();
}



float ACircularableObject::CalculateCircleRadius(const FVector& A, const FVector& B, const FVector& C)
{
    FVector AB = B - A;
    FVector AC = C - A;

    FVector CrossProduct = FVector::CrossProduct(AB, AC);
    float Area = CrossProduct.Size() / 2.0f; // Area of the triangle

    float ABLength = AB.Size();
    float ACLength = AC.Size();
    float BCLength = (C - B).Size();

    float SemiPerimeter = (ABLength + ACLength + BCLength) / 2.0f;

    // Avoid division by zero
    if (Area == 0 || SemiPerimeter == 0)
    {
        return -1.0f; // Invalid radius
    }

    float Radius = (ABLength * ACLength * BCLength) / (4.0f * Area);
    return Radius;
}


float ACircularableObject::ComputeVariance(const TArray<float>& Radii)
{
    if (Radii.Num() == 0)
    {
        return 0.0f;
    }

    float Mean = 0.0f;
    for (float Radius : Radii)
    {
        Mean += Radius;
    }
    Mean /= Radii.Num();

    float Variance = 0.0f;
    for (float Radius : Radii)
    {
        Variance += FMath::Square(Radius - Mean);
    }
    Variance /= Radii.Num();

    return Variance;
}
