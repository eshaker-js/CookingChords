#include "LevelEditorUI.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "DesktopPlatformModule.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "RuntimeAudioImporterLibrary.h"
#include "SlateBasics.h"
#include "Slate/SlateBrushAsset.h"
#include "Widgets/Images/SImage.h"
#include "Styling/SlateBrush.h"
#include "dr_wav.h"


bool ULevelEditorUI::Initialize()
{
    Super::Initialize();

    TestPressed = false;
    UpdateLoadingUI(false);
    // Bind UI buttons
    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnPlayClicked);
    if (SaveButton)
        SaveButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnSaveClicked);
    if (UploadSongButton)
        UploadSongButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnUploadSongClicked);
    if (TestButton)
        TestButton->OnClicked.AddDynamic(this, &ULevelEditorUI::OnTestClicked);

    if (SongProgressSlider)
    {
        SongProgressSlider->OnValueChanged.AddDynamic(this, &ULevelEditorUI::OnSliderValueChanged);
        SongProgressSlider->OnMouseCaptureBegin.AddDynamic(this, &ULevelEditorUI::OnSliderCaptureBegin);
        SongProgressSlider->OnMouseCaptureEnd.AddDynamic(this, &ULevelEditorUI::OnSliderCaptureEnd);
    }

    if (Sliceable_Lane_1)
    {
        // Set the button's normal style to fully green with low opacity
        FButtonStyle ButtonStyle = Sliceable_Lane_1->WidgetStyle;

        // Create a green color with low opacity
        FSlateBrush BackgroundBrush;
        BackgroundBrush.TintColor = FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f, 0.3f));  // Green with 0.3 opacity
        BackgroundBrush.DrawAs = ESlateBrushDrawType::Box;  // Draw the background as a box

        // Set the button style for the entire button
        ButtonStyle.Normal = BackgroundBrush;
        ButtonStyle.Hovered = BackgroundBrush;
        ButtonStyle.Pressed = BackgroundBrush;

        SelectedButtonStyle = ButtonStyle;


        NormalButtonStyle = Sliceable_Lane_1->WidgetStyle;
        Sliceable_Lane_1->OnClicked.AddDynamic(this, &ULevelEditorUI::OnSliceableLane1Clicked);
        Sliceable_Lane_1->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (Sliceable_Lane_2)
    {
        Sliceable_Lane_2->OnClicked.AddDynamic(this, &ULevelEditorUI::OnSliceableLane2Clicked);
        Sliceable_Lane_2->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (Shootable_Lane_1)
    {
        Shootable_Lane_1->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (Shootable_Lane_2)
    {
        Shootable_Lane_2->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (Fat_Lane)
    {
        Fat_Lane->SetVisibility(ESlateVisibility::Collapsed);
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

            // Load the .wav file into an array
            TArray<uint8> WavData;
            if (!FFileHelper::LoadFileToArray(WavData, *SelectedFilePath))
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load .wav file to array."));
                return;
            }

            // Initialize drwav for reading from memory
            drwav wav;
            if (!drwav_init_memory(&wav, WavData.GetData(), WavData.Num(), nullptr))
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to initialize dr_wav with the provided WAV data."));
                return;
            }

            // Calculate total sample count
            size_t totalSampleCount = wav.totalPCMFrameCount * wav.channels;

            // Allocate memory to hold the PCM frames
            int16* pPCMFrames = (int16*)malloc(totalSampleCount * sizeof(int16));
            if (pPCMFrames == nullptr)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to allocate memory for PCM frames."));
                drwav_uninit(&wav);
                return;
            }

            // Read PCM frames
            size_t framesRead = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pPCMFrames);
            if (framesRead != wav.totalPCMFrameCount)
            {
                UE_LOG(LogTemp, Warning, TEXT("Mismatch in frames read. Expected: %llu, Actual: %llu"), wav.totalPCMFrameCount, framesRead);
                // If there's a mismatch, attempt to read again or handle the issue appropriately
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Successfully read all PCM frames."));
            }

            float totalDuration = totalSampleCount / wav.sampleRate;
            // Use the PCM data to generate the waveform
            GenerateWaveformFromPCMData(pPCMFrames, framesRead * wav.channels, totalDuration);

            // Clean up allocated memory and dr_wav resources
            free(pPCMFrames);
            drwav_uninit(&wav);

            UpdateLoadingUI(true);

            // Use RuntimeAudioImporter to import the sound
            AudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
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

        /*const uint8* AudioData = ImportedSoundWave->GetResourceData();
        int32 DataSize = ImportedSoundWave->GetResourceSize();

        // Process the audio data and generate a waveform
        GenerateWaveformTexture(AudioData, DataSize);*/

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
        UpdateLoadingUI(false);

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
            CollapseAllButtons();
            AudioComponent->Play(CurrentPlaybackPosition);  // Resume from the current position
            StartTime = GetWorld()->GetTimeSeconds();
            bIsPlaying = true;

            UE_LOG(LogTemp, Warning, TEXT("Playing sound at position: %f"), CurrentPlaybackPosition);
        }
        else
        {
            float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
            CurrentPlaybackPosition += ElapsedTime;  // Store the playback position
            AudioComponent->Stop();  // Stop the audio
            bIsPlaying = false;

            ShowButtonsAtPlaybackTime(CurrentPlaybackPosition);

            UE_LOG(LogTemp, Warning, TEXT("Paused sound at position: %f"), CurrentPlaybackPosition);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AudioComponent or ImportedSoundWave is missing."));
    }
}

