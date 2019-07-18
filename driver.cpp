/*
*/

#define DEFINE_DEBUG_VARS

//static char STR_MODULENAME[] = "MyKs1: ";

#include "device.h"

#ifdef ALLOC_PRAGMA
#pragma code_seg("PAGE")
#endif // ALLOC_PRAGMA


extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C"
NTSTATUS
DriverEntry(DRIVER_OBJECT *driverObject, UNICODE_STRING *registryPath)
{
    PAGED_CODE();
    DENTER();

    NTSTATUS status = ::KsInitializeDriver(driverObject, registryPath, &ksDeviceDescriptor);
    if (NT_SUCCESS(status)) {
    } else {
    }

    DLEAVE();

    return status;
}
