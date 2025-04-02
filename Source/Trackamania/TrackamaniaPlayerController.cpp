// Copyright Epic Games, Inc. All Rights Reserved.


#include "TrackamaniaPlayerController.h"
#include "TrackamaniaPawn.h"
#include "TrackamaniaUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "EngineUtils.h"

void ATrackamaniaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// spawn the UI widget and add it to the viewport
	VehicleUI = CreateWidget<UTrackamaniaUI>(this, VehicleUIClass);

	check(VehicleUI);

	VehicleUI->AddToViewport();

	for (TActorIterator<ACheckPoint> It(GetWorld()); It; ++It)
	{
		if (*It)
		{
			CheckPoints.Add(*It);
		}
	}
}

void ATrackamaniaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		// optionally add the steering wheel context
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void ATrackamaniaPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
		VehicleUI->UpdateTimer(VehiclePawn->Timer);
	}
}

void ATrackamaniaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// get a pointer to the controlled pawn
	VehiclePawn = CastChecked<ATrackamaniaPawn>(InPawn);
}

void ATrackamaniaPlayerController::Fullreset()
{
	for (ACheckPoint* CheckPoint : CheckPoints)
	{
		CheckPoint->SetActorEnableCollision(true);
	}
}

void ATrackamaniaPlayerController::SaveBestTime(float BestTime)
{
	VehicleUI->UpdateBestTime(BestTime);
}
