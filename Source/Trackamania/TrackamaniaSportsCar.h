// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TrackamaniaPawn.h"
#include "TrackamaniaSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class TRACKAMANIA_API ATrackamaniaSportsCar : public ATrackamaniaPawn
{
	GENERATED_BODY()
	
public:

	ATrackamaniaSportsCar();
};
