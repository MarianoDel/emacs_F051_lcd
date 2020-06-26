//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TEST.C ################################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------


#include <stdio.h>
// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
unsigned char SwapNibble (unsigned char a);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Test Nibble swaping\n");
    for (unsigned char i = 0; i < 0x0f; i++)
    {
        printf("%x %x\n", i, SwapNibble(i));
    }
    
    printf("Test Ended\n");
    
    return 0;
}


unsigned char SwapNibble (unsigned char a)
{
	unsigned char result = 0;

	if (a & 0x01)
		result |= 0x08;
	if (a & 0x02)
		result |= 0x04;
	if (a & 0x04)
		result |= 0x02;
	if (a & 0x08)
		result |= 0x01;

	return result;
}

//--- end of file ---//


