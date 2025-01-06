

#pragma once

#include "CoreMinimal.h"
#include "SliceableObject.h"
#include "CircularableObject.generated.h"

UCLASS()
class COOKINGCHORDS_API ACircularableObject : public ASliceableObject
{
    GENERATED_BODY()

private:
    // Timer handle for periodic motion tracking
    FTimerHandle QueryTimerHandle;

    // Interval in seconds for motion tracking
    float QueryInterval = 0.01f;

    float ThresholdVariance = 1.0f;

    bool bTestedCircle;

    int RequiredPoints = 100;

    float OverlapTime = 0.0f;

    // Boolean to track if player is overlapping
    bool bIsOverlapping = false;

    // Array to store motion positions
    TArray<FVector> TrackedPositions;

    void QueryMotionController();

    void AnalyzeTrackedPositions();

    float CalculateCircleRadius(const FVector& A, const FVector& B, const FVector& C);

    float ComputeVariance(const TArray<float>& Radii);

protected:
    // Called when the game starts or the object is spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
