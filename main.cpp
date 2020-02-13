#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // SKSEInterface, PluginInfo
#include "skse64/GameForms.h"
#include "skse64/GameReferences.h"
#include "skse64_common/SafeWrite.h"
#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"  // VERSION_VERSTRING, VERSION_MAJOR

uintptr_t BaseHandle = 0;
BYTE* PlayerIsRunning = 0;

bool (*CopyFromAnimGraph)(uintptr_t, float*);

bool (*CopyToBufferIfAnimGraphDoesnotExist)(uintptr_t, float*);


bool  GetSpeedStruct(uintptr_t* Acteur, float* StructToCopy)
{
	//Actor[7] is actually animGraphHolder
	if (Acteur[7] && CopyFromAnimGraph(Acteur[7], StructToCopy))
	{
		Actor* TheActor = reinterpret_cast<Actor*>(Acteur);

		if (!*PlayerIsRunning) {

				StructToCopy[5] = StructToCopy[4] * 1.01;
		}
		else {
			StructToCopy[4] = StructToCopy[5] / 1.01;

		}
			return TRUE;
		
	}
	else
		return CopyToBufferIfAnimGraphDoesnotExist(reinterpret_cast<uintptr_t>(Acteur) - 0xB8, StructToCopy);

}

extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		//BaseHandle = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\WalkFix.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
		
		_MESSAGE("WalkFix SuccessfullyLoaded");
		BaseHandle = reinterpret_cast<uintptr_t> (GetModuleHandle(NULL));
		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "WalkFix";
		a_info->version = 1;

		if (a_skse->isEditor) {
			return false;
		} else if (a_skse->runtimeVersion != RUNTIME_VERSION_1_5_97) {
			_FATALERROR("[FATAL ERROR] Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}
		_MESSAGE("pass query");
		return true;
	}



	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		PlayerIsRunning = reinterpret_cast<BYTE*>(BaseHandle + 0x2F37CD9);
		CopyFromAnimGraph = reinterpret_cast <bool (*)(uintptr_t, float*)> (BaseHandle + 0x65B940);
		CopyToBufferIfAnimGraphDoesnotExist = reinterpret_cast <bool (*)(uintptr_t, float*)> (BaseHandle + 0x6082B0);
		SafeWrite64(BaseHandle + 0x1664128, (uintptr_t)GetSpeedStruct); //Actor State vTable for PlayerCharacter + 0x48. We override the function that gets the anim graph struct for the player ONLY
		_MESSAGE("correct load");
		return true;
	}
};
