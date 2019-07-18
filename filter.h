/*
*/

#ifndef FILTER_H
#define FILTER_H

#include "common.h"


class CaptureFilter
{
public:
    static NTSTATUS DispatchCreate(KSFILTER *filter, IRP *irp);

private:
    CaptureFilter (KSFILTER *filter);
    ~CaptureFilter();

    NTSTATUS Create();

private:
    static void Cleanup(CaptureFilter *filterObject);

private:
    KSFILTER *m_ksFilter;
};

#endif // FILTER_H

