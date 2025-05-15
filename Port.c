/******************************************************************************
 *  @file       Port.c
 *  @author     Hassan Darwish
 *  @date       Feb 2025
 *  @brief      the Port Driver for TIVA-C Cortex M4
 *
 *  @details
 *  This Source file contains the implementation of the Port driver,
 *  responsible for managing port and pin directions and configuring their types
 *  for the AUTOSAR MCAL layer.
 *
 *  @warning    Ensure proper pins and ports configuration before usage.
 ******************************************************************************/

/******************************************************************************
 *  INCLUDES
 ******************************************************************************/

/* Port driver header file */
#include "Port.h"

/* Port registers header file */
#include "Port_Regs.h"

#if (PORT_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR checking version compatibility between Det and Port Modules */
#if ((DET_AR_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 || (DET_AR_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 || (DET_AR_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Det.h does not match the expected version"
#endif

#endif

/******************************************************************************
 *  STATIC VARIABLES
 ******************************************************************************/

/* A global variable used for defining the port status */
static uint8 Port_Status = PORT_NOT_INITIALIZED;

/* A global variable holds the Configuration of the Port driver */
static const Port_ConfigType* Port_ConfigPtr = NULL_PTR;

/******************************************************************************
 *  FUNCTION DEFINITIONS
 ******************************************************************************/

/******************************************************************************
* @Service Name: Port_Init
* @Service ID[hex]: 0x00
* @Sync/Async: Synchronous
* @Reentrancy: Non-Reentrant
* @Parameters (in):  ConfigPtr - Pointer to the post-build configuration data
* @Parameters (out): None
* @Return value: None
* @Description: Initializes the Port Driver module and configures all pins
******************************************************************************/
void Port_Init(const Port_ConfigType* ConfigPtr)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Validate the pointer parameter */
    if (NULL_PTR == ConfigPtr)
    {
        /* Report error to DET: Invalid config pointer */
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_INIT_SID,
                        PORT_E_PARAM_CONFIG);
        return;
    }
#endif

    /* Save the pointer to the config structure globally so other functions can use it */
    Port_ConfigPtr = ConfigPtr;

    /* For safety, mark the driver as not initialized until we finish configuration */
    Port_Status = PORT_NOT_INITIALIZED;

    /* A local variable to track which ports have been enabled */
    uint8 portsEnabledMask = 0; /* each bit in this mask can represent a port A-F as needed */

    /* Loop through each pin in the config array */
    uint8 loop_idx;

    for (loop_idx = 0; loop_idx < PORT_CONFIGURED_CHANNELS; loop_idx++)
    {
        /* Extract the needed config fields */
        Port_PortType port_num  = Port_ConfigPtr->Pins[loop_idx].Port_Num;  /* 0..5 => A..F */
        Port_PinType pin_num   = Port_ConfigPtr->Pins[loop_idx].Ch_Num;     /* 0..7 => pin index */
        Port_PinDirectionType direction = Port_ConfigPtr->Pins[loop_idx].Direction;
        Port_PinModeType mode           = Port_ConfigPtr->Pins[loop_idx].Mode;
        Port_PinLevelType initVal       = Port_ConfigPtr->Pins[loop_idx].InitialValue;
        Port_InternalResistorType resistor = Port_ConfigPtr->Pins[loop_idx].Resistor;

        /* We assume the 6 least significant bits in 'portsEnabledMask' map to ports 0..5 */
        if ( (portsEnabledMask & (1 << port_num)) == 0 )
        {
            /* Enable clock */
            SYSCTL_RCGCGPIO_REG |= (1U << port_num);
            portsEnabledMask |= (uint8)(1U << port_num);
        }

        /* Get the base address pointer for the correct port */
        volatile uint32* PortGpio_Ptr = NULL_PTR;
        switch (port_num)
        {
            case 0: PortGpio_Ptr = (volatile uint32*)GPIO_PORTA_BASE_ADDRESS; break;
            case 1: PortGpio_Ptr = (volatile uint32*)GPIO_PORTB_BASE_ADDRESS; break;
            case 2: PortGpio_Ptr = (volatile uint32*)GPIO_PORTC_BASE_ADDRESS; break;
            case 3: PortGpio_Ptr = (volatile uint32*)GPIO_PORTD_BASE_ADDRESS; break;
            case 4: PortGpio_Ptr = (volatile uint32*)GPIO_PORTE_BASE_ADDRESS; break;
            case 5: PortGpio_Ptr = (volatile uint32*)GPIO_PORTF_BASE_ADDRESS; break;
            /* If invalid port_num, you could DET_ReportError and continue or return */
            default:
#if (PORT_DEV_ERROR_DETECT == STD_ON)
                Det_ReportError(PORT_MODULE_ID,
                                PORT_INSTANCE_ID,
                                PORT_INIT_SID,
                                PORT_E_PARAM_PIN);
#endif
                continue;
        }

        /* Unlock and commit for (PD7 or PF0) */
        if (((port_num == 3) && (pin_num == 7)) ||  /* PD7 */
            ((port_num == 5) && (pin_num == 0)))   /* PF0 */
        {
            /* Unlock the GPIOCR register */
            *(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;
            /* Commit the pin changes */
            SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET), pin_num);
        }


        /* Configure Direction (Input or Output) */
        if (direction == PORT_PIN_OUT)
        {
            SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIR_REG_OFFSET), pin_num);

            /* Set the initial value if it's an output pin */
            if (initVal == STD_HIGH)
            {
                SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DATA_REG_OFFSET), pin_num);
            }
            else
            {
                CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DATA_REG_OFFSET), pin_num);
            }
        }
        else
        {
            /* direction == PORT_PIN_IN */
            CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIR_REG_OFFSET), pin_num);

            /* Configure the resistor (pull-up, pull-down, or off) */
            if (resistor == PULL_UP)
            {
                SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET), pin_num);
                CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET), pin_num);
            }
            else if (resistor == PULL_DOWN)
            {
                SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET), pin_num);
                CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET), pin_num);
            }
            else
            {
                /* do nothing */
            }
        }
        switch (mode)
        {
        case PIN_MODE_DIO:
            /* Clear Analog Mode Select */
            CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET), pin_num);

            /* Disable Alternate Function */
            CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET), pin_num);

            /* Clear the PCTL nibble for this pin (4 bits per pin) */
            *(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0F << (pin_num * 4));

            /* Enable Digital I/O */
            SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET), pin_num);
            break;
        default:
            /* can do many cases for other modes like (ADC, UART, etc.)*/
