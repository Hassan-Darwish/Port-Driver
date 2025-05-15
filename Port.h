/******************************************************************************
 *  @file       Port.h
 *  @author     Hassan Darwish
 *  @date       Feb 2025
 *  @brief      The Port Driver for TIVA-C Cortex M4
 *
 *  @details
 *  This header file contains the implementation of the Port driver,
 *  responsible for managing port and pin directions and configuring their types
 *  for the AUTOSAR MCAL layer.
 *
 *  @warning    Ensure proper pin and port configurations before usage.
 ******************************************************************************/

/* ****************************************************************************
 *  INCLUDE GUARD
 * ****************************************************************************/
#ifndef PORT_H_
#define PORT_H_

/* ****************************************************************************
 *  INCLUDES
 * ****************************************************************************/

/* AUTOSAR Standard Types Header File */
#include "Std_Types.h"

/* The Pre-Compiled Configuration Header file for the Port Driver */
#include "Port_Cfg.h"

/* ****************************************************************************
 *  MACRO DEFINITIONS AND CONFIGURATIONS
 * ****************************************************************************/

/* ****************************************************************
 * Driver Metadata
 * ****************************************************************/

/* Vendor ID */
#define PORT_VENDOR_ID                            (1000U)

/* Module ID */
#define PORT_MODULE_ID                            (124U)

/* Instance ID */
#define PORT_INSTANCE_ID                          (0U)

/* ****************************************************************
 * Version
 * ****************************************************************/

/* Module Version 1.0.0 */
#define PORT_SW_MAJOR_VERSION                     (1U)
#define PORT_SW_MINOR_VERSION                     (0U)
#define PORT_SW_PATCH_VERSION                     (0U)

/* AUTOSAR Version 4.0.3 */
#define PORT_AR_RELEASE_MAJOR_VERSION             (4U)
#define PORT_AR_RELEASE_MINOR_VERSION             (0U)
#define PORT_AR_RELEASE_PATCH_VERSION             (3U)

/* ****************************************************************
 * Port Status
 * ****************************************************************/

#define PORT_INITIALIZED                          (1U)
#define PORT_NOT_INITIALIZED                      (0U)

/* ****************************************************************
 * Compatibilities
 * ****************************************************************/

/* Checking AUTOSAR version compatibility between Port.h & Std_Types.h */
#if ((PORT_AR_RELEASE_MAJOR_VERSION != STD_TYPES_AR_RELEASE_MAJOR_VERSION) \
  || (PORT_AR_RELEASE_MINOR_VERSION != STD_TYPES_AR_RELEASE_MINOR_VERSION) \
  || (PORT_AR_RELEASE_PATCH_VERSION != STD_TYPES_AR_RELEASE_PATCH_VERSION))
    #error "The AUTOSAR Version of Std_Types.h does not match the expected version in Port.h"
#endif

/* Checking software version compatibility between Port.h & Port_Cfg.h */
#if ((PORT_SW_MAJOR_VERSION != PORT_CFG_SW_MAJOR_VERSION) \
  || (PORT_SW_MINOR_VERSION != PORT_CFG_SW_MINOR_VERSION) \
  || (PORT_SW_PATCH_VERSION != PORT_CFG_SW_PATCH_VERSION))
    #error "The Software Version of Port_Cfg.h does not match the expected version in Port.h"
#endif

/* Checking AUTOSAR version compatibility between Port.h & Port_Cfg.h */
#if ((PORT_AR_RELEASE_MAJOR_VERSION != PORT_CFG_AR_RELEASE_MAJOR_VERSION) \
  || (PORT_AR_RELEASE_MINOR_VERSION != PORT_CFG_AR_RELEASE_MINOR_VERSION) \
  || (PORT_AR_RELEASE_PATCH_VERSION != PORT_CFG_AR_RELEASE_PATCH_VERSION))
    #error "The AUTOSAR Version of Port_Cfg.h does not match the expected version in Port.h"
#endif

/* Non-AUTOSAR files */
#include "common_macros.h"

/* ****************************************************************
 * API Service ID Macros
 * ****************************************************************/

/* Service ID for Port_Init API */
#define PORT_INIT_SID                       (uint8)(0x00)

