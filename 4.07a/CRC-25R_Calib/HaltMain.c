/*********************************************************************

  MODULE:   Main for CRC-25R

  FILE:     CalibratorMain_R.c

    DATE:     03/25/08

  ANALYSIS:     Main Routine -- first C routine
          Calls routines to do hardware startup
          Calls routines to do chamber initialization
          Calls signon screen
          Calls Main Measurement screen
          is never returned to


  *************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

int main(void){
    asm("nop");
    asm("halt");
    return 0;
}
