#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chaos/Pair.h"

#include "BaseGeometry.generated.h"

UCLASS()
class EXPERIMENTPROJECT_API ABaseGeometry : public AActor
{
	GENERATED_BODY()

public:	
	ABaseGeometry();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual Chaos::Pair<int32, int32> GetYZCoord();
	void SetMainMaterial(UMaterialInterface* Material);
	void SetPivot(const bool Pivot) { IsPivot = Pivot; }
	bool GetPivot() const { return IsPivot; }

private:
	bool IsPivot = false; 
public:
	UPROPERTY(BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* MainMesh;

	static constexpr float Size = 100.f;
	static constexpr float Points = 5.f;
protected:

	UMaterialInterface* _MainMaterial;
};
