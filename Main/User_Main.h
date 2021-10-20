/**
 *  @file User_Main.h
 *
 *  @date 2021/10/1
 *
 *  @author Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief None
 *  
 *  @version v1.0
 */
#ifndef USER_MAIN_H
#define USER_MAIN_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /*need definition of uint8_t*/
#include <stddef.h> /*need definition of NULL*/
#include <stdbool.h>/*need definition of BOOL*/
#include <stdio.h>  /*if need printf*/
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< if need INT_MAX*/
/** Private includes ---------------------------------------------------------*/
#include "CircularQueue.h"
#include "SPI_Audio_Port.h"
#include "Timer_Port.h"
#include "UART_Port.h"
#include "Audio_Debug.h"   
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/* AUDIO EVENT */
#define AUDIO_RECEIVED_OK_EVENT (1U << 0)
  
#ifndef SECTION
  #ifdef __CC_ARM                        /* ARM Compiler */
      #define SECTION(x)                 __attribute__((section(x)))
      #define USED                       __attribute__((used))
      #define UNUSED_CODE                __attribute__((unused))
  #elif defined (__IAR_SYSTEMS_ICC__)    /* for IAR Compiler */
      #define SECTION(x)                 @ x
      #define USED                       __root
      #define UNUSED_CODE                     
  #elif defined (__GNUC__)               /* GNU GCC Compiler */
      #define SECTION(x)                 __attribute__((section(x)))
      #define USED                       __attribute__((used))
      #define UNUSED_CODE                __attribute__((unused))
  #else
      #error not supported tool chain
  #endif /* __CC_ARM */
#endif
  
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*初始化入口*/
void User_Main_Init(void);
/*主循环入口*/
void User_Main_Loop(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
