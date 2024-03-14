#include "BaseGeometry.h"
#include "MainManager.h"

ABaseGeometry::ABaseGeometry()
{
 	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main"));
	SetRootComponent(MainMesh);

	/*ConstructorHelpers::FObjectFinder<UStaticMesh> MeshCube(TEXT("/Script/Engine.StaticMesh'/Game/Meshes/BaseCube.BaseCube'"));
	if (MeshCube.Succeeded())
	{
		MainMesh->SetStaticMesh(MeshCube.Object);
	}*/
}

void ABaseGeometry::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseGeometry::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AMainManager* OwnerManager = Cast<AMainManager>(GetOwner());
	if (OwnerManager)
	{
		OwnerManager->IncreaseScore(10);
	}
	else
	{
		UE_LOG(LogMainManager, Display, TEXT("ABaseGeometry cannot be deleted. Invalid pointer. (ABaseGeometry.cpp | ABaseGeometry::EndPlay)"))
	}
}

void ABaseGeometry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Вовзвращает [X, Y] координаты в массиве _FieldOfGeometry
Chaos::Pair<int32, int32> ABaseGeometry::GetYZCoord()
{
	FVector Coord = GetActorLocation();
	return Chaos::Pair<int32, int32>(Coord.Y / Size, Coord.Z / Size);
}

void ABaseGeometry::SetMainMaterial(UMaterialInterface* Material)
{
	_MainMaterial = Material;
	MainMesh->SetMaterial(0, _MainMaterial);
}