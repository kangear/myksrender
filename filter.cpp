/*
*/

#include "filter.h"
#include "capture.h"

#ifdef ALLOC_PRAGMA
#pragma code_seg("PAGE")
#endif // ALLOC_PRAGMA


/* CaptureFilter */

// Public methods


// Public static methods

/**
 * This is the filter creation dispatch for the capture filter. It
 * creates the CaptureFilter object, associates it with the AVStream
 * object, and bags it for easy cleanup later.
 * @param filter The AVStream filter being created
 * @param irp The creation Irp
 * @return NT status code.
 */
NTSTATUS
CaptureFilter::DispatchCreate(KSFILTER *filter, IRP *irp)
{
    PAGED_CODE();
    DENTER();
    ASSERT(filter);
    (void)irp;

    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    CaptureFilter *filterObject = new (NonPagedPool) CaptureFilter(filter);
    if (NULL != filterObject) {
        status = filterObject->Create();
        if (!NT_SUCCESS(status))
            delete filterObject;
    }

    DLEAVE();
    return status;
}

// Private methods

/**
 * The capture filter object constructor.  Since the new operator will
 * have zeroed the memory, do not bother initializing any NULL or 0
 * fields. Only initialize non-NULL, non-0 fields.
 */
CaptureFilter::CaptureFilter(KSFILTER *filter)
    : m_ksFilter(filter)
{
    PAGED_CODE();
    DENTER();
    //
    DLEAVE();
}

CaptureFilter::~CaptureFilter()
{
    PAGED_CODE();
    DENTER();
    //
    DLEAVE();
}

NTSTATUS
CaptureFilter::Create()
{
    PAGED_CODE();
    DENTER();

    NTSTATUS status = ::KsAddItemToObjectBag(m_ksFilter->Bag,
                                             reinterpret_cast<PVOID>(this),
                                             reinterpret_cast<PFNKSFREE>(CaptureFilter::Cleanup));
    if (NT_SUCCESS(status))
        m_ksFilter->Context = reinterpret_cast<PVOID>(this);

    DLEAVE();
    return status;
}

// Private static methods

/**
 * This is the bag cleanup callback for the CaptureFilter. Not providing
 * one would cause ExFreePool to be used. This is not good for C++
 * constructed objects. We simply delete the object here.
 */
void
CaptureFilter::Cleanup(CaptureFilter *filterObject)
{
    PAGED_CODE();
    DENTER();

    delete filterObject;

    DLEAVE();
}

/* DESCRIPTOR AND DISPATCH LAYOUT */

/**
 *
 */
static
const
GUID
guidCaptureFilterCategories[] = {
    STATICGUIDOF(KSCATEGORY_AUDIO),
    STATICGUIDOF(KSCATEGORY_RENDER)
};

/**
 * The list of pin descriptors on the capture filter.
 */
