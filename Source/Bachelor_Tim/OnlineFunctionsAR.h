//kopiert von Google Sample

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FindSessionsCallbackProxy.h"
#include "OnlineFunctionsAR.generated.h"

/**
 * 
 */
UCLASS()
class BACHELOR_TIM_API UOnlineFunctionsAR : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "UOnlineFunctionsAR")
		static FString GetLocalHostIPAddress();

	UFUNCTION(BlueprintPure, Category = "UOnlineFunctionsAR")
		static FString GetSessionId(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "UOnlineFunctionsAR", meta = (WorldContext = "WorldContextObject"))
		static FString GetHostSessionId(UObject* WorldContextObject);
	
	
};
