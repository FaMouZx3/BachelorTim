//kopiert von Google Sample

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARSystem.h"
#include "ARPointCloudRenderer.generated.h"

UCLASS()
class BACHELOR_TIM_API AARPointCloudRenderer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AARPointCloudRenderer();

	/** The color of the point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GoogleARCore|PointCloudRenderer")
		FColor PointColor;

	/** The size of the point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GoogleARCore|PointCloudRenderer")
		float PointSize;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void RenderPointCloud();

	TSharedPtr<FARSystemBase, ESPMode::ThreadSafe> ARSystem;
};
