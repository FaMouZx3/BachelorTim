//kopiert von Google Sample

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "ARTrackable.h"

#include "ARPlaneRenderer.generated.h"

UCLASS()
class BACHELOR_TIM_API AARPlaneRenderer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AARPlaneRenderer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** The feathering distance for the polygon edge. Default to 10 cm*/
	UPROPERTY(Category = ARPlaneRenderer, EditAnywhere, BlueprintReadWrite)
		float EdgeFeatheringDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* PlaneMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FColor> PlaneColors;


private:
	void UpdatePlane(UARPlaneGeometry* ARCorePlaneObject);
	void UpdatePlaneMesh(UARPlaneGeometry* ARCorePlaneObject, UProceduralMeshComponent* PlanePolygonMeshComponent);

	UPROPERTY()
		TMap<UARPlaneGeometry*, UProceduralMeshComponent*> PlaneMeshMap;

	int NewPlaneIndex;
};
