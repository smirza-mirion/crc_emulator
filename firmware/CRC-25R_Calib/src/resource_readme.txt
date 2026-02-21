Board Setup Files (.dbs)
------------------------------------------------------------------------------
The board setup files (.dbs) are used by Green Hills Software's debug servers 
to initialize target hardware before beginning a debugging session.

The automatically-created default.con connection file contains
connection methods that reference the .dbs files in this resource.bld.

You can modify the scripts in the board setup files to suit your specific
hardware configuration and connection needs.

For more information about board setup files and connection files, 
see the "Target Connection User's Guide" for your processor.


Linker Directive Files (.ld)
------------------------------------------------------------------------------
The linker directive files (.ld) are used to link your program.

This resource.bld contains three linker directives files:
** standalone_ram.ld -- Used for programs that are linked into and run out
of RAM. 
** standalone_romcopy.ld -- Used for programs that are linked into ROM, but 
run out of RAM.
** standalone_romrun.ld -- Used for programs that are linked into and run
out of ROM. 

The .ld file located in the .bld file of your executable project will be used
when linking.  This can be replaced with a custom .ld file.

You can modify the provided linker directives files to suit your specific
hardware configuration and program layout needs.

For more information about linker directives files, see the "Embedded 
Development Guide" for your processor.
