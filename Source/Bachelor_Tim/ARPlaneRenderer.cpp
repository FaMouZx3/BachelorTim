//kopiert von Google Sample

#include "ARPlaneRenderer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ARBlueprintLibrary.h"

// Sets default values
AARPlaneRenderer::AARPlaneRenderer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EdgeFeatheringDistance = 10.0f;
	NewPlaneIndex = 0.0f;
}

// Called when the game starts or when spawned
void AARPlaneRenderer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AARPlaneRenderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UARBlueprintLibrary::GetTrackingQuality() == EARTrackingQuality::OrientationAndPosition)
	{
		TArray<UARTrackedGeometry*> AllGeometries = UARBlueprintLibrary::GetAllGeometries();
		for (UARTrackedGeometry* Geometry : AllGeometries)
		{
			if (Geometry->IsA(UARPlaneGeometry::StaticClass()))
			{
				UARPlaneGeometry* PlaneGeometry = Cast<UARPlaneGeometry>(Geometry);
				UpdatePlane(PlaneGeometry);
			}
		}
	}
}

//Updated die bereits getrackten Planes sowie die neu getrackten und lässt dann das PlaneMesh darauf Rendern um eine Visuelle Unterstützung
//für die Benutzer zu schaffen
void AARPlaneRenderer::UpdatePlane(UARPlaneGeometry* ARCorePlaneObject)
{
	UProceduralMeshComponent* PlanePolygonMeshComponent = nullptr;
	if (!PlaneMeshMap.Contains(ARCorePlaneObject))
	{
		if (ARCorePlaneObject->GetSubsumedBy() != nullptr || ARCorePlaneObject->GetTrackingState() == EARTrackingState::StoppedTracking)
		{
			return;
		}

		PlanePolygonMeshComponent = NewObject<UProceduralMeshComponent>(this);
		PlanePolygonMeshComponent->RegisterComponent();
		PlanePolygonMeshComponent->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(PlaneMaterial, this);
		FColor Color = FColor::White;
		if (PlaneColors.Num() != 0)
		{
			int ColorIndex = NewPlaneIndex % PlaneColors.Num();
			Color = PlaneColors[ColorIndex];
		}
		DynMaterial->SetScalarParameterValue(FName(TEXT("TextureRotationAngle")), FMath::FRandRange(0.0f, 1.0f));
		DynMaterial->SetVectorParameterValue(FName(TEXT("PlaneTint")), FLinearColor(Color));

		PlanePolygonMeshComponent->SetMaterial(0, DynMaterial);
		PlaneMeshMap.Add(ARCorePlaneObject, PlanePolygonMeshComponent);
		NewPlaneIndex++;
	}
	else
	{
		PlanePolygonMeshComponent = *PlaneMeshMap.Find(ARCorePlaneObject);
	}

	if (ARCorePlaneObject->GetTrackingState() == EARTrackingState::Tracking &&
		ARCorePlaneObject->GetSubsumedBy() == nullptr)
	{
		if (!PlanePolygonMeshComponent->bVisible)
		{
			PlanePolygonMeshComponent->SetVisibility(true, true);
		}
		UpdatePlaneMesh(ARCorePlaneObject, PlanePolygonMeshComponent);
	}
	else if (PlanePolygonMeshComponent->bVisible)
	{
		PlanePolygonMeshComponent->SetVisibility(false, true);
	}

	if (ARCorePlaneObject->GetSubsumedBy() != nullptr || ARCorePlaneObject->GetTrackingState() == EARTrackingState::StoppedTracking)
	{
		PlanePolygonMeshComponent = *PlaneMeshMap.Find(ARCorePlaneObject);
		if (PlanePolygonMeshComponent != nullptr)
		{
			PlanePolygonMeshComponent->DestroyComponent(true);
			PlaneMeshMap.Remove(ARCorePlaneObject);
		}
	}
}

