// Copyright Epic Games, Inc. All Rights Reserved.


#include "ExperimentProjectGameModeBase.h"
#include "MainManagerController.h"
#include "MainPawn.h"
#include "BaseGeometry.h"

AExperimentProjectGameModeBase::AExperimentProjectGameModeBase()
{
	DefaultPawnClass = AMainPawn::StaticClass();
	PlayerControllerClass = AMainManagerController::StaticClass();
}