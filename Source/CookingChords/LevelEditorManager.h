// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEditorUI.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<ULevelEditorUI> LevelEditorUIClass;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    // Function to place the player at the Player Start's location and rotation
    void PlacePlayerAtStart();

    // Function to disable VR (if necessary)
    void DisableVR();
};


