#ifndef vdversion_h_included
#define vdversion_h_included

#include <vd.h>

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#endif
#endif

EXTERN_C INT VdGetICAVersion(PVD pVd, PUINT pMajorVersion, PUINT pMinorVersion, PUINT pRevision, PUINT pBuildNumber);


#endif // vdversion_h_included