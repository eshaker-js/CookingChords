#include "LevelEditorUI.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "DesktopPlatformModule.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"

bool ULevelEditorUI::Initialize()
{
    Super::Initialize();

    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnPlayClicked);
    if (PauseButton)
        PauseButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnPauseClicked);
    if (UploadSongButton)
        UploadSongButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnUploadSongClicked);
    if (SongProgressSlider)
        SongProgressSlider->OnValueChanged.AddDynamic(this, &ULevelEditorUI::OnSliderValueChanged);

    return true;
}

void ULevelEditorUI::OnPlayClicked()
{
    if (LoadedSoundWave)
    {
        UGameplayStatics::PlaySound2D(this, LoadedSoundWave);
        StartTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Warning, TEXT("Playing loaded procedural sound."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No sound loaded to play."));
    }
}

void ULevelEditorUI::OnPauseClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Pause functionality not supported in this simplified setup."));
}

void ULevelEditorUI::OnUploadSongClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        TArray<FString> OutFiles;
        FString DefaultPath = FPaths::ProjectContentDir();
        DesktopPlatform->OpenFileDialog(nullptr, TEXT("Select Audio File"), DefaultPath, TEXT(""), TEXT("Sound Files (*.wav)|*.wav"), EFileDialogFlags::None, OutFiles);

        if (OutFiles.Num() > 0)
        {
            FString SelectedFilePath = OutFiles[0];
            UE_LOG(LogTemp, Warning, TEXT("Selected File Path: %s"), *SelectedFilePath);
            LoadedSoundWave = LoadWavAsProcedural(SelectedFilePath);  // Load as procedural sound wave
            if (LoadedSoundWave)
            {
                TotalSongDuration = LoadedSoundWave->Duration;
                UE_LOG(LogTemp, Warning, TEXT("Loaded procedural sound with duration: %f"), TotalSongDuration);
            }
        }
    }
}

void ULevelEditorUI::OnSliderValueChanged(float Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Slider functionality is not supported."));
}

USoundWaveProcedural* ULevelEditorUI::LoadWavAsProcedural(const FString& FilePath)
{
    TArray<uint8> RawFileData;
    if (!FFileHelper::LoadFileToArray(RawFileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return nullptr;
    }

    FWaveModInfo WaveInfo;
    if (!WaveInfo.ReadWaveInfo(RawFileData.GetData(), RawFileData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read wave info from file: %s"), *FilePath);
        return nullptr;
    }

    // Create a new procedural sound wave
    USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
    SoundWave->SetSampleRate(*WaveInfo.pSamplesPerSec);
    SoundWave->NumChannels = *WaveInfo.pChannels;
    SoundWave->Duration = (float)(*WaveInfo.pWaveDataSize) / (*WaveInfo.pSamplesPerSec * *WaveInfo.pChannels * sizeof(int16));

    // Store full PCM data for level creation or waveform visualization
    TArray<uint8> FullAudioData;
    FullAudioData.Append(RawFileData);  // Store all the audio data in memory

    // Stream audio data into the procedural sound wave
    const int32 BufferSize = 16384;  // Stream chunk size
    for (int32 Offset = 0; Offset < RawFileData.Num(); Offset += BufferSize)
    {
        int32 RemainingSize = FMath::Min(BufferSize, RawFileData.Num() - Offset);
        SoundWave->QueueAudio(&RawFileData[Offset], RemainingSize);
    }

    // Now you can use FullAudioData later for waveform visualization or gameplay logic
    // Example: FullAudioData contains the entire PCM audio data

    return SoundWave;
}