#if (PORT_DEV_ERROR_DETECT == STD_ON)
            /* Det error: mode not supported or not found */
            Det_ReportError(PORT_MODULE_ID,
                            PORT_INSTANCE_ID,
                            PORT_INIT_SID,
                            PORT_E_PARAM_INVALID_MODE);
#endif

            break;
        }

    }

    /* Announcing that the Port driver has been initialized */
    Port_Status = PORT_INITIALIZED;
}

/******************************************************************************
* @Service Name: Port_setPinDirection
* @Service ID[hex]: 0x01
* @Sync/Async: Synchronous
* @Reentrancy: Reentrant
* @Parameters (in): Pin - Port Pin ID number
*                   Direction - Port Pin direction
* @Parameters (inout): None
* @Parameters (out): None
* @Return value: None
* @Description: Sets the port pin direction
* @Available via: Port.h
 ******************************************************************************/
void Port_setPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* 1. Check if the Port Driver is initialized */
    if (Port_Status == PORT_NOT_INITIALIZED)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_SET_PIN_DIRECTION_SID,
                        PORT_E_UNINIT);
        return;
    }

    /* 2. Check if the Pin ID is within the valid range */
    if (Pin >= PORT_CONFIGURED_CHANNELS)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_SET_PIN_DIRECTION_SID,
                        PORT_E_PARAM_PIN);
        return;
    }

    /* 3. Check if this pin’s direction can actually be changed at runtime */
    if (Port_ConfigPtr->Pins[Pin].Direction_Changeable == FALSE)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_SET_PIN_DIRECTION_SID,
                        PORT_E_DIRECTION_UNCHANGEABLE);
        return;
    }
#endif

    /*
     * If we reach here, the pin index is valid, the driver is initialized,
     * and the pin's direction is allowed to change.
     */

    /* 4. Extract the port number and pin number from the configuration */
    uint8 port_num = Port_ConfigPtr->Pins[Pin].Port_Num;  /* Which port (0..5) */
    uint8 pin_num  = Port_ConfigPtr->Pins[Pin].Ch_Num;    /* Which pin (0..7)  */

    /* 5. Get the base address of the required port */
    volatile uint32* PortGpio_Ptr = NULL_PTR;
    switch (port_num)
    {
        case 0: PortGpio_Ptr = (volatile uint32*)GPIO_PORTA_BASE_ADDRESS; break;
        case 1: PortGpio_Ptr = (volatile uint32*)GPIO_PORTB_BASE_ADDRESS; break;
        case 2: PortGpio_Ptr = (volatile uint32*)GPIO_PORTC_BASE_ADDRESS; break;
        case 3: PortGpio_Ptr = (volatile uint32*)GPIO_PORTD_BASE_ADDRESS; break;
        case 4: PortGpio_Ptr = (volatile uint32*)GPIO_PORTE_BASE_ADDRESS; break;
        case 5: PortGpio_Ptr = (volatile uint32*)GPIO_PORTF_BASE_ADDRESS; break;
        default:
            /* Should never happen if we validated Pin correctly, but just in case: */
#if (PORT_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(PORT_MODULE_ID,
                            PORT_INSTANCE_ID,
                            PORT_SET_PIN_DIRECTION_SID,
                            PORT_E_PARAM_PIN);
