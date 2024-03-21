#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainPawn.generated.h"


// The class was created to remove the default class and gain control over the camera.
UCLASS()
class EXPERIMENTPROJECT_API AMainPawn : public APawn
{
	GENERATED_BODY()

public:
	AMainPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
