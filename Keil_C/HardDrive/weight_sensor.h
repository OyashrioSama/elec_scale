//weight_sensor.h
/**
 ******************************************************************************
 * @file    weight_sensor.h
 * @author  Embedded Developer
 * @version V1.0.0
 * @date    2025-01-20
 * @brief   电子秤重量传感器驱动头文件
 ******************************************************************************
 */

#ifndef __WEIGHT_SENSOR_H
#define __WEIGHT_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sc32f1xxx_op.h"
#include "sc32f1xxx_adc.h"
#include "sc32f1xxx_rcc.h"

/** @defgroup 重量传感器相关定义
  * @{
  */
#define WEIGHT_SAMPLE_BUFFER_SIZE    1024    // 滑动窗口大小
#define WEIGHT_ADC_REF_VOLTAGE      2.048f  // ADC参考电压2.048V
#define WEIGHT_ADC_RESOLUTION       16384   // 14位ADC分辨率(2^14)

/** @defgroup 传感器校准参数
  * @{
  */
typedef struct {
    uint32_t ZeroPoint;          // 零点校准值
    uint32_t FullScale;          // 满量程校准值
    float ScaleFactor;           // 比例因子(g/count)
    uint16_t TareValue;          // 去皮值
} Weight_CalibTypeDef;

/**
  * @}
  */

/** @defgroup 重量传感器初始化结构体
  * @{
  */
typedef struct {
    OP_TypeDef* OPx;                    // 运放实例
    ADC_TypeDef* ADCx;                  // ADC实例
    uint32_t ADC_Channel;               // ADC输入通道
    OP_PGAGain_TypeDef OP_Gain;         // 运放增益
    FunctionalState AutoCalib;          // 自动校准使能
} WeightSensor_InitTypeDef;

/**
  * @}
  */

/** @defgroup 导出函数声明
  * @{
  */

/* 初始化函数 */
void WeightSensor_Init(WeightSensor_InitTypeDef* WeightSensor_InitStruct);
void WeightSensor_OPInit(OP_TypeDef* OPx, OP_PGAGain_TypeDef Gain);
void WeightSensor_ADCInit(ADC_TypeDef* ADCx, uint32_t Channel);

/* 采样和滤波函数 */
uint16_t WeightSensor_ReadRawADC(void);
uint32_t WeightSensor_SlidingWindowFilter(void);
float WeightSensor_GetVoltage(void);

/* 校准函数 */
void WeightSensor_Tare(void);
void WeightSensor_CalibrateZero(void);
void WeightSensor_CalibrateFullScale(uint32_t knownWeight);

/* 重量获取函数 */
float WeightSensor_GetWeightGram(void);
uint32_t WeightSensor_GetWeightCount(void);

/* 状态函数 */
FlagStatus WeightSensor_IsDataReady(void);
void WeightSensor_StartConversion(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __WEIGHT_SENSOR_H */
