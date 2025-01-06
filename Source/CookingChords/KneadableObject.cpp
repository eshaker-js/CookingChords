// Fill out your copyright notice in the Description page of Project Settings.


#include "KneadableObject.h"
#include "Logging/LogMacros.h"
DEFINE_LOG_CATEGORY_STATIC(LogKneadableObject, Log, All);


AKneadableObject::AKneadableObject()
{
	HitPoints = 5;
}

void AKneadableObject::OnOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HitPoints--;
	UE_LOG(LogKneadableObject, Warning, TEXT("Object Hit Detected! with hp %d"), HitPoints);
	if (HitPoints == 0)
	{
		SliceObject();
	}
}