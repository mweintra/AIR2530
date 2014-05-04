/**
 * @ingroup hal
 * @{
 * @file hal.h
 *
 * @brief Hardware Abstraction Layer (HAL) file
 *
 * This library is designed to make it easy to use the same code across multiple hardware platforms.
 * This file allows you to use project properties to select which HAL file gets included.
 *
 * @note If changing hardware platforms add your file in the list below. You must also set the board
 * version (e.g. MY_SUPER_DUPER_PROTOTYPE_BOARD) in Project Properties. Also be sure to add your hal
 * file to the IAR project. We have more hal files available - contact support at email below.
 * @see hal_helper.c for utilities to assist when changing hardware platforms
*
* $Rev: 2067 $
* $Author: dsmith $
* $Date: 2014-03-06 10:18:48 -0800 (Thu, 06 Mar 2014) $
*
* @section support Support
* Please refer to the wiki at http://teslacontrols.com/mw/ for more information. Additional support
* is available via email at the following addresses:
* - Questions on how to use the product: AIR@anaren.com
* - Feature requests, comments, and improvements:  featurerequests@teslacontrols.com
* - Consulting engagements: sales@teslacontrols.com
*
* @section license License
* Copyright (c) 2014 Tesla Controls. All rights reserved. This Software may only be used with an 
* Anaren A2530E24AZ1, A2530E24CZ1, A2530R24AZ1, or A2530R24CZ1 module. Redistribution and use in 
* source and binary forms, with or without modification, are subject to the Software License 
* Agreement in the file "anaren_eula.txt"
* 
* YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” 
* WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
* WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO 
* EVENT SHALL ANAREN MICROWAVE OR TESLA CONTROLS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, 
* STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR 
* INDIRECT DAMAGES OR EXPENSE INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, 
* PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE 
* GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY 
* DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/
    #include "../HAL/hal_version.h"


#if defined(__TM4C129XNCZAD__)

#elif defined(__TM4C1294NCPDT__)

#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)
    #include "../HAL/hal_ek-lm4f120xl.h"	  //Tiva LaunchPad
    #include "../HAL/hal_stellaris_softi2c.h"
#define PART_LM4F120H5QR
#else
#error "**** No PART defined or unsupported PART ****"
#endif




/* @} */
