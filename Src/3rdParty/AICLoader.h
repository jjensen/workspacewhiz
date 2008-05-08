/***************************************************************************/
/* NOTE:                                                                   */
/* This document is copyright (c) by Oz Solomonovich.  All non-commercial  */
/* use is allowed, as long as this document is not altered in any way, and */
/* due credit is given.                                                    */
/***************************************************************************/

// AddInComm Loader, (AIC 1.3.0 - March 2000)

#ifndef __AICLOADER_H
#define __AICLOADER_H

/* Will load the AIC library.  
   Debug builds try to load AICD.DLL be default, release builds try to load 
   AIC.mod.  To override this behavior use UseDeubgLibrary();
*/
bool aiclLoadAICLibrary(HINSTANCE hInst);

/* Will force the AIC library to unload.  The Use of this function is 
   optional. 
*/
void aiclUnloadAICLibrary();

/* Override default library load:
   TRUE   -  Use debugging version of AIC (AICD.mod)
   FALSE  -  Use release version of AIC (AIC.mod)
*/
void aiclUseDebugLibrary(BOOL bUse);

bool aiclIsAICLoaded();


/* == AICL functionality not related to AddInComm == */
/* (some internal loader function that should be useful to outsiders) */

/* These functions allow you to extract the content of a file's version 
   resource.  You can either specify a file name (aiclGetFileVersion()) or
   a name or a running module (aiclGetModuleVersion()).
*/
void aiclGetFileVersion  (LPCTSTR pszPath, LPSTR out_pszVer);
void aiclGetModuleVersion(LPCTSTR pszMod,  LPSTR out_pszVer);

/* Returns DevStudio's registry key in HKEY_CURRENT_USER (works only when
   DevStudio is running).
*/
void aiclGetDSCurrentUserRegKey(LPSTR out_pszKeyName);

/* Returns DevStudio's add-in directory (empty string returned on error) */
void aiclGetDSAddInDir(LPSTR out_pszPath, DWORD cb);


#endif // __AICLOADER_H