void ULevelEditorUI::OnSaveClicked()
{
    // Iterate over the LaneSelections map and copy the keys and values into a matrix with 2 rows
    TArray<TArray<float>> Matrix;
    Matrix.SetNum(2); // Create a matrix with 2 rows

    for (const auto& Entry : LaneSelections)
    {
        Matrix[0].Add(Entry.Key);  // First row for keys
        Matrix[1].Add(Entry.Value);        // Second row for values
    }

    // Sort the matrix based on the keys (first row)
    TArray<int32> Indices;
    Indices.SetNum(Matrix[0].Num());
    for (int32 i = 0; i < Indices.Num(); ++i)
    {
        Indices[i] = i;
    }

    Indices.Sort([&Matrix](int32 A, int32 B) {
        return Matrix[0][A] < Matrix[0][B];
        });

    ReadyLevel.Keys.Empty();
    ReadyLevel.Values.Empty();
    for (int32 i = 0; i < Indices.Num(); ++i)
    {
        ReadyLevel.Keys.Add(Matrix[0][Indices[i]]);
        ReadyLevel.Values.Add(Matrix[1][Indices[i]]);
    }

    // JSON serialization and saving logic commented out
/*
// Convert the sorted matrix to a JSON string
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    Writer->WriteObjectStart();
    Writer->WriteArrayStart("Keys");
    for (float Key : SortedMatrix[0])
    {
        Writer->WriteValue(Key);
    }
    Writer->WriteArrayEnd();

    Writer->WriteArrayStart("Values");
    for (float Value : SortedMatrix[1])
    {
        Writer->WriteValue(Value);
    }
    Writer->WriteArrayEnd();
    Writer->WriteObjectEnd();
    Writer->Close();

    FString SavePath = FPaths::ProjectContentDir() + TEXT("SavedLevels/LevelData.json");
    FFileHelper::SaveStringToFile(OutputString, *SavePath);

    UE_LOG(LogTemp, Log, TEXT("Level data saved to %s"), *SavePath);
*/

    return;
}

void ULevelEditorUI::OnTestClicked()
{
    TestPressed = true;
    return;
}

void ULevelEditorUI::OnSliderValueChanged(float Value)
{
    if (bIsUserInteractingWithSlider && AudioComponent && ImportedSoundWave)
    {
        float NewPlaybackPosition = ImportedSoundWave->Duration * Value;
        CurrentPlaybackPosition = NewPlaybackPosition;
        if (bIsPlaying)
        {
            AudioComponent->Play(NewPlaybackPosition);  // Manually setting the new playback time
        }
        else
        {
            ShowButtonsAtPlaybackTime(CurrentPlaybackPosition);
        }
        StartTime = GetWorld()->GetTimeSeconds();
    }
}

void ULevelEditorUI::OnSliderCaptureBegin()
{
    bIsUserInteractingWithSlider = true;  // User has started dragging the slider
}

