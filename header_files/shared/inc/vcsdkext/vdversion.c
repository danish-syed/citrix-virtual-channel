
#include <citrix.h> // includes windows.h automatically
#include <clterr.h>

#include "vdversion.h"


// internal helper function
VOID VdGetEngineProcessName(PCHAR* path, PUINT32 copied) {

	UINT32 len = 0;

	do {
		len += MAX_PATH;
		*path = realloc(*path, len);
		*copied = GetModuleFileName(NULL, *path, len);
	} while (*copied >= len);
}

INT VdGetICAVersion(PVD pVd, PUINT pMajorVersion, PUINT pMinorVersion, PUINT pRevision, PUINT pBuildNumber) {

	PCHAR filename = NULL;
	UINT32 filenameLength = 0;
	VdGetEngineProcessName(&filename, &filenameLength);
	if (filenameLength > 0) {

		DWORD versionSize = GetFileVersionInfoSize(filename, NULL);
		if (versionSize != 0) {

			LPSTR lpData = malloc(versionSize);
			if (lpData != NULL && GetFileVersionInfo(filename, 0, versionSize, lpData)) {

				UINT len = 0;
				VS_FIXEDFILEINFO* info = NULL;
				if (VerQueryValue(lpData, "\\", &info, &len) && len != 0) {
					if (pMajorVersion) *pMajorVersion = HIWORD(info->dwFileVersionMS);
					if (pMinorVersion) *pMinorVersion = LOWORD(info->dwFileVersionMS);
					if (pRevision) *pRevision = HIWORD(info->dwFileVersionLS);
					if (pBuildNumber) *pBuildNumber = LOWORD(info->dwFileVersionLS);

					return CLIENT_STATUS_SUCCESS;
				}

				free(lpData);
			}
		}

		free(filename);
	}

	return CLIENT_ERROR;
}