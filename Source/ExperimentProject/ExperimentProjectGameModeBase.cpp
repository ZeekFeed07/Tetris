// Copyright Epic Games, Inc. All Rights Reserved.


#include "ExperimentProjectGameModeBase.h"
#include "MainManagerController.h"
#include "BaseGeometry.h"

AExperimentProjectGameModeBase::AExperimentProjectGameModeBase()
{
	//DefaultPawnClass = ABaseGeometry::StaticClass();
	PlayerControllerClass = AMainManagerController::StaticClass();
}