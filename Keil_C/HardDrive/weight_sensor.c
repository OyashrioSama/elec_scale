/**
 ******************************************************************************
 * @file    weight_sensor.c
 * @author  Embedded Developer
 * @version V1.0.0
 * @date    2025-01-20
 * @brief   电子秤重量传感器驱动实现
 ******************************************************************************
 */

#include "weight_sensor.h"
#include <string.h>

/** @defgroup 模块私有变量
  * @{
  */
static uint16_t SampleBuffer[WEIGHT_SAMPLE_BUFFER_SIZE];  // 采样缓冲区
static uint16_t SampleIndex = 0;                          // 采样索引
static uint32_t RunningSum = 0;                           // 滑动窗口累加和
static uint8_t BufferFilled = 0;                          // 缓冲区填充标志

static Weight_CalibTypeDef WeightCalib = {
    .ZeroPoint = 0,
    .FullScale = 16383,  // 默认满量程
    .ScaleFactor = 0.061f, // 默认比例因子(1kg/16383)
    .TareValue = 0
};

static ADC_TypeDef* ADC_Instance = ADC;

/**
  * @}
  */

/** @defgroup 私有函数声明
  * @{
  */
static uint32_t CalculateMovingAverage(uint16_t newSample);
/**
  * @}
  */

/**
  * @brief  重量传感器完整初始化
  * @param  WeightSensor_InitStruct: 初始化结构体指针
  * @retval 无
  */
void WeightSensor_Init(WeightSensor_InitTypeDef* WeightSensor_InitStruct)
{
    /* 保存实例指针 */
    ADC_Instance = WeightSensor_InitStruct->ADCx;
    
    /* 初始化运放 */
    WeightSensor_OPInit(WeightSensor_InitStruct->OPx, WeightSensor_InitStruct->OP_Gain);
    
    /* 初始化ADC */
    WeightSensor_ADCInit(WeightSensor_InitStruct->ADCx, WeightSensor_InitStruct->ADC_Channel);
    
    /* 清空采样缓冲区 */
    memset(SampleBuffer, 0, sizeof(SampleBuffer));
    SampleIndex = 0;
    RunningSum = 0;
    BufferFilled = 0;
    
    /* 如果需要自动校准 */
    if(WeightSensor_InitStruct->AutoCalib == ENABLE) {
        WeightSensor_CalibrateZero();
    }
}

/**
  * @brief  运放初始化配置
  * @param  OPx: 运放实例
  * @param  Gain: 运放增益
  * @retval 无
  */
void WeightSensor_OPInit(OP_TypeDef* OPx, OP_PGAGain_TypeDef Gain)
{
    OP_InitTypeDef OP_InitStruct;
    
    /* 运放结构体默认配置 */
    OP_InitStruct.OP_ShortCircuit = OP_ShortCircuit_OFF;  // 短路保护关闭
    OP_InitStruct.OP_FDBResisrance = OP_FDBResisrance_VSS; // 反馈电阻到VSS
    OP_InitStruct.OP_PGAGain = Gain;                      // 设置增益
    OP_InitStruct.OP_Posittive = OP_Posittive_OPP0;       // 正输入端OPP0
    OP_InitStruct.OP_Negative = OP_Negative_OPN;          // 负输入端OPN
    OP_InitStruct.OP_Output = OP_Output_ON;               // 输出使能
    
    /* 应用配置 */
    OP_Init(OPx, &OP_InitStruct);
    
    /* 使能运放 */
    OP_Cmd(OPx, ENABLE);
    
    /* 运放偏置校准 */
    OP_OffsetSet(OPx);
}

/**
  * @brief  ADC初始化配置
  * @param  ADCx: ADC实例
  * @param  Channel: ADC通道
  * @retval 无
  */
void WeightSensor_ADCInit(ADC_TypeDef* ADCx, uint32_t Channel)
{
    ADC_InitTypeDef ADC_InitStruct;
    
    /* ADC结构体默认配置 */
    ADC_StructInit(&ADC_InitStruct);
    
    /* 自定义配置 - 使用正确的参考电压定义 */
    ADC_InitStruct.ADC_ConvMode = ADC_ConvMode_Single;    // 单次转换模式
    ADC_InitStruct.ADC_Prescaler = ADC_Prescaler_3CLOCK;  // 3分频
    ADC_InitStruct.ADC_VREF = ADC_VREF_2_048V;           // 2.048V参考电压（已修复）
    ADC_InitStruct.ADC_EAIN = ADC_EAIN_Less;              // 外部模拟输入
    
    /* 应用ADC配置 */
    ADC_Init(ADCx, &ADC_InitStruct);
    
    /* 设置ADC通道 - 修复枚举类型警告 */
    ADC_SetChannel(ADCx, (ADC_ChannelTypedef)Channel);
    
    /* 使能ADC */
    ADC_Cmd(ADCx, ENABLE);
    
    /* 使能ADC转换完成中断 */
    ADC_ITConfig(ADCx, ADC_IT_ADCIF, ENABLE);
}

