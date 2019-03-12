//kopiert von Google Sample

#include "OnlineFunctionsAR.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "OnlineSubsystemUtils.h"
#include "Engine.h"

//Holt sich die locale IP des Gerätes im Netzwerk (127.0.0.1 im Handynetz / Normale vergebene IP im WLAN/Lan Netz)
FString UOnlineFunctionsAR::GetLocalHostIPAddress()
{
	bool bCanBindAll;
	TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
	if (LocalIp->IsValid())
	{
		return LocalIp->ToString(false);
	}
	return FString("");
}

//Holt sich die aktuelle Session ID
FString UOnlineFunctionsAR::GetSessionId(const FBlueprintSessionResult& Result)
{
	return Result.OnlineResult.Session.OwningUserName.Right(5);
}

//Sobald eine Session gestartet wurde kann die Session ID hier geholt werden
FString UOnlineFunctionsAR::GetHostSessionId(UObject* WorldContextObject)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull));
	auto Sessions = OnlineSubsystem->GetSessionInterface();
	auto Session = Sessions->GetNamedSession(NAME_GameSession);
	if (Session != nullptr)
	{
		return Session->OwningUserName.Right(5);
	}

	return "";
}

