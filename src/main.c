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
volatile unsigned short timer_led;


// Globals ---------------------------------------------------------------------



// ------- de los timers -------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby = 0;
// volatile unsigned short timer_prender_ringing = 0;
// //volatile unsigned char display_timer;
// volatile unsigned char timer_meas;




// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);

// To export -------------------------------------------------------------------
typedef struct {
    unsigned char treatment_time_min;
    unsigned char alarms_onoff;
    unsigned char ticker_onoff;
    unsigned short ticker_time;
    
} mem_bkp_t;


typedef enum {
    SW_NO = 0,
    SW_MIN,
    SW_HALF,
    SW_FULL
    
} resp_sw_t;

resp_sw_t CheckSET (void);
resp_sw_t CheckO3 (void);
void UpdateSwitchesTimeout (void);
void UpdateSwitches (void);
resp_t MENU_Main (mem_bkp_t *);

// Main Program States
typedef enum
{    
    MAIN_INIT = 0,
    MAIN_WELCOME,
    MAIN_STAND_BY_0,
    MAIN_STAND_BY_1,
    MAIN_START_TREATMENT,
    MAIN_IN_TREATMENT,
    MAIN_GO_PAUSE,
    MAIN_PAUSED,    
    MAIN_RESUMING,
    MAIN_ENDING_TREATMENT,
    MAIN_IN_MAIN_MENU
    
} main_state_t;

volatile unsigned short sw_o3_cntr = 0;
volatile unsigned short sw_set_cntr = 0;

//ESTADOS DEL BUZZER
typedef enum
{    
    BUZZER_INIT = 0,
    BUZZER_TO_STOP,

    BUZZER_MULTIPLE_LONG,
    BUZZER_MULTIPLE_LONGA,
    BUZZER_MULTIPLE_LONGB,

    BUZZER_MULTIPLE_HALF,
    BUZZER_MULTIPLE_HALFA,
    BUZZER_MULTIPLE_HALFB,

    BUZZER_MULTIPLE_SHORT,
    BUZZER_MULTIPLE_SHORTA,
    BUZZER_MULTIPLE_SHORTB
    
} buzzer_state_t;

//COMANDOS DEL BUZZER	(tienen que ser los del estado de arriba)
#define BUZZER_STOP_CMD		BUZZER_TO_STOP
#define BUZZER_LONG_CMD		BUZZER_MULTIPLE_LONG
#define BUZZER_HALF_CMD		BUZZER_MULTIPLE_HALF
#define BUZZER_SHORT_CMD	BUZZER_MULTIPLE_SHORT

#define TIM_BIP_SHORT		300
#define TIM_BIP_SHORT_WAIT	500
#define TIM_BIP_HALF		600
#define TIM_BIP_HALF_WAIT	800
#define TIM_BIP_LONG		2000
#define TIM_BIP_LONG_WAIT	2000

// for the buzzer
buzzer_state_t buzzer_state = BUZZER_INIT;
unsigned char buzzer_multiple;
volatile unsigned short buzzer_timeout = 0;
void BuzzerCommands(unsigned char, unsigned char);
void UpdateBuzzer (void);
void UpdateBuzzerTimeout (void);;

volatile unsigned char treatment_running = 0;
volatile unsigned char treatment_running_mins = 0;
volatile unsigned char treatment_running_secs = 0;
volatile unsigned short timer_ticker = 0;

volatile unsigned short millis = 0;

void RelayOn (void);
void RelayOff (void);
unsigned char RelayIsOn (void);
unsigned char RelayIsOff (void);
void UpdateRelay (void);
void UpdateRelayTimeout (void);