/**
  * @brief  读取原始ADC值
  * @param  无
  * @retval ADC原始采样值
  */
uint16_t WeightSensor_ReadRawADC(void)
{
    /* 启动ADC转换 */
    ADC_SoftwareStartConv(ADC_Instance);
    
    /* 等待转换完成 */
    while(ADC_GetFlagStatus(ADC_Instance, ADC_Flag_ADCIF) == RESET) {
        // 等待转换完成
    }
    
    /* 清除标志位 */
    ADC_ClearFlag(ADC_Instance, ADC_Flag_ADCIF);
    
    /* 返回转换结果 */
    return ADC_GetConversionValue(ADC_Instance);
}

/**
  * @brief  滑动窗口平均滤波算法
  * @param  无
  * @retval 滤波后的ADC值
  */
uint32_t WeightSensor_SlidingWindowFilter(void)
{
    uint16_t newSample = WeightSensor_ReadRawADC();
    return CalculateMovingAverage(newSample);
}

/**
  * @brief  计算滑动窗口平均值（私有函数）
  * @param  newSample: 新采样值
  * @retval 滑动平均值
  */
static uint32_t CalculateMovingAverage(uint16_t newSample)
{
    /* 减去即将被替换的旧值 */
    if(BufferFilled) {
        RunningSum -= SampleBuffer[SampleIndex];
    }
    
    /* 加上新采样值 */
    RunningSum += newSample;
    
    /* 更新缓冲区 */
    SampleBuffer[SampleIndex] = newSample;
    SampleIndex++;
    
    /* 循环缓冲区 */
    if(SampleIndex >= WEIGHT_SAMPLE_BUFFER_SIZE) {
        SampleIndex = 0;
        BufferFilled = 1;  // 标记缓冲区已填满
    }
    
    /* 计算平均值 */
    if(BufferFilled) {
        return RunningSum / WEIGHT_SAMPLE_BUFFER_SIZE;
    } else {
        return RunningSum / (SampleIndex + 1);
    }
}

/**
  * @brief  获取电压值
  * @param  无
  * @retval 计算得到的电压值(V)
  */
float WeightSensor_GetVoltage(void)
{
    uint32_t filteredValue = WeightSensor_SlidingWindowFilter();
    return (float)filteredValue * WEIGHT_ADC_REF_VOLTAGE / WEIGHT_ADC_RESOLUTION;
}

/**
  * @brief  去皮功能（清零）
  * @param  无
  * @retval 无
  */
void WeightSensor_Tare(void)
{
    WeightCalib.TareValue = WeightSensor_SlidingWindowFilter();
}

/**
  * @brief  零点校准
  * @param  无
  * @retval 无
  */
void WeightSensor_CalibrateZero(void)
{
    /* 采集多个样本求平均作为零点 */
    uint32_t sum = 0;
    uint16_t samples = 100;
    
    for(uint16_t i = 0; i < samples; i++) {
        sum += WeightSensor_ReadRawADC();
    }
    
    WeightCalib.ZeroPoint = sum / samples;
    WeightCalib.TareValue = WeightCalib.ZeroPoint;  // 同时设置去皮值
}

/**
  * @brief  满量程校准
  * @param  knownWeight: 已知重量(g)
  * @retval 无
  */
void WeightSensor_CalibrateFullScale(uint32_t knownWeight)
{
    if(knownWeight == 0) return;
    
    /* 获取当前ADC值（假设已放置已知重量） */
    uint32_t currentADC = WeightSensor_SlidingWindowFilter();
    
    /* 计算有效ADC值 */
    uint32_t effectiveADC = currentADC - WeightCalib.ZeroPoint;
    
    if(effectiveADC > 0) {
        /* 计算比例因子 */
        WeightCalib.FullScale = currentADC;
        WeightCalib.ScaleFactor = (float)knownWeight / effectiveADC;
    }
}

/**
  * @brief  获取重量值（克）
  * @param  无
  * @retval 重量值(g)
  */
float WeightSensor_GetWeightGram(void)
{
    uint32_t currentADC = WeightSensor_SlidingWindowFilter();
    
    /* 减去去皮值 */
    int32_t netADC = currentADC - WeightCalib.TareValue;
    
    if(netADC <= 0) return 0.0f;
    
    /* 转换为重量 */
    return (float)netADC * WeightCalib.ScaleFactor;
}

/**
  * @brief  获取原始计数值
  * @param  无
  * @retval 原始ADC计数值
  */
uint32_t WeightSensor_GetWeightCount(void)
{
    return WeightSensor_SlidingWindowFilter();
}

/**
  * @brief  检查数据是否就绪
  * @param  无
  * @retval 数据状态(SET/RESET)
  */
FlagStatus WeightSensor_IsDataReady(void)
{
    return ADC_GetFlagStatus(ADC_Instance, ADC_Flag_ADCIF);
}

/**
  * @brief  启动ADC转换
  * @param  无
  * @retval 无
  */
void WeightSensor_StartConversion(void)
{
    ADC_SoftwareStartConv(ADC_Instance);
}
