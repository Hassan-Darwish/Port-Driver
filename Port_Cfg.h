/******************************************************************************
 *  @file       Port_Cfg.h
 *  @author     Hassan Darwish
 *  @date       Feb 2025
 *  @brief      the configuration Header file for Port Driver of TIVA-C Cortex M4
 *
 *  @details
 *  This Header file contains the configurations and Module version
 *  for the Port Driver for the AUTOSAR MCAL layer.
 ******************************************************************************/
#ifndef PORT_CFG_H_
#define PORT_CFG_H_

/* Module Version 1.0.0 */
#define PORT_CFG_SW_MAJOR_VERSION     (1U)
#define PORT_CFG_SW_MINOR_VERSION     (0U)
#define PORT_CFG_SW_PATCH_VERSION     (0U)

/* AUTOSAR Version 4.0.3 */
#define PORT_CFG_AR_RELEASE_MAJOR_VERSION  (4U)
#define PORT_CFG_AR_RELEASE_MINOR_VERSION  (0U)
#define PORT_CFG_AR_RELEASE_PATCH_VERSION  (3U)

/* Pre-compile or Post-Build Configuration Switches */
#define PORT_DEV_ERROR_DETECT         (STD_ON)
#define PORT_VERSION_INFO_API         (STD_OFF)

/* Number of pins configured in the Port_ConfigType array */
#define PORT_CONFIGURED_CHANNELS      (2U)

#define PIN_MODE_DIO                   (0U)
/* Include Dio_Cfg.h so we can use the same numeric values for LED/Switch */
#include "Dio_Cfg.h"


#endif /* PORT_CFG_H_ */