void ULevelEditorUI::OnSliderCaptureEnd()
{
    bIsUserInteractingWithSlider = false;  // User has released the slider
    UE_LOG(LogTemp, Warning, TEXT("Slider moved to position: %f"), CurrentPlaybackPosition);
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


void ULevelEditorUI::UpdateLoadingUI(bool bIsLoading)
{
    if (LoadingSpinner && LoadingText)  // Ensure the widgets exist
    {
        if (bIsLoading)
        {
            // Show the spinner and message
            LoadingSpinner->SetVisibility(ESlateVisibility::Visible);
            LoadingText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            // Hide the spinner and message
            LoadingSpinner->SetVisibility(ESlateVisibility::Hidden);
            LoadingText->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}


void ULevelEditorUI::SetSliderBackground(UTexture2D* WaveformTexture)
{
    if (SongProgressSlider && WaveformTexture)
    {
        // Get the current style of the slider
        FSliderStyle SliderStyle = SongProgressSlider->GetWidgetStyle();

        if (!WaveformTexture)
        {
            UE_LOG(LogTemp, Error, TEXT("Waveform texture is null."));
        }
        if (!SongProgressSlider)
        {
            UE_LOG(LogTemp, Error, TEXT("Song progress slider is null."));
        }

        // Create a brush for the background image using the waveform texture
        FSlateBrush WaveformBrush;
        WaveformBrush.SetResourceObject(WaveformTexture);
        WaveformBrush.ImageSize = FVector2D(512, 128);  // Adjust size to match texture dimensions
        WaveformBrush.Tiling = ESlateBrushTileType::Both;  // Ensure it tiles properly if needed
        WaveformBrush.DrawAs = ESlateBrushDrawType::Image;

        // Apply the waveform texture as the background
        SliderStyle.SetNormalBarImage(WaveformBrush);
        SliderStyle.SetHoveredBarImage(WaveformBrush);  // Ensure it shows on hover
        SliderStyle.SetDisabledBarImage(WaveformBrush); // Ensure it shows when disabled

        // Update the slider's style
        SongProgressSlider->SetWidgetStyle(SliderStyle);
        SongProgressSlider->SynchronizeProperties();  // Force the slider to refresh its style
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Slider or WaveformTexture is null."));
    }
}


// Function to generate waveform texture from amplitude data
void ULevelEditorUI::GenerateWaveformTextureFromAmplitudes(const TArray<float>& Amplitudes)
{
    if (Amplitudes.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid amplitude data."));
        return;
    }

    // Get the dimensions of the slider to dynamically create a texture that matches
    FVector2D SliderDimensions = SongProgressSlider->GetDesiredSize();
    int32 TextureWidth = FMath::RoundToInt(SliderDimensions.X);
    int32 TextureHeight = FMath::RoundToInt(SliderDimensions.Y);

    // Safety check if slider dimensions are valid
    if (TextureWidth <= 0 || TextureHeight <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid slider dimensions."));
        return;
    }

    // Create the transient texture to match the slider size
    PersistentWaveformTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_B8G8R8A8);

    if (!PersistentWaveformTexture || !PersistentWaveformTexture->GetPlatformData())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture or platform data is invalid."));
        return;
    }

    // Set texture properties to ensure they render properly in UI
    PersistentWaveformTexture->MipGenSettings = TMGS_NoMipmaps;
    PersistentWaveformTexture->CompressionSettings = TC_VectorDisplacementmap;  // Avoid color artifacts
    PersistentWaveformTexture->SRGB = true;  // sRGB may look better for UI purposes

    // Lock the texture for editing
    FTexture2DMipMap& Mip = PersistentWaveformTexture->GetPlatformData()->Mips[0];
    FColor* TextureData = static_cast<FColor*>(Mip.BulkData.Lock(LOCK_READ_WRITE));

    if (!TextureData)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to lock texture data."));
        return;
    }

    // Clear the texture to black
    for (int32 i = 0; i < TextureWidth * TextureHeight; ++i)
    {
        TextureData[i] = FColor(0, 0, 0, 255); // Set fully opaque black
    }

    int32 AmplitudeCount = Amplitudes.Num();
    float AmplitudesPerPixel = static_cast<float>(AmplitudeCount) / TextureWidth;

    for (int32 X = 0; X < TextureWidth; ++X)
    {
        // Calculate the range of amplitudes to consider for this pixel
        int32 StartAmplitudeIndex = FMath::RoundToInt(X * AmplitudesPerPixel);
        int32 EndAmplitudeIndex = FMath::RoundToInt((X + 1) * AmplitudesPerPixel);

        float MaxAmplitude = 0.0f;
        float SumAmplitude = 0.0f;
        int32 Count = 0;

        // Compute max and average amplitude for the current segment
        for (int32 i = StartAmplitudeIndex; i < EndAmplitudeIndex && i < AmplitudeCount; ++i)
        {
            float Amplitude = Amplitudes[i];
            MaxAmplitude = FMath::Max(MaxAmplitude, Amplitude);
            SumAmplitude += Amplitude;
            Count++;
        }

        float AverageAmplitude = (Count > 0) ? (SumAmplitude / Count) : 0.0f;

        // Use max amplitude for more distinct waveform peaks
        float FinalAmplitude = MaxAmplitude;
        FinalAmplitude = FMath::Clamp(FinalAmplitude, 0.0f, 1.0f);

        int32 WaveHeight = FMath::RoundToInt(FinalAmplitude * TextureHeight);

        // Draw the waveform as a vertical line
        for (int32 Y = 0; Y < TextureHeight; ++Y)
        {
            if (Y < WaveHeight)
            {
                TextureData[(TextureHeight - 1 - Y) * TextureWidth + X] = FColor(255, 255, 255, 255); // White for waveform
            }
        }
    }

    // Unlock and update the texture
    Mip.BulkData.Unlock();
    PersistentWaveformTexture->UpdateResource();

    // Set the generated texture as the slider background
    SetSliderBackground(PersistentWaveformTexture);
}

