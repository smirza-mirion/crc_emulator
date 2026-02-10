
		Green Hills Software Low-level Library Source

These source files contain low level routines that can be 
modified and/or customized for the user's target environment.

The Green Hills Development Guide contains a chapter describing
in more detail the runtime environment and library system implemented
by these modules.

These source files comprise two binary files which are typically
included in the architecture-specific library directories which are
part of a cross-distribution package: libstartup.a and crt0.o.
These low level objects are linked in when building applications with
the Green Hills environment,  in its default mode of operation.
The user can modify and rebuild these low level objects using the
existing projects:  libsys.bld and crt0.bld, and replace them
in the binary library directories if desired.  Alternatively,  these source
files can be used as a reference for the low level functions of the Green 
Hills runtime system and as a basis upon which to create a set
of customized routines that may better suit the user's environment.

crt0.o is a the startup or initialization module. crt0.o contains the
routine _start() which is the default entry point for applications.
crt0.o is built from a minimal amount of assembly source;  it does some
basic setup (e.g. initialize stack pointer) and then calls into 
libstartup.a for some low level initialization.  Consult the
individual assembly module for the architecture of interest for more
detailed information.

To rebuild crt0.o using the included project file, crt0.bld, select the
crt0.bld field from within the parent project and use the
"Build Selected Files" option of the MULTI Builder's "Build" menu.

To rebuild libstartup.a using the included project file, libstartup.bld,
select the libstartup.bld field from within the parent project and use the
"Build Selected Files" option of the MULTI Builder's "Build" menu.

