/*
*/

#include "capture.h"
#include "device.h"

#ifdef ALLOC_PRAGMA
#pragma code_seg("PAGE")
#endif // ALLOC_PRAGMA


/* CapturePin */

// Public methods



// Public static methods

/**
 * Create a new capture pin. This is the creation dispatch for
 * the audio capture pin.
 * @param pin The pin being created
 * @param irp The creation Irp
 * @return NT status code.
 */
NTSTATUS
CapturePin::DispatchCreate(KSPIN *pin, IRP *irp)
{
    PAGED_CODE();
    DENTER();
    ASSERT(pin);
    ASSERT(pin->Context);
    (void)irp;

    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    CapturePin *pinObject = new (NonPagedPool) CapturePin(pin);
    if (NULL != pinObject) {
        status = pinObject->Create();
        if (!NT_SUCCESS(status)) {
            delete pinObject;
        }
    }

    //
    DLEAVE();
    return status;
}

/**
 * This is the set device state dispatch for the pin. The routine bridges
 * to SetState() in the context of the CapturePin.
 * @param pin The pin..
 * @param to
 * @param from
 * @return NT status code.
 */
NTSTATUS
CapturePin::DispatchSetState(KSPIN *pin, KSSTATE to, KSSTATE from)
{
    PAGED_CODE();
    DENTER();
    ASSERT(pin);
    ASSERT(pin->Context);

    CapturePin *pinObject = reinterpret_cast<CapturePin *>(pin->Context);

    DLEAVE();
    return pinObject->SetState(to, from);
}

/**
 * This is the set data format dispatch for the capture pin. It is called
 * in two circumstances:
 * # before Pin's creation dispatch has been made to verify that
 * pin->ConnectionFormat is an acceptable format for the range
 * DataRange. In this case oldKsFormat is NULL.
 * # after Pin's creation dispatch has been made and an initial format
 * selected in order to change the format for the pin. In this case,
 * oldKsFormat will not be NULL.
 * Validate that the format is acceptible and perform the actions necessary
 * to change format if appropriate.
 * @param pin The pin this format is being set on. The format itself will be in
 * pin->ConnectionFormat.
 * @param oldFormat The previous format used on this pin. If this is NULL, it is an
 * indication that Pin's creation dispatch has not yet been made and
 * that this is a request to validate the initial format and not to
 * change formats.
 * @param oldAttributeList The old attribute list for the prior format
 * @param dataRange A range out of our list of data ranges which was determined to be
 * at least a partial match for pin->ConnectionFormat. If the format
 * there is unacceptable for the range, STATUS_NO_MATCH should be returned.
 * @param attributeRange The attribute range.
 * @return NT status code.
 */
NTSTATUS
CapturePin::DispatchSetFormat(KSPIN *pin,
                              KSDATAFORMAT *oldFormat, KSMULTIPLE_ITEM *oldAttributeList,
                              const KSDATARANGE *dataRange, const KSATTRIBUTE_LIST *attributeRange)
{
    PAGED_CODE();
    DENTER();
    ASSERT(pin);
    ASSERT(pin->Context);

    (void)pin;
    (void)oldFormat;
    (void)oldAttributeList;
    (void)dataRange;
    (void)attributeRange;

    NTSTATUS status = STATUS_SUCCESS;

    DLEAVE();
    return status;
}

/**
 * This is the processing dispatch for the capture pin.  The routine
 * bridges to Process() in the context of the CapturePin.
 * @param pin
 * @return NT status code.
 */
NTSTATUS
CapturePin::DispatchProcess(KSPIN *pin)
{
    PAGED_CODE();
    DENTER();
    ASSERT(pin);
    ASSERT(pin->Context);

    CapturePin *pinObject = reinterpret_cast<CapturePin *>(pin->Context);

    DLEAVE();
    return pinObject->Process();
}

/**
 * This routine handles audio pin intersection queries by determining the
 * intersection between two data ranges.
 * @param filter Contains a void pointer to the  filter structure.
 * @param irp Contains a pointer to the data intersection property request.
 * @param pinInstance Contains a pointer to a structure indicating the pin in question.
 * @param callerDataRange Contains a pointer to one of the data ranges supplied by the client
 * in the data intersection request. The format type, subtype and
 * specifier are compatible with the DescriptorDataRange.
 * @param descriptorDataRange Contains a pointer to one of the data ranges from the pin descriptor
 * for the pin in question. The format type, subtype and specifier are
 * compatible with the CallerDataRange.
 * @param bufferSize Contains the size in bytes of the buffer pointed to by the Data
 * argument. For size queries, this value will be zero.
 * @param data Optionally contains a pointer to the buffer to contain the data
 * format structure representing the best format in the intersection
 * of the two data ranges.  For size queries, this pointer will be NULL.
 * @param dataSize Contains a pointer to the location at which to deposit the size
 * of the data format. This information is supplied by the function
 * when the format is actually delivered and in response to size queries.
 * @return NT status code.
 */
NTSTATUS
CapturePin::IntersectHandler(KSFILTER *filter, IRP *irp, KSP_PIN *pinInstance,
                             KSDATARANGE *callerDataRange, KSDATARANGE *descriptorDataRange,
                             ULONG bufferSize, VOID *data, ULONG *dataSize)
{
    PAGED_CODE();
    DENTER();
    ASSERT(filter);
    ASSERT(irp);
    ASSERT(pinInstance);
    ASSERT(callerDataRange);
    ASSERT(descriptorDataRange);
    ASSERT(dataSize);
    (void)bufferSize;
    (void)data;

    NTSTATUS status = STATUS_SUCCESS;

    DLEAVE();
    return status;
}

// Private methods

