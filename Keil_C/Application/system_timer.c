//system_timer.c

#include "system_timer.h"
#include "sc32f1xxx_btm.h"

/** @defgroup 模块私有变量
  * @{
  */
SystemTimer_t systemTimer;

/**
  * @}
  */

/**
  * @brief  系统定时器初始化
  */
void SystemTimer_Init(void)
{
    // 初始化计数器变量
    systemTimer.msCounter = 0;
    systemTimer.lastOverweightCheck = 0;
    systemTimer.lastActivityTime = 0;
    systemTimer.lastSystemUpdate = 0;
    systemTimer.loopCounter = 0;
    
    // 初始化BTM定时器（62.5ms中断）
    BTM_FSConfig(BTM, BTM_FreqSelect_62_5MS);
    BTM_ITConfig(BTM, BTM_IT_INT, ENABLE);
    BTM_Cmd(BTM, ENABLE);
    
    // 设置初始活动时间
    ResetInactivityTimer();
}

uint32_t GetSystemTimeMs(void)
{
    return systemTimer.msCounter;
}

uint32_t GetSystemTime(void)
{
    return systemTimer.loopCounter;
}

uint32_t SystemTimer_GetElapsedMs(uint32_t startTime)
{
    uint32_t currentTime = GetSystemTimeMs();
    
    if (currentTime >= startTime) {
        return currentTime - startTime;
    } else {
        return (0xFFFFFFFF - startTime) + currentTime + 1;
    }
}

uint32_t SystemTimer_GetElapsedLoops(uint32_t startCount)
{
    uint32_t currentCount = GetSystemTime();
    
    if (currentCount >= startCount) {
        return currentCount - startCount;
    } else {
        return (0xFFFFFFFF - startCount) + currentCount + 1;
    }
}

void ResetInactivityTimer(void)
{
    systemTimer.lastActivityTime = GetSystemTimeMs();
}

uint32_t GetInactivityTime(void)
{
    return SystemTimer_GetElapsedMs(systemTimer.lastActivityTime);
}

uint8_t IsInactivityTimeout(void)
{
    return (GetInactivityTime() >= INACTIVITY_TIMEOUT_MS);
}

uint8_t IsOverweightCheckTime(void)
{
    return (SystemTimer_GetElapsedMs(systemTimer.lastOverweightCheck) >= OVERWEIGHT_CHECK_INTERVAL);
}

void UpdateOverweightCheckTime(void)
{
    systemTimer.lastOverweightCheck = GetSystemTimeMs();
}

uint8_t IsSystemUpdateTime(void)
{
    return (SystemTimer_GetElapsedMs(systemTimer.lastSystemUpdate) >= SYSTEM_UPDATE_INTERVAL);
}

void UpdateSystemUpdateTime(void)
{
    systemTimer.lastSystemUpdate = GetSystemTimeMs();
}

void SystemTimer_Update(void)
{
    systemTimer.loopCounter++;
}
