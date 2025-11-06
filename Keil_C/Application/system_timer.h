//system_timer.h

#ifndef __SYSTEM_TIMER_H
#define __SYSTEM_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sc32f1xxx.h"
#include "sc32f1xxx_btm.h"

/** @defgroup 系统定时器相关定义
  * @{
  */
#define INACTIVITY_TIMEOUT_MS    60000   // 无操作超时时间60秒
#define OVERWEIGHT_CHECK_INTERVAL 1000   // 超重检查间隔1秒
#define SYSTEM_UPDATE_INTERVAL    100    // 系统状态更新间隔100ms

/** @}
  */

/** @defgroup 系统定时器结构体
  * @{
  */
typedef struct {
    volatile uint32_t msCounter;           // 毫秒计数器（BTM中断更新）
    uint32_t lastOverweightCheck;          // 上次超重检查时间
    uint32_t lastActivityTime;             // 最后操作时间
    uint32_t lastSystemUpdate;             // 上次系统更新时间
    uint32_t loopCounter;                  // 主循环计数器（兼容原有代码）
} SystemTimer_t;

/** @}
  */

/** @defgroup 导出函数声明
  * @{
  */

/* 初始化函数 */
void SystemTimer_Init(void);
void BTM_Timer_Init(void);

/* 时间获取函数 */
uint32_t GetSystemTimeMs(void);
uint32_t GetSystemTime(void);
uint32_t SystemTimer_GetElapsedMs(uint32_t startTime);
uint32_t SystemTimer_GetElapsedLoops(uint32_t startCount);

/* 无操作检测函数 */
void ResetInactivityTimer(void);
uint32_t GetInactivityTime(void);
uint8_t IsInactivityTimeout(void);

/* 定时检查函数 */
uint8_t IsOverweightCheckTime(void);
uint8_t IsSystemUpdateTime(void);
void UpdateOverweightCheckTime(void);
void UpdateSystemUpdateTime(void);

/* 主循环更新 */
void SystemTimer_Update(void);

/** @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_TIMER_H */