/**
 * The capture pin's constructor.  Initialize any non-0, non-NULL fields
 * (since new will have zero'ed the memory anyway) and set up our
 * device level pointers for access during capture routines.
 * @param pin The AVStream pin object corresponding to the capture pin.
 */
CapturePin::CapturePin(KSPIN *pin)
    : m_ksPin(pin)
{
    PAGED_CODE();
    DENTER();

    KSDEVICE *device = ::KsPinGetDevice(pin);
    // Set up our device pointer.  This gives us access to "hardware I/O"
    // during the capture routines.
    m_deviceObject = reinterpret_cast<Device *> (device->Context);

    DLEAVE();
}

CapturePin::~CapturePin()
{
    PAGED_CODE();
    DENTER();
    //
    DLEAVE();
}

NTSTATUS
CapturePin::Create()
{
    PAGED_CODE();
    DENTER();

    NTSTATUS status = ::KsAddItemToObjectBag(m_ksPin->Bag,
                                             reinterpret_cast<PVOID>(this),
                                             reinterpret_cast<PFNKSFREE>(CapturePin::Cleanup));
    if (NT_SUCCESS(status))
        m_ksPin->Context = reinterpret_cast<PVOID>(this);

    DLEAVE();
    return status;
}

/**
 * This is called when the caputre pin transitions state. The routine
 * attempts to acquire/release any hardware resources and start up
 * or shut down capture based on the states we are transitioning to
 * and away from.
 * @param to The state we're transitioning to.
 * @param from The state we're transitioning away from.
 * @return NT status code.
 */
NTSTATUS
CapturePin::SetState(KSSTATE to, KSSTATE from)
{
    PAGED_CODE();
    DENTER();

    (void)to;
    (void)from;

    NTSTATUS status = STATUS_SUCCESS;

    DLEAVE();
    return status;
}

/**
 * The process dispatch for the pin bridges to this location.
 * We handle setting up scatter gather mappings, etc...
 * @return NT status code.
 */
NTSTATUS
CapturePin::Process()
{
    PAGED_CODE();
    DENTER();

    NTSTATUS status = STATUS_SUCCESS;

    DLEAVE();
    return status;
}

/**
 * Clean up any references we're holding on frames after we abruptly
 * stop the hardware.
 * @return NT status code.
 */
NTSTATUS
CapturePin::CleanupReferences()
{
    PAGED_CODE();
    DENTER();

    KSSTREAM_POINTER *clone = ::KsPinGetFirstCloneStreamPointer(m_ksPin);
    KSSTREAM_POINTER *nextClone = NULL;
    // Walk through the clones, deleting them, and setting DataUsed to
    // zero since we didn't use any data!
    while (NULL != clone) {

        nextClone = ::KsStreamPointerGetNextClone(clone);
        clone->StreamHeader->DataUsed = 0;
        ::KsStreamPointerDelete(clone);
        clone = nextClone;
    }

    NTSTATUS status = STATUS_SUCCESS;

    DLEAVE();
    return status;
}

// Private static methods

/**
 * This is the free callback from the bagged item (CapturePin). If we
 * do not provide a callback when we bag the CapturePin, ExFreePool
 * would be called. This is not desirable for C++ constructed objects.
 * We merely delete the object here.
 */
void
CapturePin::Cleanup(CapturePin *pin)
{
    PAGED_CODE();
    DENTER();

    delete pin;

    DLEAVE();
}

/* DESCRIPTOR AND DISPATCH LAYOUT */

/**
 * This is the dispatch table for the capture pin. It provides notifications
 * about creation, closure, processing, data formats, etc...
 */
const
KSPIN_DISPATCH
ksCapturePinDispatch = {
    CapturePin::DispatchCreate,            // Pin Create
    NULL,                                  // Pin Close
    CapturePin::DispatchProcess,           // Pin Process
    NULL,                                  // Pin Reset
    CapturePin::DispatchSetFormat,         // Pin Set Data Format
    CapturePin::DispatchSetState,          // Pin Set Device State
    NULL,                                  // Pin Connect
    NULL,                                  // Pin Disconnect
    NULL,                                  // Clock Dispatch
    NULL                                   // Allocator Dispatch
};

/**
 *
 */
static
const
KSDATARANGE_AUDIO
ksAudioStreamFormat = {

    { // KSDATARANGE
      sizeof (KSDATARANGE_AUDIO),             // FormatSize
      0,                                      // Flags
      4,                                      // SampleSize
      0,                                      // Reserved
      STATIC_KSDATAFORMAT_TYPE_AUDIO,             // Media type
      STATIC_KSDATAFORMAT_SUBTYPE_PCM,            // Media subtype
      STATIC_KSDATAFORMAT_SPECIFIER_WAVEFORMATEX, // Media format specifier
    },

    2,                                      // MaximumChannels
    16,                                     // MinimumBitsPerSample
    32,                                     // MaximumBitsPerSample
    48000,                                  // MinimumSampleFrequency
    48000                                   // MaximumSampleFrequency

};

/**
 *
 */
const
PKSDATARANGE
ksAudioPinDataRanges[1] = {
    PKSDATARANGE(&ksAudioStreamFormat)
};

/**
 * A default framing for the audio pin. In order for this to work properly,
 * the frame size must be at least 1/fps * bytes_per_sec large. Otherwise,
 * the audio stream will fall behind. This is dynamically adjusted when
 * the actual pin is created.
 */
DECLARE_SIMPLE_FRAMING_EX (
        ksAudioPinAllocatorFraming,                  //name
        STATICGUIDOF(KSMEMORY_TYPE_KERNEL_NONPAGED),   //memory type
        KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY | KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY, //flags
        25,             //frames
        0,              //alignment
        2 * PAGE_SIZE,  //min frame size
        2 * PAGE_SIZE   //max frame size
        );















