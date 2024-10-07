#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Sound/SoundWave.h"
#include "RuntimeAudioImporterLibrary.h"
#include "LevelEditorUI.generated.h"

UCLASS()
class COOKINGCHORDS_API ULevelEditorUI : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    // Override the Tick function to update the slider as the song plays
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


    float GetPlaybackTime() const;

    bool bIsUserInteractingWithSlider = false;


protected:
    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    UButton* PauseButton;

    UPROPERTY(meta = (BindWidget))
    UButton* UploadSongButton;

    UPROPERTY(meta = (BindWidget))
    USlider* SongProgressSlider;

    UFUNCTION()
    void OnSliderCaptureBegin();

    UFUNCTION()
    void OnSliderCaptureEnd();


    UPROPERTY()
    UImportedSoundWave* ImportedSoundWave;  // SoundWave object for loaded audio

    UPROPERTY()
    UAudioComponent* AudioComponent;  // AudioComponent for playback

    UPROPERTY()
    URuntimeAudioImporterLibrary* AudioImporter;  // Audio importer for runtime importing

    bool bIsPlaying = false;  // Tracks if audio is playing
    float CurrentPlaybackPosition = 0.0f;  // Tracks current position in the audio
    float StartTime = 0.0f;  // Time when playback started

    // Function to handle playing sound
    UFUNCTION()
    void OnPlayClicked();

    // Function to handle pausing sound
    UFUNCTION()
    void OnPauseClicked();

    // Function to handle uploading sound
    UFUNCTION()
    void OnUploadSongClicked();

    // Function to handle slider change
    UFUNCTION()
    void OnSliderValueChanged(float Value);

    // Callback function when audio is imported successfully
    UFUNCTION()
    void OnAudioImported(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedWave, ERuntimeImportStatus Status);
};