//Zeichnet das Plane Mesh auf die getrackte Fläche
void AARPlaneRenderer::UpdatePlaneMesh(UARPlaneGeometry* ARCorePlaneObject, UProceduralMeshComponent* PlanePolygonMeshComponent)
{
	// Update polygon mesh vertex indices, using triangle fan due to its convex.
	TArray<FVector> BoundaryVertices;
	BoundaryVertices = ARCorePlaneObject->GetBoundaryPolygonInLocalSpace();
	int BoundaryVerticesNum = BoundaryVertices.Num();

	if (BoundaryVerticesNum < 3)
	{
		PlanePolygonMeshComponent->ClearMeshSection(0);
		return;
	}

	int PolygonMeshVerticesNum = BoundaryVerticesNum * 2;
	// Triangle number is interior(n-2 for convex polygon) plus perimeter (EdgeNum * 2);
	int TriangleNum = BoundaryVerticesNum - 2 + BoundaryVerticesNum * 2;

	TArray<FVector> PolygonMeshVertices;
	TArray<FLinearColor> PolygonMeshVertexColors;
	TArray<int> PolygonMeshIndices;
	TArray<FVector> PolygonMeshNormals;

	PolygonMeshVertices.Empty(PolygonMeshVerticesNum);
	PolygonMeshVertexColors.Empty(PolygonMeshVerticesNum);
	PolygonMeshIndices.Empty(TriangleNum * 3);
	PolygonMeshNormals.Empty(PolygonMeshVerticesNum);

	FVector PlaneNormal = ARCorePlaneObject->GetLocalToWorldTransform().GetRotation().GetUpVector();
	for (int i = 0; i < BoundaryVerticesNum; i++)
	{
		FVector BoundaryPoint = BoundaryVertices[i];
		float BoundaryToCenterDist = BoundaryPoint.Size();
		float FeatheringDist = FMath::Min(BoundaryToCenterDist, EdgeFeatheringDistance);
		FVector InteriorPoint = BoundaryPoint - BoundaryPoint.GetUnsafeNormal() * FeatheringDist;

		PolygonMeshVertices.Add(BoundaryPoint);
		PolygonMeshVertices.Add(InteriorPoint);

		PolygonMeshNormals.Add(PlaneNormal);
		PolygonMeshNormals.Add(PlaneNormal);

		PolygonMeshVertexColors.Add(FLinearColor(0.0f, 0.f, 0.f, 0.f));
		PolygonMeshVertexColors.Add(FLinearColor(0.0f, 0.f, 0.f, 1.f));
	}

	// Generate triangle indices

	// Perimeter triangles
	for (int i = 0; i < BoundaryVerticesNum - 1; i++)
	{
		PolygonMeshIndices.Add(i * 2);
		PolygonMeshIndices.Add(i * 2 + 2);
		PolygonMeshIndices.Add(i * 2 + 1);

		PolygonMeshIndices.Add(i * 2 + 1);
		PolygonMeshIndices.Add(i * 2 + 2);
		PolygonMeshIndices.Add(i * 2 + 3);
	}

	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2);
	PolygonMeshIndices.Add(0);
	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2 + 1);


	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2 + 1);
	PolygonMeshIndices.Add(0);
	PolygonMeshIndices.Add(1);

	// interior triangles
	for (int i = 3; i < PolygonMeshVerticesNum - 1; i += 2)
	{
		PolygonMeshIndices.Add(1);
		PolygonMeshIndices.Add(i);
		PolygonMeshIndices.Add(i + 2);
	}

	// No need to fill uv and tangent;
	PlanePolygonMeshComponent->CreateMeshSection_LinearColor(0, PolygonMeshVertices, PolygonMeshIndices, PolygonMeshNormals, TArray<FVector2D>(), PolygonMeshVertexColors, TArray<FProcMeshTangent>(), false);

	// Set the component transform to Plane's transform.
	PlanePolygonMeshComponent->SetWorldTransform(ARCorePlaneObject->GetLocalToWorldTransform());
}

