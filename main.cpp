#include "global.h"


global prg;
u8 profibus_out_table[OUTPUT_DATA_SIZE];

u8 profibus_in_table[INPUT_DATA_SIZE];
int main(void)
{

  profibus_out_table[0]=0xFF;
  profibus_in_table[0]=0xFF;

	SystemInit();



prg.init();

prg.cycle();

}
