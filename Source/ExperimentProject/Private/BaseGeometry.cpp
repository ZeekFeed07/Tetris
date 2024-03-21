#include "BaseGeometry.h"
#include "MainManager.h"

ABaseGeometry::ABaseGeometry()
{
 	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main"));
	SetRootComponent(MainMesh);
}

void ABaseGeometry::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseGeometry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Returns[X, Y] coordinates in the _FieldOfGeometry 
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