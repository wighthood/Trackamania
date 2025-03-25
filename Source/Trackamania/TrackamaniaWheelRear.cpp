// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrackamaniaWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UTrackamaniaWheelRear::UTrackamaniaWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}