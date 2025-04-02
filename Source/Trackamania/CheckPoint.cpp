// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "Components/BoxComponent.h"
#include "TrackamaniaPawn.h"

// Sets default values
ACheckPoint::ACheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(50.0f, 10.0f, 50.0f));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnOverlapBegin);
}


// Called when the game starts or when spawned
void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		if (OtherActor->IsA(ATrackamaniaPawn::StaticClass()))
		{
			ATrackamaniaPawn* other = Cast<ATrackamaniaPawn>(OtherActor);
			if (other->IsValidLowLevel())
			{
				if (!IsEnd)
				{
					other->SetRespawn(this);
				}
				else
				{
					other->SaveBestTime();
					other->FullReset();
				}
			}
		}

}