// End To Export ---------------------------------------------------------------

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
    resp_t resp = resp_continue;
    unsigned char lcd_l1_was_on = 0;
    unsigned char last_secs = 0;
    unsigned char barrita = 0;

    //configuracion desde la memoria
    // configuration_in_mem.treatment_time_min = 10;
    // configuration_in_mem.ticker_time = 60000;
    mem_bkp_t configurations_in_mem;
    configurations_in_mem.ticker_onoff = 1;
    unsigned char treatment_time_min = 10;
    unsigned short ticker = 60000;

    LCD_UtilsInit();
    
    while (1)
    {
        switch(main_state)
        {
        case MAIN_INIT:
            LCD_ClearScreen();
            main_state = MAIN_WELCOME;
            LCD_Scroll1Reset();
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
            sprintf (s_lcd, "Tiempo de Generacion: %d", treatment_time_min);
            strcat (s_lcd, " minutos - Presione O3 para Generar Ozono o SET para ingresar al Menu");
            main_state++;
            break;

        case MAIN_STAND_BY_1:
            resp = LCD_Scroll2 (s_lcd);    //da vueltas solo? SIP

            if (CheckO3() > SW_MIN)
            {
                treatment_running_mins = treatment_time_min;
                treatment_running_secs = 0;
                
                main_state = MAIN_START_TREATMENT;
            }

            if (CheckSET() > SW_MIN)
                main_state = MAIN_IN_MAIN_MENU;
            
            break;

        case MAIN_START_TREATMENT:
            if (CheckO3() == SW_NO)
            {
                LCD_ClearScreen();
                BuzzerCommands(BUZZER_LONG_CMD, 1);
                timer_ticker = ticker;
                RelayOn();

                treatment_running = 1;

                main_state = MAIN_IN_TREATMENT;
            }
            break;

        case MAIN_IN_TREATMENT:
            if (ticker)
            {
                if (!timer_ticker)
                {
                    BuzzerCommands(BUZZER_SHORT_CMD, 1);
                    timer_ticker = ticker;
                }
            }

            if (CheckO3() > SW_MIN)
            {
                //freno contador aca?
                treatment_running = 0;
                main_state = MAIN_GO_PAUSE;
            }

            //Update del Display timer
            if (last_secs != treatment_running_secs)
            {
                last_secs = treatment_running_secs;
                sprintf(s_lcd, "%02d:%02d         O3", treatment_running_mins, treatment_running_secs);
                LCD_Writel1(s_lcd);
            }

            //Update del Display barrita
            if (!timer_standby)
            {
                timer_standby = 300;
                
                switch(barrita)
                {
                    case 0:
                        Lcd_SetDDRAM (0x4f);
                        LCDStartTransmit(0xB0);
                        // LCD_Writel2("               -");
                        barrita++;
                        break;

                    case 1:
                        Lcd_SetDDRAM (0x4f);
                        LCDStartTransmit(0x2f);
                        // LCD_Writel2("               /");
                        barrita++;
                        break;

                    case 2:
                        Lcd_SetDDRAM (0x4f);
                        LCDStartTransmit(0x7C);
                        // LCD_Writel2("               -");
                        barrita = 0;
                        break;

                default:
                    barrita = 0;
                    break;
                }
            }
            

            //terminacion de tratamiento
            if ((treatment_running_mins == 0) && (treatment_running_secs == 0))
                main_state = MAIN_ENDING_TREATMENT;
            
            break;
            
        case MAIN_GO_PAUSE:
            if (CheckO3() == SW_NO)
            {
                LCD_ClearScreen();
                BuzzerCommands(BUZZER_SHORT_CMD, 3);
                RelayOff();
                LCD_Scroll2Reset();
                main_state = MAIN_PAUSED;
            }
            break;

        case MAIN_PAUSED:

            if (!timer_standby)
            {
                if (!lcd_l1_was_on)
                {
                    LCD_Writel1("  O3 en Pausa  ");
                    timer_standby = 1000;
                    lcd_l1_was_on = 1;
                }
                else
                {
                    LCD_Writel1(s_blank);
                    timer_standby = 1000;
                    lcd_l1_was_on = 0;                    
                }
            }

            resp = LCD_Scroll2 ("Presione O3 para continuar o SET para terminar");

            if (CheckO3() > SW_MIN)
                main_state = MAIN_RESUMING;

            if (CheckSET() > SW_MIN)
                main_state = MAIN_ENDING_TREATMENT;

            break;

        case MAIN_RESUMING:
            if (CheckO3() == SW_NO)
                main_state = MAIN_START_TREATMENT;

            break;

        case MAIN_ENDING_TREATMENT:
            if (CheckSET() == SW_NO)
            {
                BuzzerCommands(BUZZER_LONG_CMD, 3);
                main_state = MAIN_INIT;
            }
            break;
            
        case MAIN_IN_MAIN_MENU:
            resp = MENU_Main(&configurations_in_mem);

            if (resp == resp_finish)
                main_state = MAIN_INIT;
            
            break;

            
            
        default:
            main_state = MAIN_INIT;
            break;
        }

        UpdateRelay();
        UpdateSwitches();
        UpdateBuzzer();
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

#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

resp_sw_t CheckO3 (void)
{
    resp_sw_t sw = SW_NO;
    
    if (sw_o3_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (sw_o3_cntr > SWITCHES_THRESHOLD_HALF)
        sw = SW_HALF;
    else if (sw_o3_cntr > SWITCHES_THRESHOLD_MIN)
        sw = SW_MIN;

    return sw;    
}


resp_sw_t CheckSET (void)
{
    resp_sw_t sw = SW_NO;
    
    if (sw_set_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (sw_set_cntr > SWITCHES_THRESHOLD_HALF)
        sw = SW_HALF;
    else if (sw_set_cntr > SWITCHES_THRESHOLD_MIN)
        sw = SW_MIN;

    return sw;    
}


unsigned char switches_timer = 0;

void UpdateSwitchesTimeout (void)
{
    if (switches_timer)
        switches_timer--;
}


void UpdateSwitches (void)
{
    if (!switches_timer)
    {
        if (SWITCH_O3)
            sw_o3_cntr++;
        else if (sw_o3_cntr > 50)
            sw_o3_cntr -= 50;
        else if (sw_o3_cntr > 10)
            sw_o3_cntr -= 5;
        else if (sw_o3_cntr)
            sw_o3_cntr--;

        if (SWITCH_SET)
            sw_set_cntr++;
        else if (sw_set_cntr > 50)
            sw_set_cntr -= 50;
        else if (sw_set_cntr > 10)
            sw_set_cntr -= 5;
        else if (sw_set_cntr)
            sw_set_cntr--;
        
        switches_timer = SWITCHES_TIMER_RELOAD;
    }       
}


void UpdateBuzzerTimeout (void)
{
    if (buzzer_timeout)
        buzzer_timeout--;
}


void BuzzerCommands(unsigned char command, unsigned char multiple)
{
    buzzer_state = command;
    buzzer_multiple = multiple;
}


void UpdateBuzzer (void)
{
    switch (buzzer_state)
    {
        case BUZZER_INIT:
            break;

        case BUZZER_MULTIPLE_SHORT:
            if (buzzer_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_SHORT;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_SHORTA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_SHORT_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_SHORTB:
            if (!buzzer_timeout)
            {
                if (buzzer_multiple)
                    buzzer_multiple--;

                buzzer_state = BUZZER_MULTIPLE_SHORT;
            }
            break;

        case BUZZER_MULTIPLE_HALF:
            if (buzzer_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_HALF;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_HALFA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_HALF_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_HALFB:
            if (!buzzer_timeout)
            {
                if (buzzer_multiple)
                    buzzer_multiple--;

                buzzer_state = BUZZER_MULTIPLE_HALF;
            }
            break;

        case BUZZER_MULTIPLE_LONG:
            if (buzzer_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_LONG;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_LONGA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_LONG_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_LONGB:
            if (!buzzer_timeout)
            {
                if (buzzer_multiple)
                    buzzer_multiple--;

                buzzer_state = BUZZER_MULTIPLE_LONG;
            }
            break;

        case BUZZER_TO_STOP:
        default:
            BUZZER_OFF;
            buzzer_state = BUZZER_INIT;
            break;
    }
}


#define TT_DELAYED_OFF		3600		//para relay placa redonda
#define TT_DELAYED_ON		4560		//para relay placa redonda
#define TT_RELAY			60		//timeout de espera antes de pegar o despegar el relay

enum Relay_State {

	ST_OFF = 0,
	ST_WAIT_ON,
	ST_DELAYED_ON,
	ST_ON,
	ST_WAIT_OFF,
	ST_DELAYED_OFF

};

volatile unsigned short timer_relay = 0;
enum Relay_State relay_state = ST_OFF;
unsigned char last_edge;

//Pide conectar el relay
void RelayOn (void)
{
    if (!RelayIsOn())
    {
        relay_state = ST_WAIT_ON;
        timer_relay = TT_RELAY;
    }
}

//Pide desconectar el relay
void RelayOff (void)
{
    if (!RelayIsOff())
    {
        relay_state = ST_WAIT_OFF;
        timer_relay = TT_RELAY;
    }
}

//Revisa el estado del relay
unsigned char RelayIsOn (void)
{
    if ((relay_state == ST_WAIT_ON) ||
        (relay_state == ST_DELAYED_ON) ||
        (relay_state == ST_ON))
        return 1;
    else
        return 0;
}

//Revisa el estado del relay
unsigned char RelayIsOff (void)
{
    if ((relay_state == ST_WAIT_OFF) ||
        (relay_state == ST_DELAYED_OFF) ||
        (relay_state == ST_OFF))
        return 1;
    else
        return 0;
}


void UpdateRelayTimeout (void)
{
    if (timer_relay)
        timer_relay--;
}

//chequeo continuo del estado del relay
void UpdateRelay (void)
{
    unsigned char edge = 0;

    // if ((!last_edge) && (SYNC))		//flanco ascendente detector
    // {									//senoidal arriba
    //     last_edge = 1;
    //     SYNCP_ON;
    // }

    // if ((last_edge) && (!SYNC))		//flanco descendente detector
    // {									//senoidal abajo
    //     edge = 1;
    //     last_edge = 0;
    //     SYNCP_OFF;
    // }

    // switch (relay_state)
    // {
    // case ST_OFF:

    //     break;

    // case ST_WAIT_ON:
    //     if (edge)
    //     {
    //         edge = 0;
    //         relay_state = ST_DELAYED_ON;
    //         TIM16->CNT = 0;
    //     }

    //     if (!timer_relay)		//agoto el timer y no encontro sincro, pega igual
    //     {
    //         RELAY_ON;
    //         relay_state = ST_ON;
    //     }
    //     break;

    // case ST_DELAYED_ON:
    //     if (TIM16->CNT > TT_DELAYED_ON)
    //     {
    //         RELAY_ON;
    //         relay_state = ST_ON;
    //     }
    //     break;

    // case ST_ON:

    //     break;

    // case ST_WAIT_OFF:
    //     if (edge)
    //     {
    //         edge = 0;
    //         relay_state = ST_DELAYED_OFF;
    //         TIM16->CNT = 0;
    //     }

    //     if (!timer_relay)		//agoto el timer y no encontro sincro, despega igual
    //     {
    //         RELAY_OFF;
    //         relay_state = ST_OFF;
    //     }

    //     break;

    // case ST_DELAYED_OFF:
    //     if (TIM16->CNT > TT_DELAYED_OFF)
    //     {
    //         RELAY_OFF;
    //         relay_state = ST_OFF;
    //     }
    //     break;

    // default:
    //     RELAY_OFF;
    //     relay_state = ST_OFF;
    //     break;
    // }
}


void UpdateTreatmentTimeout (void)
{
    if (treatment_running)
    {
        if (millis)
            millis--;
        else
        {
            if (treatment_running_secs)
            {
                treatment_running_secs--;
                millis = 1000;
            }
            else
            {
                if (treatment_running_mins)
                {
                    treatment_running_mins--;
                    treatment_running_secs = 59;
                    millis = 1000;
                }
            }
        }
    }
}



typedef enum {
    MENU_INIT = 0,
    MENU_SHOW_TREATMENT_TIME,
    MENU_SHOW_ALARM,
    MENU_SHOW_TICKER,
    MENU_SHOW_END_CONF,
    MENU_CONF_TREATMENT_TIME,
    MENU_CONF_ALARM,
    MENU_CONF_TICKER,
    MENU_END_CONF

} menu_state_t;

menu_state_t menu_state = MENU_INIT;
//funcion de seleccion del menu principal
//devuelve nueva selección o estado anterior
resp_t MENU_Main (mem_bkp_t * configurations)
{
    resp_t resp = resp_continue;
    sw_actions_t actions = selection_none;
    unsigned char onoff = 0;
    unsigned short time = 0;

    switch (menu_state)
    {
    case MENU_INIT:
        resp = LCD_ShowBlink ("    Entering    ",
                              "  Config Menu   ",
                              1,
                              BLINK_DIRECT);

        if (resp == resp_finish)
        {
            resp = resp_continue;
            if (CheckSET() == SW_NO)
                menu_state++;
        }
        break;

    case MENU_SHOW_TREATMENT_TIME:
        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckO3() > SW_NO)
            actions = selection_dwn;
        
        resp = LCD_ShowSelectv2((const char *) "Treatment Time  ",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_ALARM;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_END_CONF;
        
        if (resp == resp_selected)
            menu_state = MENU_CONF_TREATMENT_TIME;

        break;

    case MENU_SHOW_ALARM:
        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckO3() > SW_NO)
            actions = selection_dwn;

        resp = LCD_ShowSelectv2((const char *) "Alarm           ",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_TICKER;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_TREATMENT_TIME;

        if (resp == resp_selected)
            menu_state = MENU_CONF_ALARM;

        break;

    case MENU_SHOW_TICKER:
        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckO3() > SW_NO)
            actions = selection_dwn;

        resp = LCD_ShowSelectv2((const char *) "Ticker          ",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_END_CONF;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_END_CONF;

        if (resp == resp_selected)
        {
            onoff = configurations->ticker_onoff;
            menu_state = MENU_CONF_TICKER;
        }
        break;


    case MENU_SHOW_END_CONF:
        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckO3() > SW_NO)
            actions = selection_dwn;

        resp = LCD_ShowSelectv2((const char *) "End Configuration",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_TREATMENT_TIME;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_TREATMENT_TIME;

        if (resp == resp_selected)
            menu_state = MENU_END_CONF;

        break;
        

    case MENU_CONF_TREATMENT_TIME:
        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckO3() > SW_NO)
            actions = selection_dwn;

        time = configurations->treatment_time_min;
        resp = LCD_EncoderChange("minutos:        ",
                                 &time,
                                 1,
                                 20,
                                 actions);

        if (resp == resp_finish)
        {
            configurations->treatment_time_min = time;
            menu_state = MENU_SHOW_TREATMENT_TIME;
            resp = resp_continue;
        }
        break;

    case MENU_CONF_ALARM:
        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckO3() > SW_NO)
            actions = selection_dwn;

        onoff = configurations->alarms_onoff;
        resp = LCD_EncoderOptionsOnOff("Alarm           ",
                                       &onoff,
                                       actions);

        if (resp == resp_finish)
        {
            configurations->alarms_onoff = onoff;            
            menu_state = MENU_SHOW_TREATMENT_TIME;
            resp = resp_continue;
        }
        break;

    case MENU_CONF_TICKER:
        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckO3() > SW_NO)
            actions = selection_dwn;

        onoff = configurations->ticker_onoff;
        resp = LCD_EncoderOptionsOnOff("Ticker          ",
                                       &onoff,
                                       actions);

        if (resp == resp_finish)
        {
            configurations->ticker_onoff = onoff;
            menu_state = MENU_SHOW_TREATMENT_TIME;
            resp = resp_continue;
        }
        break;


    case MENU_END_CONF:
        menu_state = MENU_INIT;
        resp = resp_finish;
        break;
        
    default:
        menu_state = MENU_INIT;
        break;
    }
    
    return resp;
}


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


    LCD_UpdateTimer();

    UpdateSwitchesTimeout();

    UpdateBuzzerTimeout();    

    UpdateRelayTimeout();

    UpdateTreatmentTimeout();
}

//--- end of file ---//

