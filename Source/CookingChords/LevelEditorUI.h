#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Sound/SoundWaveProcedural.h"  // Use SoundWaveProcedural for streaming audio
#include "LevelEditorUI.generated.h"

UCLASS()
class COOKINGCHORDS_API ULevelEditorUI : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    UButton* PauseButton;

    UPROPERTY(meta = (BindWidget))
    UButton* UploadSongButton;

    UPROPERTY(meta = (BindWidget))
    USlider* SongProgressSlider;

    UPROPERTY()
    USoundWaveProcedural* LoadedSoundWave;  // Procedural SoundWave

    float TotalSongDuration;
    float CurrentPlaybackPosition = 0.0f;
    float StartTime = 0.0f;

    UFUNCTION()
    void OnPlayClicked();

    UFUNCTION()
    void OnPauseClicked();

    UFUNCTION()
    void OnUploadSongClicked();

    UFUNCTION()
    void OnSliderValueChanged(float Value);

    // Function to load a WAV file and create a procedural USoundWave
    USoundWaveProcedural* LoadWavAsProcedural(const FString& FilePath);

    // Function to play the loaded WAV file using PlaySound2D
    void PlayWavFile(const FString& FilePath);
};
