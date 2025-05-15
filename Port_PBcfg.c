/******************************************************************************
 *  @file       Port_PBcfg.c
 *  @author     Hassan Darwish
 *  @date       Feb 2025
 *  @brief      the Post Build configuration for Port Driver of TIVA-C Cortex M4
 *
 *  @details
 *  This source file contains the Post Build configurations and Module verison
 *  for the Port Driver for the AUTOSAR MCAL layer.
 ******************************************************************************/

/* ****************************************************************************
 *  INCLUDES
 * ****************************************************************************/

#include "Std_Types.h"
#include "Dio.h"
#include "Port.h"
#include "Port_Cfg.h"



/* ****************************************************************
 * Version
 * ****************************************************************/

/*
 * Module Version 1.0.0
 */
#define PORT_PBCFG_SW_MAJOR_VERSION                     (1U)
#define PORT_PBCFG_SW_MINOR_VERSION                     (0U)
#define PORT_PBCFG_SW_PATCH_VERSION                     (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define PORT_PBCFG_AR_RELEASE_MAJOR_VERSION             (4U)
#define PORT_PBCFG_AR_RELEASE_MINOR_VERSION             (0U)
#define PORT_PBCFG_AR_RELEASE_PATCH_VERSION             (3U)

/* ****************************************************************
 * Compatibilities
 * ****************************************************************/

/* AUTOSAR Version checking between PORT_PBcfg.c and PORT.h files */
#if ((PORT_PBCFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_PBCFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_PBCFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of PBcfg.c does not match the expected version"
#endif

/* Software Version checking between PORT_PBcfg.c and PORT.h files */
#if ((PORT_PBCFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_PBCFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_PBCFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of PBcfg.c does not match the expected version"
#endif


const Port_ConfigType Port_Configuration =
{
    .Pins =
    {
        {
            DioConf_LED1_PORT_NUM,
            DioConf_LED1_CHANNEL_NUM,
            PIN_MODE_DIO,      /* e.g., #define PIN_MODE_DIO 0U */
            PORT_PIN_OUT,
            STD_HIGH,
            TRUE,
            TRUE,
            RESISTOR_OFF
        },
        {
            DioConf_SW1_PORT_NUM,
            DioConf_SW1_CHANNEL_NUM,
            PIN_MODE_DIO,
            PORT_PIN_IN,
            STD_LOW,
            FALSE,
            TRUE,
            PULL_UP
        }
    }
};



