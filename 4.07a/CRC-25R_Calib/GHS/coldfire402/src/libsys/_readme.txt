
		Green Hills Software Low-level Library Source

These source files contain low level routines that can be 
modified and/or customized for the user's target environment.

The Green Hills Development Guide contains a chapter describing
in more detail the runtime environment and library system implemented
by these modules.

These source files comprise one binary files which is typically
included in the architecture-specific library directories which are
part of a cross-distribution package: libsys.a.

libsys.a is the low-level system call library. This library includes
such routines as open(), write(), and exit() which together provide a
full-featured runtime environment.  The library also contains
routines that control runtime profiling, a feature of the MULTI
Development Environment.  A small amount of architecture-specific
assembly code used to accomplish various low-level tasks is included in the 
library as well.

To rebuild libsys.a using the included project file, libsys.bld,
select the libsys.bld field from within the parent project and use the
"Build Selected Files" option of the MULTI Builder's "Build" menu.
