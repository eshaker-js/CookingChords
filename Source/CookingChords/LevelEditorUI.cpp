#include "LevelEditorUI.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "DesktopPlatformModule.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "RuntimeAudioImporterLibrary.h"

bool ULevelEditorUI::Initialize()
{
    Super::Initialize();

    // Bind UI buttons
    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnPlayClicked);
    if (PauseButton)
        PauseButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnPauseClicked);
    if (UploadSongButton)
        UploadSongButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnUploadSongClicked);

    if (SongProgressSlider)
    {
        SongProgressSlider->OnValueChanged.AddDynamic(this, &ULevelEditorUI::OnSliderValueChanged);
        SongProgressSlider->OnMouseCaptureBegin.AddDynamic(this, &ULevelEditorUI::OnSliderCaptureBegin);
        SongProgressSlider->OnMouseCaptureEnd.AddDynamic(this, &ULevelEditorUI::OnSliderCaptureEnd);
    }

    return true;
}

void ULevelEditorUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Update slider only if the player isn't interacting with it manually
    if (AudioComponent && bIsPlaying && SongProgressSlider && ImportedSoundWave && !bIsUserInteractingWithSlider)
    {
        float CurrentTime = GetPlaybackTime();  // Get current playback time
        float SongDuration = ImportedSoundWave->Duration;

        if (SongDuration > 0)
        {
            float SliderValue = FMath::Clamp(CurrentTime / SongDuration, 0.0f, 1.0f);
            SongProgressSlider->SetValue(SliderValue);  // Update the slider based on the playback progress
        }
    }
}

void ULevelEditorUI::OnUploadSongClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        TArray<FString> OutFiles;
        FString DefaultPath = FPaths::ProjectContentDir();  // Use the project content directory for simplicity
        DesktopPlatform->OpenFileDialog(nullptr, TEXT("Select Audio File"), DefaultPath, TEXT(""), TEXT("Sound Files (*.wav)|*.wav"), EFileDialogFlags::None, OutFiles);

        if (OutFiles.Num() > 0)
        {
            FString SelectedFilePath = OutFiles[0];

            // Use RuntimeAudioImporter to import the sound
            AudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();

            // Bind the callback for import result
            AudioImporter->OnResultNative.AddUObject(this, &ULevelEditorUI::OnAudioImported);
            AudioImporter->ImportAudioFromFile(SelectedFilePath, ERuntimeAudioFormat::Auto);
        }
    }
}

void ULevelEditorUI::OnAudioImported(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedWave, ERuntimeImportStatus Status)
{
    if (Status == ERuntimeImportStatus::SuccessfulImport)
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully imported sound with duration: %f"), ImportedWave->Duration);

        // Assign the imported sound wave
        ImportedSoundWave = ImportedWave;

        // If AudioComponent hasn't been created yet, create it now
        if (!AudioComponent)
        {
            APlayerController* PlayerController = GetOwningPlayer();
            if (PlayerController && PlayerController->GetPawn())
            {
                APawn* Pawn = PlayerController->GetPawn();
                AudioComponent = NewObject<UAudioComponent>(Pawn);
                AudioComponent->AttachToComponent(Pawn->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
                AudioComponent->RegisterComponent();
            }
        }

        if (AudioComponent)
        {
            // Set the sound to the audio component and prepare for playback
            AudioComponent->SetSound(ImportedSoundWave);
            UE_LOG(LogTemp, Warning, TEXT("AudioComponent attached and registered to the Pawn."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to import audio."));
    }
}

void ULevelEditorUI::OnPlayClicked()
{
    if (ImportedSoundWave && AudioComponent)
    {
        if (!AudioComponent->IsPlaying())  // Check if it's not already playing
        {
            AudioComponent->Play(CurrentPlaybackPosition);  // Resume from the current position
            StartTime = GetWorld()->GetTimeSeconds();
            bIsPlaying = true;

            UE_LOG(LogTemp, Warning, TEXT("Playing sound at position: %f"), CurrentPlaybackPosition);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Audio is already playing"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AudioComponent or ImportedSoundWave is missing."));
    }
}

void ULevelEditorUI::OnPauseClicked()
{
    if (bIsPlaying && AudioComponent)
    {
        float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
        CurrentPlaybackPosition += ElapsedTime;  // Store the playback position
        AudioComponent->Stop();  // Stop the audio
        bIsPlaying = false;

        UE_LOG(LogTemp, Warning, TEXT("Paused sound at position: %f"), CurrentPlaybackPosition);
    }
}

void ULevelEditorUI::OnSliderValueChanged(float Value)
{
    if (bIsUserInteractingWithSlider && AudioComponent && ImportedSoundWave)
    {
        float NewPlaybackPosition = ImportedSoundWave->Duration * Value;
        CurrentPlaybackPosition = NewPlaybackPosition;
        AudioComponent->Play(NewPlaybackPosition);  // Manually setting the new playback time
        StartTime = GetWorld()->GetTimeSeconds();

        UE_LOG(LogTemp, Warning, TEXT("Slider moved to position: %f"), NewPlaybackPosition);
    }
}

void ULevelEditorUI::OnSliderCaptureBegin()
{
    bIsUserInteractingWithSlider = true;  // User has started dragging the slider
}

void ULevelEditorUI::OnSliderCaptureEnd()
{
    bIsUserInteractingWithSlider = false;  // User has released the slider
}

float ULevelEditorUI::GetPlaybackTime() const
{
    if (AudioComponent && bIsPlaying)
    {
        // Calculate the time elapsed since the audio started playing
        float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
        return CurrentPlaybackPosition + ElapsedTime;
    }
    return CurrentPlaybackPosition;  // If not playing, return the last known position
}