#endif
            return;
    }

    /* 6. Unlock/Commit if needed (e.g., for PD7 or PF0). */
    if (((port_num == 3) && (pin_num == 7)) ||  /* PD7 */
        ((port_num == 5) && (pin_num == 0)))   /* PF0 */
    {
        /* Unlock the GPIOCR register */
        *(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;
        SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET), pin_num);
    }
    /* port C0-C3 (JTAG) Do nothing */

    /* 7. Actually set or clear the DIR bit */
    if (Direction == PORT_PIN_OUT)
    {
        /* Set as output */
        SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIR_REG_OFFSET), pin_num);
    }
    else
    {
        /* Set as input */
        CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIR_REG_OFFSET), pin_num);
    }

}

/******************************************************************************
* @Service Name: Port_RefreshPortDirection
* @Service ID[hex]: 0x02
* @Sync/Async: Synchronous
* @Reentrancy: Non Reentrant
* @Parameters (in): None
* @Parameters (inout): None
* @Parameters (out): None
* @Return value: None
* @Description: Refreshes the direction of all unchangeable pins
******************************************************************************/
void Port_RefreshPortDirection(void)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /*  Check if the Port Driver is initialized */
    if (Port_Status == PORT_NOT_INITIALIZED)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_REFRESH_PIN_DIRECTION_SID,
                        PORT_E_UNINIT);
        return;
    }
#endif

     uint8 loop_idx;
    /* Loop through all configured channels */
    for (loop_idx = 0; loop_idx < PORT_CONFIGURED_CHANNELS; loop_idx++)
    {
        /* Check if direction is unchangeable */
        if (Port_ConfigPtr->Pins[loop_idx].Direction_Changeable == FALSE)
        {
            /* Re-apply the originally configured direction */
            uint8 port_num = Port_ConfigPtr->Pins[loop_idx].Port_Num;
            uint8 pin_num  = Port_ConfigPtr->Pins[loop_idx].Ch_Num;
            Port_PinDirectionType configuredDirection = Port_ConfigPtr->Pins[loop_idx].Direction;

            /* Get the base address of the required port */
            volatile uint32* PortGpio_Ptr = NULL_PTR;
            switch (port_num)
            {
                case 0: PortGpio_Ptr = (volatile uint32*)GPIO_PORTA_BASE_ADDRESS; break;
                case 1: PortGpio_Ptr = (volatile uint32*)GPIO_PORTB_BASE_ADDRESS; break;
                case 2: PortGpio_Ptr = (volatile uint32*)GPIO_PORTC_BASE_ADDRESS; break;
                case 3: PortGpio_Ptr = (volatile uint32*)GPIO_PORTD_BASE_ADDRESS; break;
                case 4: PortGpio_Ptr = (volatile uint32*)GPIO_PORTE_BASE_ADDRESS; break;
                case 5: PortGpio_Ptr = (volatile uint32*)GPIO_PORTF_BASE_ADDRESS; break;
                default:
                    /* In case of invalid config, just skip or report error */
#if (PORT_DEV_ERROR_DETECT == STD_ON)
                    Det_ReportError(PORT_MODULE_ID,
                                    PORT_INSTANCE_ID,
                                    PORT_REFRESH_PIN_DIRECTION_SID,
                                    PORT_E_PARAM_PIN);
#endif
                    continue; /* Move to next channel */
            }

            /* If the pin is PD7 or PF0, unlock if needed to change direction bits */
            if (((port_num == 3) && (pin_num == 7)) ||  /* PD7 */
                ((port_num == 5) && (pin_num == 0)))   /* PF0 */
            {
                *(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;
                SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET), pin_num);
            }
            /* If port C0-C3 (JTAG), typically do nothing to avoid issues */

            /* Re-apply direction */
            if (configuredDirection == PORT_PIN_OUT)
            {
                SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIR_REG_OFFSET), pin_num);
            }
            else /* PORT_PIN_IN */
            {
                CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIR_REG_OFFSET), pin_num);
            }
        }
    }
}

