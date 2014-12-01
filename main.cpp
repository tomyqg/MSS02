#include "global.h"


global prg;

u8 profibus_out_table[OUTPUT_DATA_SIZE];

u8 profibus_in_table[INPUT_DATA_SIZE];
int main(void)
{


	SystemInit();



prg.init();

prg.cycle();

}
//main

