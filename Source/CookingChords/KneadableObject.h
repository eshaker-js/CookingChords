// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SliceableObject.h"
#include "KneadableObject.generated.h"

/**
 * 
 */
UCLASS()
class COOKINGCHORDS_API AKneadableObject : public ASliceableObject
{
	GENERATED_BODY()

public:
	AKneadableObject();

	virtual void OnOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	
private:
	int HitPoints;
};
