/*
*/

#ifndef DEVICE_H
#define DEVICE_H

#include "common.h"


class Device
{
public:
    static NTSTATUS DispatchPnpAdd(KSDEVICE *device);
    static NTSTATUS DispatchPnpStart(KSDEVICE *device, IRP *irp,
                                     CM_RESOURCE_LIST *translatedResourceList,
                                     CM_RESOURCE_LIST *untranslatedResourceList);
    static void DispatchPnpStop(KSDEVICE *device, IRP *irp);

private:
    Device(KSDEVICE *device);
    ~Device();

    NTSTATUS PnpAdd();
    NTSTATUS PnpStart();
    void PnpStop();

private:
    static void Cleanup(Device *deviceObject);

private:
    KSDEVICE *m_ksDevice;
};

#endif // DEVICE_H

