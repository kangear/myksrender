!IF 0

Copyright (C) Denis Shienkov, 2011

Module Name:

    makefile.

!ENDIF

!if "$(DDK_TARGET_OS)"=="Win2K"
!message This driver is not intended to target the Windows 2000 platform.
!else
!INCLUDE $(NTMAKEENV)\makefile.def
!endif
