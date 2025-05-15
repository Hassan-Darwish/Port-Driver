# AUTOSAR Port Driver Configuration – TIVA-C (TM4C123GH6PM)

This repository contains the configuration files for the **AUTOSAR Port Driver** designed for the TIVA-C Cortex M4 (TM4C123GH6PM) microcontroller. It follows AUTOSAR MCAL standards and provides both **pre-compile** and **post-build** configuration support.

## 📁 Project Structure

├── Port.h               # Main Port driver header file
├── Port_Cfg.h           # Configuration header file (Pre-compile options)
├── Port_PBcfg.c         # Post-build configuration source file
├── Dio_Cfg.h            # DIO module config (referenced for pin definitions)


##  Features

- AUTOSAR-compliant Port Driver configuration
- Support for both Pre-Compile and Post-Build configuration
- Strict AUTOSAR and software version compatibility checks
- Configurable direction, mode, initial level, and internal resistor for each pin
- External DIO configuration compatibility (via `Dio_Cfg.h`)

##  Configuration Details

- **AUTOSAR Version:** 4.0.3
- **Software Version:** 1.0.0
- **Configured Channels:** 2 pins

### Configured Pins

#### 1. LED1
| Attribute         | Value              |
|------------------|--------------------|
| Port/Channel      | `DioConf_LED1_*`   |
| Mode              | Digital I/O        |
| Direction         | Output             |
| Initial Level     | High               |
| Direction Changeable | TRUE           |
| Mode Changeable      | TRUE           |
| Internal Resistor     | OFF            |

#### 2. SW1 (Push Button)
| Attribute         | Value              |
|------------------|--------------------|
| Port/Channel      | `DioConf_SW1_*`    |
| Mode              | Digital I/O        |
| Direction         | Input              |
| Initial Level     | Low                |
| Direction Changeable | FALSE          |
| Mode Changeable      | TRUE           |
| Internal Resistor     | Pull-Up         |

## 🛠 Dependencies

- `Std_Types.h`
- `Dio.h`
- `Port.h`
- `Port_Cfg.h`
- `Dio_Cfg.h`

Ensure that your project includes the standard AUTOSAR MCAL headers and that the `DioConf_*` macros are defined in `Dio_Cfg.h`.

##  Author

**Hassan Darwish**  
Date: Feb 2025  
Module: AUTOSAR MCAL – Port Driver (TIVA-C)

---

This project is intended for educational and training purposes and follows the AUTOSAR layered architecture principles. Feel free to use and adapt it for your own learning and embedded software projects.
