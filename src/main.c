//----------------------------------------------------
// #### PROYECTO KIRNO REDONDA GSM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C #######################################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f0xx.h"

#include "gpio.h"
#include "tim.h"
#include "hard.h"
// #include "uart.h"
// #include "adc.h"
// #include "dma.h"
// #include "flash_program.h"

#include "core_cm0.h"

#include "lcd.h"
#include "lcd_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Externals -------------------------------------------------------------------
extern char s_blank [];

// ------- Externals para el LED state --------
volatile unsigned short timer_led = 0;


// Globals ---------------------------------------------------------------------



// ------- de los timers -------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby = 0;



// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);



//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (unsigned char i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }


    //--- LCD Tests ---//    
    // LCD_UtilsInit();
    // while (LCD_ShowBlink("Kirno Technology",
    //                      "  Smart Driver  ",
    //                      2,
    //                      BLINK_DIRECT) != resp_finish);

    // LCD_ClearScreen();


    // resp_t resp = resp_continue;
    // while (1)
    // {
    //     // LCD_ClearScreen();
    //     // Wait_ms(2000);
    //     do {
    //         resp = LCD_Scroll1 ("GESTALD Security & Protection Dept.");
    //     } while (resp != resp_finish);

    //     Wait_ms(2000);
    // }
    //--- Fin LCD Tests ---//

    //--- LCD Test CGRAM ---//
    // LCD_UtilsInit();
    // LCD_BigNumbersInit();
    // while (1)
    // {
    //     LCD_ClearScreen();
    //     LCD_1ER_RENGLON;
    //     Lcd_senddata(0x00);
    //     Lcd_senddata(0x01);
    //     Lcd_senddata(0x02);

        
    //     LCD_2DO_RENGLON;
    //     Lcd_senddata(0x00);
    //     Lcd_senddata(0x03);        
    //     Lcd_senddata(0x02);
    //     Lcd_senddata(0x04);
    //     Wait_ms(1000);

    // }
    // --- Fin LCD Test CGRAM ---//

    //--- LCD Test BigNumbers ---//
    // LCD_UtilsInit();
    // LCD_BigNumbersInit();
    // while (1)
    // {
    //     LCD_ClearScreen();
    //     // Lcd_SetDDRAM(0x06);
    //     // Lcd_senddata(0xa5);
    //     // Lcd_SetDDRAM(0x46);
    //     // Lcd_senddata(0xa5);
        
    //     for (unsigned char i = 0; i < 10; i++)
    //     {
    //         LCD_BigNumbers(0,i);
    //         Wait_ms(1000);
    //     }
    //     for (unsigned char i = 0; i < 10; i++)
    //     {
    //         LCD_BigNumbers(3,i);
    //         Wait_ms(1000);
    //     }
    //     for (unsigned char i = 0; i < 10; i++)
    //     {
    //         LCD_BigNumbers(7,i);
    //         Wait_ms(1000);
    //     }
    //     for (unsigned char i = 0; i < 10; i++)
    //     {
    //         LCD_BigNumbers(10,i);
    //         Wait_ms(1000);
    //     }        
    // }
    //--- Fin LCD Test BigNumbers ---//
    
    //--- Implementacion O3 ---//
    
    

    return 0;
}

//--- End of Main ---//














void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (timer_led)
        timer_led--;

    if (timer_ticker)
        timer_ticker--;


}

//--- end of file ---//

