// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnBox.h"
#include "Components/BoxComponent.h"
#include "TrackamaniaPawn.h"

// Sets default values
ARespawnBox::ARespawnBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(50.0f, 10.0f, 50.0f));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARespawnBox::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ARespawnBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARespawnBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARespawnBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ATrackamaniaPawn::StaticClass()))
	{
		Cast<ATrackamaniaPawn>(OtherActor)->Respawn();
	}
}


