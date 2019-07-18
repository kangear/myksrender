#ifndef COMMON_H
#define COMMON_H

extern "C" {
#include <wdm.h>
}

#include <windef.h>
#include <ntstrsafe.h>
#include <windef.h>
#define NOBITMAP
#include <mmreg.h>
#undef NOBITMAP
#include <unknown.h>
#include <ks.h>
#include <ksmedia.h>
#include <kcom.h>
#include <ksdebug.h>

#include "debug.h"


// From device.cpp

extern
const
KSDEVICE_DESCRIPTOR
ksDeviceDescriptor;

// From filter.cpp

extern
const
KSFILTER_DESCRIPTOR
ksCaptureFilterDescriptor;

// From capture.cpp

extern
const
KSPIN_DISPATCH
ksCapturePinDispatch;

extern
const
PKSDATARANGE
ksAudioPinDataRanges[1];

extern
const
KSALLOCATOR_FRAMING_EX
ksAudioPinAllocatorFraming;

/*
extern
const
KSPIN_DESCRIPTOR
CapturePinDescriptor;
*/



#endif // COMMON_H
