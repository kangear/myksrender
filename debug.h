/*
*/

#ifndef DEBUG_H
#define DEBUG_H

/**
 * Modified version of ksdebug.h to support runtime debug level changes.
 */
enum DebugLevel {
    DBG_NONE     = 0x00000000,
    DBG_PRINT    = 0x00000001, // Blabla. Function entries for example
    DBG_WARNING  = 0x00000002, // warning level
    DBG_ERROR    = 0x00000004, // this doesn't generate a breakpoint
    DBG_STREAM   = 0x00000010, // For stream messages
    DBG_SYSTEM   = 0x10000000, // For system information messages
    DBG_ALL      = 0xFFFFFFFF,

    DBG_DEFAULT = DBG_PRINT | DBG_WARNING | DBG_ERROR
};


// Define global debug variable.
#ifdef DEFINE_DEBUG_VARS
#  if (DBG)
unsigned long ulDebugOut = DBG_DEFAULT;
#  endif

#else // !DEFINED_DEBUG_VARS
#  if (DBG)
extern unsigned long ulDebugOut;
#  endif
#endif

// Define the print statement.
#if defined(__cplusplus)
extern "C" {
#endif


// DBG is 1 in checked builds
#if (DBG)
#  define STR_MODULENAME "MyKs:"
#  define DOUT(lvl, strings)    \
    if ((lvl) & ulDebugOut)     \
{                               \
    DbgPrint(STR_MODULENAME);   \
    DbgPrint##strings;          \
    DbgPrint("\n");             \
}
#  define DENTER()              \
{                               \
    DbgPrint(STR_MODULENAME);   \
    DbgPrint((__FUNCTION__));   \
    DbgPrint("[\n");            \
}
#  define DLEAVE()              \
{                               \
    DbgPrint(STR_MODULENAME);   \
    DbgPrint((__FUNCTION__));   \
    DbgPrint("]\n");            \
}


#  define BREAK() \
    DbgBreakPoint()

#else // if (!DBG)
#  define DOUT(lvl, strings)
#  define DENTER()
#  define DLEAVE()
#  define BREAK()
#endif // !DBG


#if defined(__cplusplus)
}
#endif

#endif // DEBUG_H