void ULevelEditorUI::GenerateWaveformFromPCMData(int16* PCMData, int32 NumSamples, float DurationSeconds)
{
    if (PCMData == nullptr || NumSamples <= 0 || DurationSeconds <= 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid PCM data, number of samples, or duration."));
        return;
    }

    // Get slider dimensions to determine texture width
    FVector2D SliderDimensions = SongProgressSlider->GetDesiredSize();
    int32 TextureWidth = FMath::RoundToInt(SliderDimensions.X);
    if (TextureWidth <= 0)
    {
        TextureWidth = 512; // Fallback to default if slider dimensions are invalid
    }

    TArray<float> WaveformAmplitudes;

    // Calculate how many samples each pixel represents, adjusted based on duration
    float SamplesPerPixel = (static_cast<float>(NumSamples) / TextureWidth) * (DurationSeconds / 10.0f); // Longer durations get more samples per pixel

    // Process each pixel's amplitude using max amplitude for distinct waveform peaks
    for (int32 X = 0; X < TextureWidth; ++X)
    {
        int32 StartSampleIndex = FMath::RoundToInt(X * SamplesPerPixel);
        int32 EndSampleIndex = FMath::RoundToInt((X + 1) * SamplesPerPixel);

        float MaxAmplitude = 0.0f;
        float SumAmplitude = 0.0f;
        int32 Count = 0;

        // Find max and accumulate amplitudes for averaging
        for (int32 i = StartSampleIndex; i < EndSampleIndex && i < NumSamples; ++i)
        {
            float Amplitude = FMath::Abs(static_cast<float>(PCMData[i])) / 32768.0f; // Normalize to [0, 1]
            MaxAmplitude = FMath::Max(MaxAmplitude, Amplitude);
            SumAmplitude += Amplitude;
            Count++;
        }

        float AverageAmplitude = (Count > 0) ? (SumAmplitude / Count) : 0.0f;

        // Add max amplitude for distinct peaks
        WaveformAmplitudes.Add(FMath::Clamp(MaxAmplitude, 0.0f, 1.0f));
    }

    // Apply a broader moving average to reduce sharp variations, but reduced smoothing range for better detail
    int32 SmoothRange = 2;  // Reduced smoothing for more detail
    for (int32 i = SmoothRange; i < WaveformAmplitudes.Num() - SmoothRange; ++i)
    {
        float Sum = 0.0f;
        for (int32 j = -SmoothRange; j <= SmoothRange; ++j)
        {
            Sum += WaveformAmplitudes[i + j];
        }
        WaveformAmplitudes[i] = Sum / (2 * SmoothRange + 1);
    }

    GenerateWaveformTextureFromAmplitudes(WaveformAmplitudes);
}