/* Service ID for Port_SetPinDirection API */
#define PORT_SET_PIN_DIRECTION_SID          (uint8)(0x01)

/* Service ID for Port_RefreshPortDirection API */
#define PORT_REFRESH_PIN_DIRECTION_SID      (uint8)(0x02)

/* Service ID for Port_GetVersionInfo API */
#define PORT_GET_VERSION_INFO_SID           (uint8)(0x03)

/* Service ID for Port_SetPinMode API */
#define PORT_SET_PIN_MODE_SID               (uint8)(0x04)

/* ****************************************************************
 * DET ERROR CODES
 * ****************************************************************/

/* Invalid Port Pin ID requested */
#define PORT_E_PARAM_PIN                    (uint8)(0x0A)

/* Port Pin not configured as changeable */
#define PORT_E_DIRECTION_UNCHANGEABLE       (uint8)(0x0B)

/* API Port_Init service called with wrong parameter */
#define PORT_E_PARAM_CONFIG                 (uint8)(0x0C)

/* API Port_SetPinMode service called when mode is unchangeable */
#define PORT_E_PARAM_INVALID_MODE           (uint8)(0x0D)
#define PORT_E_MODE_UNCHANGEABLE            (uint8)(0x0E)

/* API service called without module initialization */
#define PORT_E_UNINIT                       (uint8)(0x0F)

/* APIs called with a Null Pointer */
#define PORT_E_PARAM_POINTER                (uint8)(0x10)

/* ****************************************************************************
 *  TYPE DEFINITIONS
 * ****************************************************************************/

/* A type definition for Port_ChannelType used by the PORT APIs */
typedef uint8 Port_ChannelType;

/* A type definition for Port_PortType used by the PORT APIs */
typedef uint8 Port_PortType;

/* A type definition for Port_PinLevelType used by the PORT APIs */
typedef uint8 Port_PinLevelType;

/* A type definition for Port_PinType used by the PORT APIs */
typedef uint8 Port_PinType;

/* A type definition for Port_PinModeType used by the PORT APIs */
typedef uint8 Port_PinModeType;

/*
 * @Name:           Port_PinDirectionType
 * @Kind:           Enumeration
 * @Range:          0 - 1
 * @Description:
 * Defines possible directions of a port pin.
 * @Available via: PORT.h
 */
typedef enum
{
    PORT_PIN_IN = 0,
    PORT_PIN_OUT = 1
} Port_PinDirectionType;

/*
 * @Name:           Port_InternalResistor
 * @Kind:           Enumeration
 * @Range:          0 - 2
 * @Description:
 * Defines possible resistor configurations.
 * @Available via: PORT.h
 */
typedef enum
{
    RESISTOR_OFF = 0,
    PULL_UP = 1,
    PULL_DOWN = 2
} Port_InternalResistorType;

typedef struct
{
    Port_PortType Port_Num;         /* Port ID (PORTA, PORTB, etc.) */
    Port_ChannelType Ch_Num;        /* Pin Number (0-7) */
    Port_PinModeType Mode;          /* Pin Mode (DIO, ADC, SPI, UART, etc.) */
    Port_PinDirectionType Direction; /* Input or Output */
    Port_PinLevelType InitialValue; /* Initial level (only for DIO) */
    boolean Direction_Changeable;   /* TRUE = Can change direction */
    boolean Mode_Changeable;        /* TRUE = Can change mode */
    Port_InternalResistorType Resistor;
} Port_ConfigChannel;

/*
 * @Name:           Port_ConfigType
 * @Kind:           Structure
 * @Elements:       Hardware Dependent Structure
 * @Description:
 * External data structure containing the initialization data for the PORT Driver.
 * @Available via:  Port.h
 */
typedef struct
{
    Port_ConfigChannel Pins[PORT_CONFIGURED_CHANNELS]; /* Array of pin configurations */
} Port_ConfigType;

/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/

void Port_Init(const Port_ConfigType* ConfigPtr);
void Port_setPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);
void Port_RefreshPortDirection(void);
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo);
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);

/*******************************************************************************
 *                       External Variables                                    *
 *******************************************************************************/

/* Extern PB structures to be used by the Port and other modules */
extern const Port_ConfigType Port_Configuration;

#endif /* PORT_H_ */

