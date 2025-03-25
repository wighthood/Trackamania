// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrackamaniaWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UTrackamaniaWheelFront::UTrackamaniaWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}