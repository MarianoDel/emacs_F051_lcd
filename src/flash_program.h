//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FLASH_PROGRAM.H #######################
//---------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _FLASH_PROGRAM_H_
#define _FLASH_PROGRAM_H_

// Includes for Configurations -------------------------------------------------
#include "stm32f0xx.h"

// Configurations Defines ------------------------------------------------------
//- Where to Backup ------------------------------------------------------------
#define FLASH_PAGE_FOR_BKP PAGE60    //dejo la 61 libre para dummy

//- Data Struct to Backup ------------------------------------------------------
 //OJO esta estructuras deben estar alineadas en 4 bytes
typedef struct parameters {

    char num_reportar [20];			//20
    char imei [24];					//44
    char num_propio [20];			//64

    unsigned int acumm_wh;				//68
    unsigned int acumm_w2s;				//72
    unsigned short acumm_w2s_index;	//74

    unsigned char timer_reportar;		//75
    unsigned char envios_ok;			//76

    unsigned char send_energy_flag;	//77
    //dummys para completar
    unsigned char prender_ring;			//78
    unsigned char dummy1;			//79
    unsigned char dummy2;			//80

} parameters_typedef;

// Configuration Change
#define envios_ok		(mem_conf.envios_ok)
#define envios_ok_change	(mem_conf.send_energy_flag & 0x80)
#define envios_ok_change_set	(mem_conf.send_energy_flag |= 0x80)
#define envios_ok_change_reset	(mem_conf.send_energy_flag &= 0x7F)

#define timer_rep		(mem_conf.timer_reportar)
#define timer_rep_change	(mem_conf.send_energy_flag & 0x40)
#define timer_rep_change_set	(mem_conf.send_energy_flag |= 0x40)
#define timer_rep_change_reset	(mem_conf.send_energy_flag &= 0xBF)

#define prender_ring		(mem_conf.prender_ring)
#define prender_ring_change	(mem_conf.send_energy_flag & 0x20)
#define prender_ring_change_set	(mem_conf.send_energy_flag |= 0x20)
#define prender_ring_change_reset	(mem_conf.send_energy_flag &= 0xDF)

#define num_tel_rep			(mem_conf.num_reportar)

// Flags
#define diag_prender		(mem_conf.send_energy_flag & 0x08)
#define diag_prender_set	(mem_conf.send_energy_flag |= 0x08)
#define diag_prender_reset	(mem_conf.send_energy_flag &= 0xF7)

#define diag_ringing		(mem_conf.send_energy_flag & 0x04)
#define diag_ringing_set	(mem_conf.send_energy_flag |= 0x04)
#define diag_ringing_reset	(mem_conf.send_energy_flag &= 0xFB)

// #define send_energy		(mem_conf.send_energy_flag & 0x01)
// #define send_energy_set		(mem_conf.send_energy_flag |= 0x01)
// #define send_energy_reset	(mem_conf.send_energy_flag &= 0xFE)

// #define send_sms_ok		(mem_conf.send_energy_flag & 0x02)
// #define send_sms_ok_set		(mem_conf.send_energy_flag |= 0x02)
// #define send_sms_ok_reset	(mem_conf.send_energy_flag &= 0xFD)

// #define diag_apagar		(mem_conf.send_energy_flag & 0x20)
// #define diag_apagar_set		(mem_conf.send_energy_flag |= 0x20)
// #define diag_apagar_reset	(mem_conf.send_energy_flag &= 0xDF)

//- End of Data Struct to Backup -----------------------------------------------

// Define the STM32F10x FLASH Page Size depending on the used STM32 device
// si es mayor a 128K la pagina es de 2KB, sino 1KB

//#define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#define FLASH_PAGE_SIZE			1024
#define FLASH_PAGE_SIZE_DIV2	512
#define FLASH_PAGE_SIZE_DIV4	256

#define PAGE28_START	0
#define PAGE29_START	256
#define PAGE30_START	512
#define PAGE31_START	768
#define PAGE31_END		1023

#define PAGE27			((uint32_t)0x08006C00)
#define PAGE28			((uint32_t)0x08007000)
#define PAGE29			((uint32_t)0x08007400)
#define PAGE30			((uint32_t)0x08007800)
#define PAGE31			((uint32_t)0x08007C00)

#define PAGE60			((uint32_t)0x0800F000)
#define PAGE61			((uint32_t)0x0800F400)
#define PAGE63			((uint32_t)0x0800FC00)


//de libreria st las tiene #include "stm32f0xx_flash.h"
/**
  * @brief  FLASH Status
  */
typedef enum
{
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;

/** @defgroup FLASH_Timeout_definition
  * @{
  */
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000B0000)

/** @defgroup FLASH_Flags
  * @{
  */

#define FLASH_FLAG_BSY                 FLASH_SR_BSY     /*!< FLASH Busy flag */
#define FLASH_FLAG_PGERR               FLASH_SR_PGERR   /*!< FLASH Programming error flag */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR  /*!< FLASH Write protected error flag */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP     /*!< FLASH End of Programming flag */

#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCB) == 0x00000000) && ((FLAG) != 0x00000000))

#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_PGERR) || \
                                  ((FLAG) == FLASH_FLAG_WRPERR) || ((FLAG) == FLASH_FLAG_EOP))



typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


// Module exported Functions ---------------------------------------------------
unsigned char WriteConfigurations (void);
unsigned char WriteFlash(unsigned int * p, uint32_t p_addr, unsigned char with_lock, unsigned char len_in_4);

#endif    /* _FLASH_PROGRAM_H_ */

//--- end of file ---//