/******************************************************************************
* @Service Name: Port_GetVersionInfo
* @Service ID[hex]: 0x03
* @Sync/Async: Synchronous
* @Reentrancy: Reentrant
* @Parameters (in): versioninfo - Pointer to where to store the version info
* @Parameters (out): None
* @Return value: None
* @Description: Returns the version info of this module
******************************************************************************/
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Check for NULL pointer */
    if (versioninfo == NULL_PTR)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_GET_VERSION_INFO_SID,
                        PORT_E_PARAM_POINTER);
        return;
    }
#endif

    /* Fill the version information structure */
    versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
    versioninfo->moduleID = (uint16)PORT_MODULE_ID;
    versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
}

/******************************************************************************
* @Service Name: Port_SetPinMode
* @Service ID[hex]: 0x04
* @Sync/Async: Synchronous
* @Reentrancy: Non Reentrant
* @Parameters (in): Pin  - Port Pin ID number
*                   Mode - New Port Pin mode to be set on this pin
* @Parameters (inout): None
* @Parameters (out): None
* @Return value: None
* @Description: Sets the port pin mode at runtime (e.g., from DIO to ADC, UART, etc.)
******************************************************************************/
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Check if the Port Driver is initialized */
    if (Port_Status == PORT_NOT_INITIALIZED)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_SET_PIN_MODE_SID,
                        PORT_E_UNINIT);
        return;
    }

    /* Check if Pin is within valid range */
    if (Pin >= PORT_CONFIGURED_CHANNELS)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_SET_PIN_MODE_SID,
                        PORT_E_PARAM_PIN);
        return;
    }

    /*  Check if this pin’s mode is changeable at runtime */
    if (Port_ConfigPtr->Pins[Pin].Mode_Changeable == FALSE)
    {
        Det_ReportError(PORT_MODULE_ID,
                        PORT_INSTANCE_ID,
                        PORT_SET_PIN_MODE_SID,
                        PORT_E_MODE_UNCHANGEABLE);
        return;
    }

#endif

    /* 4. Retrieve the Port and Pin from the config */
    uint8 port_num = Port_ConfigPtr->Pins[Pin].Port_Num;
    uint8 pin_num  = Port_ConfigPtr->Pins[Pin].Ch_Num;

    /* 5. Get the base address for the port */
    volatile uint32* PortGpio_Ptr = NULL_PTR;
    switch (port_num)
    {
        case 0: PortGpio_Ptr = (volatile uint32*)GPIO_PORTA_BASE_ADDRESS; break;
        case 1: PortGpio_Ptr = (volatile uint32*)GPIO_PORTB_BASE_ADDRESS; break;
        case 2: PortGpio_Ptr = (volatile uint32*)GPIO_PORTC_BASE_ADDRESS; break;
        case 3: PortGpio_Ptr = (volatile uint32*)GPIO_PORTD_BASE_ADDRESS; break;
        case 4: PortGpio_Ptr = (volatile uint32*)GPIO_PORTE_BASE_ADDRESS; break;
        case 5: PortGpio_Ptr = (volatile uint32*)GPIO_PORTF_BASE_ADDRESS; break;
        default:
            /* Shouldn't happen if config is valid */
#if (PORT_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(PORT_MODULE_ID,
                            PORT_INSTANCE_ID,
                            PORT_SET_PIN_MODE_SID,
                            PORT_E_PARAM_PIN);
#endif
            return;
    }

    /* 6. Unlock the pin (PD7 / PF0) */
    if (((port_num == 3) && (pin_num == 7)) ||  /* PD7 */
        ((port_num == 5) && (pin_num == 0)))   /* PF0 */
    {
        *(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;
        SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET), pin_num);
    }


    switch (Mode)
    {
        case PIN_MODE_DIO:
            /* 1) Disable analog function */
            CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET), pin_num);

            /* 2) Disable alternate function */
            CLEAR_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET), pin_num);

            /* 3) Clear PCTL for this pin (4 bits per pin) */
            *(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0F << (pin_num * 4));

            /* 4) Enable digital function */
            SET_BIT(*(volatile uint32*)((volatile uint8*)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET), pin_num);
            break;

        default:

            /* can do many cases for other modes like (ADC, UART, etc.)*/

#if (PORT_DEV_ERROR_DETECT == STD_ON)
            /* Det error: mode not supported or not found */
            Det_ReportError(PORT_MODULE_ID,
                            PORT_INSTANCE_ID,
                            PORT_SET_PIN_MODE_SID,
                            PORT_E_PARAM_INVALID_MODE);
#endif
            /* Do nothing, or revert to default, or return */
            return;
    }

}
