#include "LevelEditorManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/Engine.h"
#include "SliceableObject.h"
#include "MotionControllerComponent.h"
#define FirstLane FVector(-383.607434, -1085.750609, 83.633865)
#define FourthLane FVector(383.607377, -1085.750609, 83.633865)


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
    if (LevelEditorUIPointer->GetTestPressed() && !TestRunning)
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
    TestRunning = true;
    // Enable VR
    EnableVR();

    // Define test location and rotation directly
    FVector TestLocation(0.0, 914.249391, 83.633862); 
    FRotator TestRotation(0.0f, -90.0f, 0.0f);

    // Set player location and rotation for the test sequence
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PlayerController->GetPawn())
        {
            PlayerPawn->SetActorLocation(TestLocation);
            PlayerPawn->SetActorRotation(TestRotation);
            UE_LOG(LogTemp, Log, TEXT("Player positioned at test location and rotation"));

            EquipKnivesToPlayer(PlayerPawn);
        }
    }

    LevelEditorUIPointer->SetVisibility(ESlateVisibility::Collapsed);
    StartTime = GetWorld()->GetTimeSeconds();
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
                SpawnObjectInLane(FirstLane);
            if (LevelEditorUIPointer->ReadyLevel.Values[level_object_index] & 0b0001)
                SpawnObjectInLane(FourthLane);
            level_object_index++;
        }
    }
    return;
}

void ALevelEditorManager::SpawnObjectInLane(FVector LaneLocation)
{
    if (!SliceableObjectClass)
    {
        UE_LOG(LogTemp, Error, TEXT("SliceableObjectClass is not set!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World context is invalid!"));
        return;
    }

    // Spawn the Blueprint-derived class
    ASliceableObject* SliceableObject = World->SpawnActor<ASliceableObject>(
        SliceableObjectClass,
        LaneLocation,
        FRotator::ZeroRotator
    );

    if (SliceableObject->Mesh)
    {
        if (!SliceableObject->Mesh->GetStaticMesh())
        {
            UE_LOG(LogTemp, Error, TEXT("SliceableObject has no StaticMesh assigned!"));
            return;
        }

        if (SliceableObject->Mesh->IsSimulatingPhysics())
        {
            FVector ImpulseDirection = FVector(0.0f, 1.0f, 0.0f);
            float ImpulseStrength = 50000.0f;
            SliceableObject->Mesh->AddImpulse(ImpulseDirection * ImpulseStrength);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SliceableObject's Mesh is not simulating physics."));
        }
    }

}


void ALevelEditorManager::EquipKnivesToPlayer(APawn* Player)
{
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("Player is not valid! Cannot equip knives."));
        return;
    }
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World context is invalid!"));
        return;
    }

    // Find the motion controllers
    UMotionControllerComponent* LeftController = Cast<UMotionControllerComponent>(Player->FindComponentByClass<UMotionControllerComponent>());
    UMotionControllerComponent* RightController = nullptr;
    APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null! Cannot simulate grab actions."));
        return;
    }

    // Attempt to get the second motion controller
    for (UActorComponent* Component : Player->GetComponents())
    {
        UMotionControllerComponent* MC = Cast<UMotionControllerComponent>(Component);
        if (MC && MC != LeftController)
        {
            RightController = MC;
            break;
        }
    }

    if (!LeftController || !RightController)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find both motion controllers on the player."));
        return;
    }

    // Spawn and attach the left-hand knife
    if (LeftKnifeClass)
    {
        AActor* LeftKnife = World->SpawnActor<AActor>(LeftKnifeClass, LeftController->GetComponentLocation(), FRotator::ZeroRotator);
        if (LeftKnife)
        {
            LeftKnife->SetActorScale3D(FVector(0.052155f, 0.052155f, 0.162417f)); //Set correct scale

            FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
            LeftKnife->AttachToComponent(LeftController, AttachRules);
            UE_LOG(LogTemp, Log, TEXT("Left-hand knife equipped."));
            PlayerController->InputKey(FKey("IA_Left_Grab"), EInputEvent::IE_Pressed, 1.0f, false);

        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn left-hand knife."));
        }
    }

    // Spawn and attach the right-hand knife
    if (RightKnifeClass)
    {
        AActor* RightKnife = World->SpawnActor<AActor>(RightKnifeClass, RightController->GetComponentLocation(), FRotator::ZeroRotator);
        if (RightKnife)
        {
            RightKnife->SetActorScale3D(FVector(0.052155f, 0.052155f, 0.162417f));

            FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
            RightKnife->AttachToComponent(RightController, AttachRules);
            UE_LOG(LogTemp, Log, TEXT("Right-hand knife equipped."));
            PlayerController->InputKey(FKey("IA_Right_Grab"), EInputEvent::IE_Pressed, 1.0f, false);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn right-hand knife."));
        }
    }
}



