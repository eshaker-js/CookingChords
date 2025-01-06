// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SliceableObject.generated.h"

UCLASS(Blueprintable)
class COOKINGCHORDS_API ASliceableObject : public AActor
{
	GENERATED_BODY()

private:
	bool is_sliced;

public:
	// Sets default values for this actor's properties
	ASliceableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* Mesh;

	UFUNCTION(BlueprintCallable, Category = "Slicing")
	virtual void OnOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Slicing")
	void SliceObject(); 

	UFUNCTION(BlueprintImplementableEvent, Category = "Slicing")
	void OnSliced();

	UFUNCTION(BlueprintCallable, Category = "Slicing")
	bool GetIsSliced() const;

	UFUNCTION(BlueprintCallable, Category = "Slicing")
	void SetIsSliced(bool bNewValue);

};
