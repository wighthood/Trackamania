// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrackamaniaPawn.h"
#include "TrackamaniaWheelFront.h"
#include "TrackamaniaWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "TrackamaniaPlayerController.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);


ATrackamaniaPawn::ATrackamaniaPawn()
{
	// construct the front camera boom
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

}

void ATrackamaniaPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ATrackamaniaPawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ATrackamaniaPawn::Throttle);

		// break 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ATrackamaniaPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ATrackamaniaPawn::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ATrackamaniaPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ATrackamaniaPawn::StopHandbrake);

		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::LookAround);

		// toggle camera 
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::ToggleCamera);

		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::ResetVehicle);

		//	rotation control in air
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::Jump);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::Roll);
		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::Pitch);	
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::Yaw);

		EnhancedInputComponent->BindAction(FullResetAction, ETriggerEvent::Triggered, this, &ATrackamaniaPawn::FullReset);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATrackamaniaPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// realign the camera yaw to face front
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
	Timer += Delta;
}

void ATrackamaniaPawn::Steering(const FInputActionValue& Value)
{
	// get the input magnitude for steering
	float SteeringValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ATrackamaniaPawn::Throttle(const FInputActionValue& Value)
{
	// get the input magnitude for the throttle
	float ThrottleValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

void ATrackamaniaPawn::Brake(const FInputActionValue& Value)
{
	// get the input magnitude for the brakes
	float BreakValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetBrakeInput(BreakValue);
}

void ATrackamaniaPawn::StartBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ATrackamaniaPawn::StopBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ATrackamaniaPawn::StartHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ATrackamaniaPawn::StopHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	BrakeLights(false);
}

void ATrackamaniaPawn::LookAround(const FInputActionValue& Value)
{
	// get the flat angle value for the input 
	float LookValue = Value.Get<float>();

	// add the input
	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));
}

void ATrackamaniaPawn::ToggleCamera(const FInputActionValue& Value)
{
	// toggle the active camera flag
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

bool ATrackamaniaPawn::Grounded()
{
	int grounded = 0;
	for (UChaosVehicleWheel* wheel : ChaosVehicleMovement->Wheels)
	{
		if (!wheel->IsInAir())
			grounded++;
	}
	if (grounded >= 3)
	{
		return true;
	}
	return false;
}

void ATrackamaniaPawn::Roll(const FInputActionValue& Value)
{
	if (!Grounded())
	{
		AddActorLocalRotation(FRotator(0, 0, Value.Get<float>())*2, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ATrackamaniaPawn::Jump()
{
	if (Grounded())
	{
		FVector Jumpvelocity = GetVelocity() + GetActorUpVector() * 1000;
		GetMesh()->SetPhysicsLinearVelocity(Jumpvelocity);
	}
}

void ATrackamaniaPawn::Pitch(const FInputActionValue& Value)
{
	if (!Grounded())
	{
		AddActorLocalRotation(FRotator(Value.Get<float>()*1.5, 0, 0), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ATrackamaniaPawn::Yaw(const FInputActionValue& Value)
{
	if (!Grounded())
	{
		AddActorLocalRotation(FRotator(0, Value.Get<float>()*1.5, 0), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ATrackamaniaPawn::Respawn()
{
	FVector ResetLocation;
	FRotator ResetRotation;
	if (RespawnPoint != nullptr)
	{
		// reset to a location slightly above our current one
		ResetLocation = RespawnPoint->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

		// reset to our yaw. Ignore pitch and roll
		ResetRotation = RespawnPoint->GetActorRotation();
	}
	else
	{
		// reset to a location slightly above our current one
		ResetLocation = FVector(0.0f, 0.0f, 50.0f);

		// reset to our yaw. Ignore pitch and roll
		ResetRotation = FRotator(0.0f, 0.0f, 0.0f);
	}
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);
	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

void ATrackamaniaPawn::SetRespawn(AActor* Checkpoint)
{
	RespawnPoint = Checkpoint;
	Checkpoint->SetActorEnableCollision(false);
}

void ATrackamaniaPawn::ResetVehicle(const FInputActionValue& Value)
{
	Respawn();
}

void ATrackamaniaPawn::SaveBestTime()
{
	if (BestTime == NULL || Timer < BestTime)
	{
		BestTime = Timer;
		if (Controller)
		{
			Cast<ATrackamaniaPlayerController>(Controller)->SaveBestTime(BestTime);
		}
	}
}

void ATrackamaniaPawn::FullReset()
{
	Timer = 0;
	RespawnPoint = nullptr;
	Respawn();
	if (Controller)
	{
		Cast<ATrackamaniaPlayerController>(Controller)->Fullreset();
	}
}

#undef LOCTEXT_NAMESPACE