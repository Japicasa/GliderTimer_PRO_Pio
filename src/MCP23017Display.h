/*
   MCP23017Display.h
*/

#ifndef MMCP23017Display_h
#define MCP23017Display_h


#define MCP1 0x20
#define MCP2 0x21

#define GPIOA 0x12
#define GPIOB 0x13

#define COMMON_ANODE 0xFF // Common Anode display
#define COMMON_CATODE 0x80 // Common Catode display
void initMCPs();
void test_I2C_panel();
void displaycounter(int counter, int  displayon, int showcounter);
void writenum(int pos, int digit);
void writeseg(int pos, int segment);
void display_round_group(int round, int group, int displayon, int showcounter);


#endif