void ULevelEditorUI::OnSliceableLane1Clicked()
{
    if (!bIsPlaying)
    {
        float CurrentTime = GetPlaybackTime();

        // Update the lane selection data for the first button
        UpdateLaneSelection(CurrentTime, 0b1000);  // 1000 in binary represents the first lane

        // Update the button style based on the updated lane selection
        UpdateButtonStyle(Sliceable_Lane_1, LaneSelections[CurrentTime], 0b1000);
    }
}

void ULevelEditorUI::OnSliceableLane2Clicked()
{
    if (!bIsPlaying)
    {
        float CurrentTime = GetPlaybackTime();

        // Update the lane selection data for the second button
        UpdateLaneSelection(CurrentTime, 0b0001);  // 0100 in binary represents the second lane

        // Update the button style based on the updated lane selection
        UpdateButtonStyle(Sliceable_Lane_2, LaneSelections[CurrentTime], 0b0001);
    }
}

void ULevelEditorUI::UpdateLaneSelection(float CurrentTime, int Selection)
{
    if (!LaneSelections.Contains(CurrentTime))
    {
        // Add a new entry with the specified lane value
        LaneSelections.Add(CurrentTime, Selection);
        UE_LOG(LogTemp, Log, TEXT("Added new lane selection at time %f: %d"), CurrentTime, Selection);
    }
    else
    {
        // To turn off only the specified lane if it is on
        if ((LaneSelections[CurrentTime] & Selection) != 0)
        {
            // Use AND with NOT to turn off the specified lane
            LaneSelections[CurrentTime] &= ~Selection;
            UE_LOG(LogTemp, Log, TEXT("Turned off lane selection at time %f: %d"), CurrentTime, LaneSelections[CurrentTime]);
        }
        else
        {
            // Otherwise, turn it on
            LaneSelections[CurrentTime] |= Selection;
            UE_LOG(LogTemp, Log, TEXT("Turned on lane selection at time %f: %d"), CurrentTime, LaneSelections[CurrentTime]);
        }
    }
}

void ULevelEditorUI::UpdateButtonStyle(UButton* Button, int LaneState, int Selection)
{
    if ((LaneState & Selection) != 0)  // Check if the specific lane is selected
    {
        Button->SetStyle(SelectedButtonStyle);
    }
    else
    {
        // Set the button back to the normal style with a black border
        Button->SetStyle(NormalButtonStyle);
    }
}


void ULevelEditorUI::ShowButtonsAtPlaybackTime(float CurrentTime)
{
    if (LaneSelections.Contains(CurrentTime))
    {
        (LaneSelections[CurrentTime] & 0b1000) != 0 ?
            Sliceable_Lane_1->SetStyle(SelectedButtonStyle) :
            Sliceable_Lane_1->SetStyle(NormalButtonStyle);

        (LaneSelections[CurrentTime] & 0b0001) != 0 ?
            Sliceable_Lane_2->SetStyle(SelectedButtonStyle) :
            Sliceable_Lane_2->SetStyle(NormalButtonStyle);
    }
    else
    {
        Sliceable_Lane_1->SetStyle(NormalButtonStyle);
        Sliceable_Lane_2->SetStyle(NormalButtonStyle);
    }
    ShowAllButtons();
}

void ULevelEditorUI::ShowAllButtons()
{
    Sliceable_Lane_1->SetVisibility(ESlateVisibility::Visible);
    Sliceable_Lane_2->SetVisibility(ESlateVisibility::Visible);
    /*Shootable_Lane_1->SetVisibility(ESlateVisibility::Visible);
    Shootable_Lane_2->SetVisibility(ESlateVisibility::Visible);
    Fat_Lane->SetVisibility(ESlateVisibility::Visible);*/
}

void ULevelEditorUI::CollapseAllButtons()
{
    Sliceable_Lane_1->SetVisibility(ESlateVisibility::Collapsed);
    Sliceable_Lane_2->SetVisibility(ESlateVisibility::Collapsed);
    /*Shootable_Lane_1->SetVisibility(ESlateVisibility::Collapsed);
    Shootable_Lane_2->SetVisibility(ESlateVisibility::Collapsed);
    Fat_Lane->SetVisibility(ESlateVisibility::Collapsed);*/
}

bool ULevelEditorUI::GetTestPressed()
{
    return this->TestPressed;
}

void ULevelEditorUI::SetTestPressed(bool x)
{
    this->TestPressed = x;
}