static
const
KSPIN_DESCRIPTOR_EX
ksCaptureFilterPinDescriptors[] = {

    { //Audio Input Pin
      &ksCapturePinDispatch,                    // Dispatch
      NULL,                                     // AutomationTable
      { // PinDescriptor
        DEFINE_KSPIN_DEFAULT_INTERFACES,        // Interfaces
        DEFINE_KSPIN_DEFAULT_MEDIUMS,           // Mediums
        SIZEOF_ARRAY(ksAudioPinDataRanges),     // Range Count
        ksAudioPinDataRanges,                   // Ranges
        KSPIN_DATAFLOW_IN,                     // Dataflow
        KSPIN_COMMUNICATION_BOTH,               // Communication (both source and sink)
        &KSCATEGORY_AUDIO,                      // Category
        NULL, // Name
        0                                       // Reserved
      },
      KSPIN_FLAG_DISPATCH_LEVEL_PROCESSING | KSPIN_FLAG_PROCESS_IN_RUN_STATE_ONLY | KSPIN_FLAG_DO_NOT_INITIATE_PROCESSING, // Flags
      1,                                        // Instances Possible
      0,                                        // Instances Necessary
      &ksAudioPinAllocatorFraming,              // Allocator Framing
      reinterpret_cast<PFNKSINTERSECTHANDLEREX>(CapturePin::IntersectHandler)
    },

    { // Audio Output Pin
      NULL,                                     // Dispatch
      NULL,                                     // AutomationTable
      { // PinDescriptor
        DEFINE_KSPIN_DEFAULT_INTERFACES,        // Interfaces
        DEFINE_KSPIN_DEFAULT_MEDIUMS,           // Mediums
        0,                                      // Range Count
        NULL,                                   // Ranges
        KSPIN_DATAFLOW_OUT,                      // Dataflow
        KSPIN_COMMUNICATION_NONE,               // Communication
        &KSNODETYPE_SPEAKER,             // Category
        NULL, // Name
        0                                       // Reserved
      },
      KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT, // Flags
      0,                                        // Instances Possible
      0,                                        // Instances Necessary
      NULL,                                     // Allocator Framing
      NULL                                      // IntersectHandler
    }

};

/**
 * This is the dispatch table for the capture filter.  It provides notification
 * of creation, closure, processing (for filter-centrics, not for the capture
 * filter), and resets (for filter-centrics, not for the capture filter).
 */
static
const
KSFILTER_DISPATCH
ksCaptureFilterDispatch = {
    CaptureFilter::DispatchCreate,    // Filter Create
    NULL,                             // Filter Close
    NULL,                             // Filter Process
    NULL                              // Filter Reset
};


/**
 *
 */
static
const
KSNODE_DESCRIPTOR
ksAudioNodeDescriptors[] = {
    { //Node 0 : DAC
      NULL,               //Automation table
      &KSNODETYPE_DAC,    //Type (Analog-to-digital converter)
      NULL                //Name
    },

    { //Node 1 : Volume
      NULL,               //Automation table
      &KSNODETYPE_VOLUME, //Type
      NULL                //Name
    }
};

typedef enum {
    DAC_NODE =0,
    VOLUME_NODE
} NODE_NAMES;

typedef enum {
    AUDIO_IN_PIN = 0,
    AUDIO_OUT_PIN
} PIN_NAMES;

/**
 *
 */
static
const
KSTOPOLOGY_CONNECTION
ksAudioConnections[] = {
    //FromNode       //FromNode Pin  //ToNode       //ToNodePin
    { KSFILTER_NODE, AUDIO_IN_PIN,   DAC_NODE,      1 /*Out pin*/ },
    { DAC_NODE,      0, /*in pin*/   VOLUME_NODE,   1 /*Out pin*/ },
    { VOLUME_NODE,   0, /*in pin*/   KSFILTER_NODE, AUDIO_OUT_PIN }
};


/**
 * The descriptor for the capture filter. We don't specify any topology
 * since there's only one pin on the filter. Realistically, there would
 * be some topological relationships here because there would be input
 * pins from crossbars and the like.
 */
const
KSFILTER_DESCRIPTOR
ksCaptureFilterDescriptor = {
    &ksCaptureFilterDispatch,               // Dispatch Table
    NULL,                                   // Automation Table
    KSFILTER_DESCRIPTOR_VERSION,            // Version
    0,                                      // Flags
    &KSNAME_Filter,                         // Reference GUID
    DEFINE_KSFILTER_PIN_DESCRIPTORS(ksCaptureFilterPinDescriptors),
    DEFINE_KSFILTER_CATEGORIES(guidCaptureFilterCategories),  // Categories
    DEFINE_KSFILTER_NODE_DESCRIPTORS(ksAudioNodeDescriptors), // NodeDescriptors
    DEFINE_KSFILTER_CONNECTIONS(ksAudioConnections),          // Connections
    NULL                                                      // Component ID
};


