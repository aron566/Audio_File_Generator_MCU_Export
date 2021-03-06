/**
 *  @file SPI_Audio_Port.c
 *
 *  @date 2021/10/7
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief None
 *
 *  @details None.
 *
 *  @version v1.0
 */
/** Includes -----------------------------------------------------------------*/
#include <math.h>
/* Private includes ----------------------------------------------------------*/
#include "SPI_Audio_Port.h"
#include "USB_Audio_Port.h"
#include "main.h"
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define SIN_WAVE_SAMPLE_RATE	16000 /**< 16K采样*/
#define SIN_WAVE_FQ				    250   /**< 100Hz正弦*/
#define SIN_WAVE_MAX_POINTS		SIN_WAVE_SAMPLE_RATE/SIN_WAVE_FQ
#define SIN_WAVE_DB_VAL 		  60.l
  
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
/** Private variables --------------------------------------------------------*/
/*音频缓冲区*/
static int16_t Audio_Data_Rec_Buf[STEREO_FRAME_SIZE];
/*测试音频缓冲区*/
static int16_t Sin_Wave_PCM_Buf[SIN_WAVE_MAX_POINTS];
/*音频发送区*/
static int16_t Audio_Data_Send_Buf[STEREO_FRAME_SIZE] SECTION(".USE_DMA_BUF_SPACE");
/*音频调试缓冲区*/
static int16_t Debug_Auido_Buf[STEREO_FRAME_SIZE];
/*音频事件标志位*/
//static volatile int16_t *Current_Opt_Send_Buf_Sel = Audio_Data_Send_Buf;
extern osEventFlagsId_t Audio_Rec_EventHandle;
/** Private function prototypes ----------------------------------------------*/
/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   正弦生成
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
static void Sin_Audio_Init(void)
{  
  float Average_Range = (float)pow(10.L, (double)((double)SIN_WAVE_DB_VAL - 94.f) / 20.L)*32767.f;
  for(int i = 0; i < SIN_WAVE_MAX_POINTS; i++)
  {
    Sin_Wave_PCM_Buf[i] = (int16_t)(Average_Range * sin(2.f*3.14f * SIN_WAVE_FQ / SIN_WAVE_SAMPLE_RATE * i));
  }
}

/**
  ******************************************************************
  * @brief   测试USB音频数据
  * @param   None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
static void Test_Audio_Port_Put_Data(void)
{
  /*更新USB音频数据*/
  static int index = 0;
  for(int i = 0; i < MONO_FRAME_SIZE; i++)
  {
    Audio_Data_Send_Buf[i] = Sin_Wave_PCM_Buf[index];/**< TO LEFT*/
//    Audio_Data_Send_Buf[i+MONO_FRAME_SIZE] = Sin_Wave_PCM_Buf[index];/**< TO RIGHT*/
    index = ((index+1)%(SIN_WAVE_MAX_POINTS));
  }
}

/**
  ******************************************************************
  * @brief   发送数据接口
  * @param   Data 数据.
  * @param   Len 数据字节数.
  * @return  已发送长度.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-02
  ******************************************************************
  */
static uint32_t Send_Data_Func_Port(uint8_t *Data, uint32_t Len)
{
//  int16_t *Ptr = (int16_t *)Data;
//  /*分离左右通道数据*/
//  int16_t Left_Audio[MONO_FRAME_SIZE], Right_Audio[MONO_FRAME_SIZE];
//  int index = 0;
//  for(int i = 0; i < MONO_FRAME_SIZE; i++)
//  {
//    Left_Audio[i] = Ptr[index++];
//    Right_Audio[i] = Ptr[index++];
//  }
  /*发送音频数据到SPI Slave*/
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)Audio_Data_Send_Buf, MONO_FRAME_SIZE*sizeof(int16_t));
  return Len;
}

/**
  ******************************************************************
  * @brief   获取发送空闲状态
  * @param   None.
  * @return  true 空闲.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-02
  ******************************************************************
  */
static bool Get_Idel_State_Port(void)
{
  HAL_SPI_StateTypeDef State = HAL_SPI_GetState(&hspi1);
  if(State == HAL_SPI_STATE_BUSY_TX || State == HAL_SPI_STATE_BUSY)
  {
    return false;
  }
  return true;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   音频半传输完成中断
  * @param   [in]hspi 句柄
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  (void)(hspi);
}

/**
  ******************************************************************
  * @brief   音频传输完成中断
  * @param   [in]hspi 句柄
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  (void)(hspi);
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

/**
  ******************************************************************
  * @brief   音频接口启动
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void SPI_Audio_Port_Start(void)
{
  /*等待音频数据事件*/
  osEventFlagsWait (Audio_Rec_EventHandle, AUDIO_SEND_OK_EVENT, osFlagsWaitAny, osWaitForever);
  
  /*取出音频数据给SPI Slave*/
  Test_Audio_Port_Put_Data();
  Audio_Debug_Put_Data(Audio_Data_Send_Buf, Audio_Data_Send_Buf, 0);
}

/**
  ******************************************************************
  * @brief   音频接口初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void SPI_Audio_Port_Init(void)
{
  /* 正弦音频 */
  Sin_Audio_Init();
  
  /* 初始化音频调试接口 */
  Audio_Debug_Init((uint16_t *)Debug_Auido_Buf, Send_Data_Func_Port, Get_Idel_State_Port);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
