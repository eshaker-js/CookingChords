#include "LevelEditorManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/Engine.h"
#include "SliceableObject.h"
#include "MotionControllerComponent.h"
#define FirstLane FVector(-355.0, -1085.750609, 283.633865)
#define FourthLane FVector(-50.0, -1085.750609, 83.633865)


// Sets default values
ALevelEditorManager::ALevelEditorManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevelEditorManager::BeginPlay()
{
    Super::BeginPlay();

    bAudioHasStarted = false;
    TestRunning = false;
    TestStarted = false;
    level_object_index = 0;
    UWorld* World = this->GetWorld();  // Use `this->GetWorld()` for clarity
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

    // Enable mouse controls for the UI and game
    PlayerController->bShowMouseCursor = true;
    PlayerController->bEnableClickEvents = true;
    PlayerController->bEnableMouseOverEvents = true;
    PlayerController->SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));

    // Ensure the widget blueprint class is set
    if (LevelEditorUIClass)
    {
        ULevelEditorUI* AudioUI = CreateWidget<ULevelEditorUI>(PlayerController, LevelEditorUIClass);
        if (AudioUI)
        {
            AudioUI->AddToViewport();
            FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
            FVector2D CenteredPosition = ViewportSize / 2.0f;
            //AudioUI->SetPositionInViewport(CenteredPosition, true);
            LevelEditorUIPointer = AudioUI;

        }
    }
}


// Called every frame
void ALevelEditorManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (TestRunning)
    {
        float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
        SpawnObjectsAtTimeStamp(ElapsedTime);

        if (ElapsedTime >= 4.0f && !bAudioHasStarted)
        {
            bAudioHasStarted = true;
            LevelEditorUIPointer->GetAudioComponent()->Play();
        }
    }
    if (LevelEditorUIPointer->GetTestPressed() && !TestStarted)
    {
        StartTestSequence();
    }
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


void ALevelEditorManager::StartTestSequence()
{
    TestStarted = true;
    // Enable VR
    EnableVR();

    // Define test location and rotation directly
    FVector TestLocation(0.0, 914.249391, 83.633862); 
    FRotator TestRotation(0.0f, -180.0f, 0.0f);

    // Set player location and rotation for the test sequence
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PlayerController->GetPawn())
        {
            PlayerPawn->SetActorLocation(TestLocation);
            PlayerPawn->SetActorRotation(TestRotation);
            UE_LOG(LogTemp, Log, TEXT("Player positioned at test location and rotation"));

            //EquipKnivesToPlayer(PlayerPawn);
        }
    }

    LevelEditorUIPointer->SetVisibility(ESlateVisibility::Collapsed);
    //StartTime = GetWorld()->GetTimeSeconds();
    return;
}


void ALevelEditorManager::EnableVR()
{
    if (GEngine && GEngine->XRSystem.IsValid())
    {
        // Enable VR rendering (turn on stereo view)
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            // This console command enables the stereo VR mode
            PlayerController->ConsoleCommand("vr.bEnableStereo 1");
        }
    }
}

void ALevelEditorManager::SpawnObjectsAtTimeStamp(float Time)
{
    if (level_object_index < LevelEditorUIPointer->ReadyLevel.Keys.Num())
    {
        if (Time >= LevelEditorUIPointer->ReadyLevel.Keys[level_object_index])
        {
            if (LevelEditorUIPointer->ReadyLevel.Values[level_object_index] & 0b1000)
                SpawnObjectInLane(FirstLane, ShootableObjectClass);
            if (LevelEditorUIPointer->ReadyLevel.Values[level_object_index] & 0b0001)
                SpawnObjectInLane(FourthLane, SliceableObjectClass);
            if (LevelEditorUIPointer->ReadyLevel.Values[level_object_index] & 0b0100)
                SpawnObjectInLane(FourthLane, KneadableObjectClass);
            level_object_index++;
        }
    }
    return;
}

void ALevelEditorManager::SpawnObjectInLane(FVector LaneLocation, TSubclassOf<AActor> ObjectToSpawnClass)
{
    if (!ObjectToSpawnClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ObjectToSpawnClass is not set!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World context is invalid!"));
        return;
    }

    // Spawn the specified object type
    AActor* SpawnedObject = World->SpawnActor<AActor>(
        ObjectToSpawnClass,
        LaneLocation,
        FRotator::ZeroRotator
    );

    if (!SpawnedObject)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn object!"));
        return;
    }

    // Check if the spawned object has a StaticMeshComponent
    UStaticMeshComponent* MeshComponent = SpawnedObject->FindComponentByClass<UStaticMeshComponent>();
    if (MeshComponent)
    {
        if (!MeshComponent->GetStaticMesh())
        {
            UE_LOG(LogTemp, Error, TEXT("Spawned object has no StaticMesh assigned!"));
            return;
        }

        // Apply impulse if physics simulation is enabled
        if (MeshComponent->IsSimulatingPhysics())
        {
            FVector ImpulseDirection = FVector(0.0f, 1.0f, 0.0f);
            float ImpulseStrength = 50000.0f;
            MeshComponent->AddImpulse(ImpulseDirection * ImpulseStrength);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned object's Mesh is not simulating physics."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawned object has no StaticMeshComponent."));
    }
}




void ALevelEditorManager::SetTestRunning(bool state)
{
    if(state == true)
        StartTime = GetWorld()->GetTimeSeconds();
    TestRunning = state;
}

bool ALevelEditorManager::GetTestRunning() const
{
    return TestRunning;
}

