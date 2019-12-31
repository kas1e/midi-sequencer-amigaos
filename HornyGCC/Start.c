#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>

#ifdef __amigaos4__
#include <proto/application.h>
#include <proto/wb.h>
#include <libraries/application.h>
#endif

#include <exec/exec.h>
#include <workbench/startup.h>

#include "Versionen.h"
#include "Dynamic_Strings.h"
#include "Strukturen.h"

void haupt(STRPTR startdatei);


#ifdef __amigaos4__
char *ver = "$VER: Horny 1.3 (for OS4 PPC)";
#else
char *ver = "$VER: Horny 1.3 (for OS3 68k)";
#endif


//==================================================================================
// OS 4 Specials
//==================================================================================

#ifdef __amigaos4__

// **** Application Library ****

unsigned long start_appID = 0;
struct MsgPort *start_appPort = NULL;
unsigned long start_appSigMask = 0;

struct Library           *ApplicationBase = NULL;
struct ApplicationIFace  *IApplication    = NULL;

BOOL start_newPhonolithProject = FALSE;
STRPTR start_phonolithProject = NULL;

extern struct UMGEBUNG umgebung;

void openOS4Lib()
{
	ApplicationBase = OpenLibrary("application.library", 50);
	if (ApplicationBase)
	{
		IApplication = (struct ApplicationIFace *) GetInterface(ApplicationBase, "application", 1, NULL);
	}
}

void closeOS4Lib()
{
	DropInterface((struct Interface *) IApplication);
	IApplication = NULL;
	CloseLibrary(ApplicationBase);
	ApplicationBase = NULL;
}

void registerApplication(struct WBStartup *wbstartup)
{
	if (IApplication)
	{
		start_appID = RegisterApplication("Horny",
				REGAPP_URLIdentifier, "inutilis.de",
				REGAPP_WBStartup, wbstartup,
				REGAPP_LoadPrefs, TRUE,
				REGAPP_AppNotifications, TRUE,
				//REGAPP_NoIcon, TRUE,
				TAG_DONE);

		GetApplicationAttrs(start_appID, APPATTR_Port, &start_appPort, TAG_DONE);
		start_appSigMask = 1L << start_appPort->mp_SigBit;
	}
}

void unregisterApplication()
{
	if (IApplication)
	{
		UnregisterApplication(start_appID, NULL);
	}
}

void loadPhonolithProject(STRPTR datei) {
	if (IApplication) {
		ULONG appID = FindApplication(FINDAPP_AppIdentifier, "Phonolith.inutilis.de", TAG_DONE);

		if (appID)
		{
			struct MsgPort *appPort = NULL;
			struct ApplicationOpenPrintDocMsg appMsg;

			GetApplicationAttrs(appID, APPATTR_Port, &appPort, TAG_DONE);

			memset(&appMsg, 0, sizeof(struct ApplicationOpenPrintDocMsg));
			appMsg.fileName = datei;
			if (SendApplicationMsg(start_appID, appID, &appMsg, APPLIBMT_OpenDoc))
			{
				start_newPhonolithProject = FALSE;
			}
		}
	}
}

void newPhonolithProject(STRPTR datei)
{
	if (IApplication) {
		if (datei != NULL && strlen(datei) > 0)
		{
			start_newPhonolithProject = TRUE;
			start_phonolithProject = datei;
			loadPhonolithProject(start_phonolithProject);
			if (start_newPhonolithProject) //not yet loaded
			{
				//start Phonolith
				OpenWorkbenchObject(umgebung.pfadphonolith, TAG_DONE);
			}
		}
	}
}

void checkPhonolithProject()
{
	if (start_newPhonolithProject)
	{
		loadPhonolithProject(start_phonolithProject);
	}
}


// **** AmiUpdate ****

void SetAmiUpdateENVVariable( char *varname )
{
  /* AmiUpdate support code */

  BPTR lock;
  APTR oldwin;

  /* obtain the lock to the home directory */
  if(( lock = GetProgramDir() ))
  {
    TEXT progpath[2048];
    TEXT varpath[1024] = "AppPaths";

    /*
    get a unique name for the lock,
    this call uses device names,
    as there can be multiple volumes
    with the same name on the system
    */

	if( DevNameFromLock( lock, progpath, sizeof(progpath), DN_FULLPATH ))
    {
	  /* stop any "Insert volume..." type requesters */
	  oldwin = SetProcWindow((APTR)-1);

      /*
      finally set the variable to the
      path the executable was run from
      don't forget to supply the variable
      name to suit your application
      */

	  AddPart( varpath, varname, 1024);
	  SetVar( varpath, progpath, -1, GVF_GLOBAL_ONLY|GVF_SAVE_VAR );

      /* turn requesters back on */
	  SetProcWindow( oldwin );
    }
  }
}

#endif

//==========================================================================

void wbmain(struct WBStartup *argmsg) {
	struct WBArg *wbarg;
	STRPTR wbdatei = NULL;
	BPTR lock = NULL;

	#ifdef __amigaos4__
	openOS4Lib();
	SetAmiUpdateENVVariable("Horny");
	registerApplication(argmsg);
	#endif

	
	if (argmsg->sm_NumArgs > 1) {
		wbarg = argmsg->sm_ArgList;
		wbarg++;
		wbdatei = wbarg->wa_Name;
		lock = wbarg->wa_Lock;
		CurrentDir(lock);
	}
	
	haupt(wbdatei);	

	#ifdef __amigaos4__
	unregisterApplication();
	closeOS4Lib();
	#endif
	exit(0);
}

int main(int argc, char *argv[]) {
	#ifdef __amigaos4__
	if (argc == 0)
	{
		struct WBStartup *wbstartup = (struct WBStartup *) argv;
		wbmain(wbstartup);
	}
	else
	{
		SetAmiUpdateENVVariable("Horny");
	}
	#endif
	if (argc > 1) haupt(argv[1]);
	else haupt(NULL);
	exit(0);
}
