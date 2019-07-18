/*
*/

#ifndef CAPTURE_H
#define CAPTURE_H

#include "common.h"


class Device;

/**
 * @class CapturePin
 *
 * The Audio capture pin class.
 */
class CapturePin
{
public:


public:
    static NTSTATUS DispatchCreate(KSPIN *pin, IRP *irp);
    static NTSTATUS DispatchSetState(KSPIN *pin, KSSTATE to, KSSTATE from);
    static NTSTATUS DispatchSetFormat(KSPIN *pin,
                                      KSDATAFORMAT *oldFormat, KSMULTIPLE_ITEM *oldAttributeList,
                                      const KSDATARANGE *dataRange, const KSATTRIBUTE_LIST *attributeRange);
    static NTSTATUS DispatchProcess(KSPIN *pin);
    static NTSTATUS IntersectHandler(KSFILTER *filter, IRP *irp, KSP_PIN *pinInstance,
                                     KSDATARANGE *callerDataRange, KSDATARANGE *descriptorDataRange,
                                     ULONG bufferSize, VOID *data, ULONG *dataSize);

private:
    CapturePin (KSPIN *pin);
    ~CapturePin();

    NTSTATUS Create();
    NTSTATUS SetState(KSSTATE to, KSSTATE from);
    NTSTATUS Process();
    NTSTATUS CleanupReferences();

private:
    static void Cleanup(CapturePin *pin);

private:
    KSPIN *m_ksPin;   /*!< The AVStream pin we're associated with. */
    Device *m_deviceObject; /*!< Pointer to the internal device object for our capture device.
                           We access the "fake" hardware through this object. */
};

#endif // CAPTURE_H
