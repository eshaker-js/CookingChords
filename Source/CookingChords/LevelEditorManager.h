// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEditorUI.h"
#include "SliceableObject.h"
#include "ShootableObject.h"
#include "KneadableObject.h"
#include "LevelEditorManager.generated.h"

UCLASS()
class COOKINGCHORDS_API ALevelEditorManager : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ALevelEditorManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<ASliceableObject> SliceableObjectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AShootableObject> ShootableObjectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AKneadableObject> KneadableObjectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<ULevelEditorUI> LevelEditorUIClass;
    
    UPROPERTY()
    ULevelEditorUI* LevelEditorUIPointer;

    UFUNCTION(BlueprintCallable, Category = "Test")
    void SetTestRunning(bool state);

    UFUNCTION(BlueprintCallable, Category = "Test")
    bool GetTestRunning() const;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    // Function to place the player at the Player Start's location and rotation
    void PlacePlayerAtStart();

    // Function to disable VR (if necessary)
    void DisableVR();

    bool TestStarted;

    bool TestRunning;

    void StartTestSequence();

    void EnableVR();

    void SpawnObjectsAtTimeStamp(float Time);

    float StartTime;
    float CurrentPlaybackPosition;

    int level_object_index;

    void SpawnObjectInLane(FVector LaneLocation, TSubclassOf<AActor> ObjectToSpawnClass);
    bool bAudioHasStarted;

};


