#include "LevelEditorManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/Engine.h"

// Sets default values
ALevelEditorManager::ALevelEditorManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevelEditorManager::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World context is invalid during level transition"));
        return;
    }

    // Ensure player controller is valid
    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null"));
        return;
    }

    // Ensure player pawn is valid
    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerPawn is null"));
        return;
    }

    // Ensure Player Start is valid
    AActor* PlayerStart = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass());
    if (!PlayerStart)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerStart is null"));
        return;
    }


    // Disable VR if necessary
    DisableVR();

    // Place player at Player Start and set rotation
    PlacePlayerAtStart();

        PlayerController->bShowMouseCursor = true;  // Show the mouse cursor
        PlayerController->bEnableClickEvents = true;  // Enable click events
        PlayerController->bEnableMouseOverEvents = true;  // Enable mouse over events
        PlayerController->SetInputMode(FInputModeGameAndUI());  // Set input mode to handle both game and UI
}

// Called every frame
void ALevelEditorManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ALevelEditorManager::PlacePlayerAtStart()
{
    // Get the player controller and player pawn
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();
        if (PlayerPawn)
        {
            // Find the Player Start actor in the level
            AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
            if (PlayerStart)
            {
                // Set the player's location and rotation based on Player Start
                FVector StartLocation = PlayerStart->GetActorLocation();
                FRotator StartRotation = PlayerStart->GetActorRotation();

                PlayerPawn->SetActorLocation(StartLocation);
                PlayerPawn->SetActorRotation(StartRotation);

                // Optionally, disable player movement (lock the player in place)
                PlayerController->DisableInput(nullptr);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PlayerStart is null!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerPawn is null!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null!"));
    }
}


void ALevelEditorManager::DisableVR()
{
    if (GEngine && GEngine->XRSystem.IsValid())
    {
        // Disable VR rendering (turn off stereo view)
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            // This console command disables the stereo VR mode
            PlayerController->ConsoleCommand("vr.bEnableStereo 0");
        }
    }
}


