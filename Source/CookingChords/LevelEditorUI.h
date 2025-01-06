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


USTRUCT(BlueprintType)
struct FReadyLevel
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<float> Keys;

    UPROPERTY()
    TArray<int> Values;
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

    UFUNCTION()
    bool GetTestPressed();

    UFUNCTION()
    void SetTestPressed(bool x);


    UPROPERTY()
    FReadyLevel ReadyLevel;

    UAudioComponent* GetAudioComponent();


protected:
    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SaveButton;

    UPROPERTY(meta = (BindWidget))
    UButton* TestButton;

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

    UPROPERTY(meta = (BindWidget))
    UButton* Shootable_Lane_1;

    UPROPERTY(meta = (BindWidget))
    UButton* Shootable_Lane_2;

    UPROPERTY(meta = (BindWidget))
    UButton* Fat_Lane;

    UPROPERTY()
    FButtonStyle NormalButtonStyle;

    UPROPERTY()
    FButtonStyle SelectedButtonStyle;

    TMap<float, int> LaneSelections;

    UPROPERTY()
    bool TestPressed;

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
    void OnSaveClicked();

    UFUNCTION()
    void OnTestClicked();

    // Function to handle uploading sound
    UFUNCTION()
    void OnUploadSongClicked();

    UFUNCTION()
    void OnSliceableLane1Clicked();
    
    UFUNCTION()
    void OnSliceableLane2Clicked();

    UFUNCTION()
    void OnKneadableLaneClicked();

    UFUNCTION()
    void OnCircularableLaneClicked();

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

    void UpdateLaneSelection(float CurrentTime, int Selection);

    void UpdateButtonStyle(UButton* Button, int LaneState, int Selection);

    void ShowButtonsAtPlaybackTime(float CurrentTime);

    void CollapseAllButtons();

    void ShowAllButtons();


};
