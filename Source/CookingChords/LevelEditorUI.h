#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/CircularThrobber.h"
#include "Components/TextBlock.h"
#include "Containers/Map.h"
#include "Sound/SoundWave.h"
#include "RuntimeAudioImporterLibrary.h"
#include "LevelEditorUI.generated.h"


// Enum for lane selections
UENUM(BlueprintType)
enum class ELaneSelection : uint8{
    NONE_SELECTED  UMETA(DisplayName = "No Selecetd Lanes"),
    FIRST_LANE    UMETA(DisplayName = "First Lane"),
    SECOND_LANE   UMETA(DisplayName = "Second Lane"),
    BOTH_LANES    UMETA(DisplayName = "Both Lanes")
    // Add more configurations as needed
};

UCLASS()
class COOKINGCHORDS_API ULevelEditorUI : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    // Override the Tick function to update the slider as the song plays
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void UpdateLoadingUI(bool bIsLoading);

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

    UPROPERTY(meta = (BindWidget))
    UCircularThrobber* LoadingSpinner;  

    UPROPERTY(meta = (BindWidget))
    UTextBlock* LoadingText;  

    //Level related UI that stores relavant information we need during level playback

    UPROPERTY(meta = (BindWidget))
    UButton* Sliceable_Lane_1;

    UPROPERTY(meta = (BindWidget))
    UButton* Sliceable_Lane_2;

    TMap<float, ELaneSelection> LaneSelections;

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

    UPROPERTY()
    UTexture2D* PersistentWaveformTexture;



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

    UFUNCTION()
    void OnSliceableLane1Clicked();
    
    UFUNCTION()
    void OnSliceableLane2Clicked();

    // Function to handle slider change
    UFUNCTION()
    void OnSliderValueChanged(float Value);

    // Callback function when audio is imported successfully
    UFUNCTION()
    void OnAudioImported(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedWave, ERuntimeImportStatus Status);

    void GenerateWaveformTextureFromAmplitudes(const TArray<float>& Amplitudes);

    UFUNCTION()
    void SetSliderBackground(UTexture2D* WaveformTexture);

    void GenerateWaveformFromPCMData(int16* PCMData, int32 NumSamples, float DurationSeconds);

    void UpdateLaneSelection(float CurrentTime, ELaneSelection Selection);

    void UpdateButtonStyle(UButton* Button, ELaneSelection Selection);
};
