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

// #include "lcd.h"
#include "lcd_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Externals -------------------------------------------------------------------
extern char s_blank [];

// ------- Externals para el LED state --------
volatile unsigned short timer_led;

// --- Externals Funcs GSM
unsigned char register_status = 0;
unsigned char rssi_level = 0;


// Globals ---------------------------------------------------------------------
unsigned short show_power_index = 0;	//lo uso como timer sincronizado con la medicion, tick 2 secs.
unsigned short show_power_index_debug = 0;



// ------- de los timers -------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby = 0;
volatile unsigned short timer_prender_ringing = 0;
volatile unsigned short tcp_kalive_timer;
//volatile unsigned char display_timer;
volatile unsigned char timer_meas;




// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);

// Main Program States
typedef enum
{    
    MAIN_INIT = 0,
    MAIN_WELCOME,
    MAIN_STAND_BY_0,
    MAIN_STAND_BY_1,
    MAIN_IN_MAIN_MENU
    
} main_state_t;


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


    //--- Implementacion O3 ---//    
    main_state_t main_state = MAIN_INIT;
    char s_lcd [100] = { 0 };
    unsigned char treatment_time = 0;
    resp_t resp = resp_continue;

    LCD_UtilsInit();
    
    while (1)
    {
        switch(main_state)
        {
        case MAIN_INIT:
            LCD_ClearScreen();
            treatment_time = 30;
            
            main_state = MAIN_WELCOME;
            break;
            
        case MAIN_WELCOME:
            resp = LCD_Scroll1 ("GESTALD Security & Protection Dept.");

            if (resp == resp_finish)
            {
                do {
                    resp = LCD_ShowBlink ("GESTALD O3SPv1.0",
                                          s_blank,
                                          0,
                                          BLINK_NO);
                } while (resp != resp_finish);

                main_state = MAIN_STAND_BY_0;
            }
            break;
            
        case MAIN_STAND_BY_0:
            sprintf (s_lcd, "Tiempo de Generacion: %d", treatment_time);
            strcat (s_lcd, " minutos - Presione O3 para Generar Ozono o SET para ingresar al Menu");
            main_state ++;
            break;

        case MAIN_STAND_BY_1:
            resp = LCD_Scroll2 (s_lcd);    //da vueltas solo?
            break;
            
        default:
            main_state = MAIN_INIT;
            break;
        }
    }

    //--- Fin implementacion O3 ---//
    
    // ChangeLed(LED_BLINK_3);
    // while (1)
    // {
    //     UpdateLed();
    // }

    // while (1)
    // {
    //     Wait_ms(500);
    //     if (LED)
    //         LED_OFF;
    //     else
    //         LED_ON;
    // }
    
    //--- Welcome code ---//
//     LED_OFF;
//     RELAY_OFF;

//     USART1Config();
//     USART2Config();

//     EXTIOff();

// //--- Programa de Redonda Basic - Produccion ---
//     unsigned char led_rssi_high = 0;

//     while (1)
//     {
//         switch (main_state)
//         {
//         case main_init:
//             ChangeLed(LED_STANDBY);
//             RELAY_OFF;
//             main_state = main_wait_for_gsm_network;

//             //reset de flags del gsm
//             diag_prender_reset;
//             diag_ringing_reset;

//             //reset de configuraciones del gsm
//             envios_ok_change_reset;
//             timer_rep_change_reset;
//             prender_ring_change_reset;
//             break;

//         case main_wait_for_gsm_network:
//             if (FuncsGSMStateAsk() >= gsm_state_ready)
//             {
//                 main_state = main_ready;
//                 ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
//             }
//             break;

//         case main_ready:
//             if (diag_prender)
//             {
//                 diag_prender_reset;
//                 main_state = main_enable_output;
//                 RELAY_ON;
//                 timer_standby = timer_rep * 1000;
//                 Usart2Send("RELAY ACTIVO\n");
//             }

//             if ((diag_ringing) &&
//                 (prender_ring) &&
//                 (!timer_prender_ringing))
//             {
//                 diag_ringing_reset;
//                 timer_prender_ringing = 12000;
//                 main_state = main_enable_output;
//                 RELAY_ON;
//                 timer_standby = timer_rep * 1000;
//                 Usart2Send("RELAY ACTIVO\n");
//             }

//             if (FuncsGSMStateAsk() < gsm_state_ready)
//             {
//                 main_state = main_wait_for_gsm_network;
//                 ChangeLed(LED_STANDBY);
//             }

//             if ((rssi_level > 10) && (!led_rssi_high))
//             {
//                 ChangeLed(LED_GSM_NETWORK_HIGH_RSSI);
//                 led_rssi_high = 1;
//             }

//             if ((rssi_level <= 10) && (led_rssi_high))
//             {
//                 ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
//                 led_rssi_high = 0;
//             }

//             ConfigurationCheck();
//             break;

//         case main_enable_output:
//             if (!timer_standby)
//             {
//                 main_state = main_ready;
//                 RELAY_OFF;
//             }

//             ConfigurationCheck();
//             break;
            
//         // case LAMP_OFF:
//         //     Usart2Send("PRENDIDO\r\n");
//         //     FuncsGSMSendSMS("PRENDIDO", mem_conf.num_reportar);
//         //     LED_ON;
//         //     break;


//         default:
//             main_state = main_init;
//             break;
//         }

//         //Cosas que no dependen del estado del programa
//         UpdateLed();
//         // UpdateRelay ();
//         // UpdatePhotoTransistor();
//         FuncsGSM();
//     }	//end while 1

// //---------- Fin Programa de Produccion Redonda Basic--------//
// #endif	//USE_REDONDA_BASIC

// //---------- Inicio Programa de Produccion Redonda Basic --------//





    return 0;
}

//--- End of Main ---//


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (timer_prender_ringing)
        timer_prender_ringing--;
    
    if (timer_led)
        timer_led--;

    LCD_UpdateTimer();
}

//--- end of file ---//

