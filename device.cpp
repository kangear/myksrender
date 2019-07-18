/*
*/

//static char STR_MODULENAME[] = "MyKs2: ";

#include "device.h"

#ifdef ALLOC_PRAGMA
#pragma code_seg("PAGE")
#endif // ALLOC_PRAGMA


/* Device */

// Public methods


// Public static methods

/**
 * Create the XXX device. This is the creation dispatch for the
 * capture device.
 * @param device The AVStream device being created.
 * @return NT status code.
 */
NTSTATUS
Device::DispatchPnpAdd(KSDEVICE *device)
{
    PAGED_CODE();
    DENTER();
    ASSERT(device);

    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    Device *deviceObject = new (NonPagedPool) Device(device);
    if (NULL != deviceObject) {
        status = deviceObject->PnpAdd();
        if (!NT_SUCCESS(status))
            delete deviceObject;
    }

    DLEAVE();
    return status;
}

/**
 * This is the Pnp Start dispatch for XXX device. It simply
 * bridges to PnpStart() in the context of the Device.
 * @param device
 * @param irp
 * @param translatedResourceList
 * @param untranslatedResourceList
 * @return NT status code.
 */
NTSTATUS
Device::DispatchPnpStart(KSDEVICE *device, IRP *irp,
                         CM_RESOURCE_LIST *translatedResourceList,
                         CM_RESOURCE_LIST *untranslatedResourceList)
{
    PAGED_CODE();
    DENTER();
    ASSERT(device);
    ASSERT(device->Context);
    (void)irp;
    (void)translatedResourceList;
    (void)untranslatedResourceList;

    Device *deviceObject = reinterpret_cast<Device *>(device->Context);

    DLEAVE();
    return deviceObject->PnpStart();
}

/**
 * This is the Pnp stop dispatch for the XXX device. It simply
 * bridges to PnpStop() in the context of the Device.
 * @param device
 * @param irp
 */
void
Device::DispatchPnpStop (KSDEVICE *device, IRP *irp)
{
    PAGED_CODE();
    DENTER();
    ASSERT(device);
    ASSERT(device->Context);
    (void)irp;

    Device *deviceObject = reinterpret_cast<Device *>(device->Context);
    deviceObject->PnpStop();

    DLEAVE();
}

// Private methods

/**
 *
 */
Device::Device(KSDEVICE *device)
    : m_ksDevice(device)
{
    PAGED_CODE();
    DENTER();
    //
    DLEAVE();
}

/**
 *
 */
Device::~Device()
{
    PAGED_CODE();
    DENTER();
    //
    DLEAVE();
}

/**
 *
 * @return NT status code.
 */
NTSTATUS
Device::PnpAdd()
{
    PAGED_CODE();
    DENTER();

    ::KsAcquireDevice(m_ksDevice);
    NTSTATUS status = ::KsAddItemToObjectBag(m_ksDevice->Bag,
                                             reinterpret_cast<PVOID>(this),
                                             reinterpret_cast<PFNKSFREE>(Device::Cleanup));
    ::KsReleaseDevice(m_ksDevice);
    if (NT_SUCCESS(status))
        m_ksDevice->Context = reinterpret_cast<PVOID>(this);

    DLEAVE();
    return status;
}

/**
 * This is the Pnp start routine for XXX.
 * @note That DispatchStart bridges to here in the context.
 * @return NT status code.
 */
NTSTATUS
Device::PnpStart()
{
    PAGED_CODE();
    DENTER();

    NTSTATUS status = STATUS_SUCCESS;
    if (!m_ksDevice->Started) {
        ::KsAcquireDevice(m_ksDevice);
        status = ::KsCreateFilterFactory(m_ksDevice->FunctionalDeviceObject,
                                         &ksCaptureFilterDescriptor,
                                         L"PcmRender",
                                         NULL,
                                         KSCREATE_ITEM_FREEONSTOP,
                                         NULL, NULL, NULL);
        ::KsReleaseDevice(m_ksDevice);
    }

    DLEAVE();
    return status;
}

/**
 * This is the pnp stop dispatch for the XXX device. It releases any
 * adapter object previously allocated by IoGetDmaAdapter during Pnp Start.
 */
void
Device::PnpStop()
{
    PAGED_CODE();
    DENTER();
    // Stub
    DLEAVE();
}

// Private static methods

/**
 * This is the free callback for the bagged XXX device. Not providing
 * one will call ExFreePool, which is not what we want for a constructed
 * C++ object. This simply deletes the XXX device.
 * @param deviceObject
 */
void
Device::Cleanup(Device *deviceObject)
{
    PAGED_CODE();
    DENTER();

    delete deviceObject;

    DLEAVE();
}

/* DESCRIPTOR AND DISPATCH LAYOUT */

/**
 * This is the dispatch table for the device. Plug and play
 * notifications as well as power management notifications are dispatched
 * through this table.
 */
static const
KSDEVICE_DISPATCH
ksDeviceDispatch = {
    Device::DispatchPnpAdd,    // Pnp Add Device
    Device::DispatchPnpStart,  // Pnp Start
    NULL,                      // Post-Start
    NULL,                      // Pnp Query Stop
    NULL,                      // Pnp Cancel Stop
    Device::DispatchPnpStop,   // PnP Stop
    NULL,                      // Pnp Query Remove
    NULL,                      // Pnp Cancel Remove
    NULL,                      // Pnp Remove
    NULL,                      // Pnp Query Capabilities
    NULL,                      // Pnp Surprise Removal
    NULL,                      // Power Query Power
    NULL,                      // Power Set Power
    NULL                       // Pnp Query Interface
};

/**
 * This is the device descriptor for the complete device. It points to the
 * dispatch table. No filter descriptors are statically in this descriptor.
 * This driver uses KsCreateFilterFactory to create the filters.
 */
const
KSDEVICE_DESCRIPTOR
ksDeviceDescriptor = {
    &ksDeviceDispatch,
    0,
    NULL,  // the filter descriptor is NULL, use KsCreateFilterFactory to create it dynamically
    KSDEVICE_DESCRIPTOR_VERSION
};


