//key_handler.c

#include "key_handler.h"
#include "scale_manager.h"
#include "weight_sensor.h"
#include "buzzer.h"

/**
  * @brief  按键按下回调函数
  */
void Key_Pressed_Handler(Key_ID keyId)
{
    // 任何按键按下都重置无操作计时
    ResetInactivityTimer();
    
    switch(keyId) {
        case KEY_ID_TARE:
            if(Scale_State.isMeasuring) {
                // 按键检查模式下的额外检查
                if(Scale_State.overweightMode == OVERWEIGHT_KEY_CHECK) {
                    if(OverweightCheck_OnKeyPress()) {
                        Buzzer_Beep(20);  // 短促提示，超重不允许去皮
                        break;
                    }
                }
                WeightSensor_Tare();  // 去皮
                Buzzer_Beep(50);
            } else {
                Buzzer_Beep(20);  // 短促提示，测量未启动
            }
            break;
        case KEY_ID_CALIB:  // 校准
            Buzzer_Beep(100);
            break;
        case KEY_ID_MEASURE:  // 称重启动/停止
            // 按键检查模式下的超重检查
            if(Scale_State.overweightMode == OVERWEIGHT_KEY_CHECK) {
                if(OverweightCheck_OnKeyPress()) {
                    Buzzer_Beep(20);  // 短促提示，超重不允许测量
                    break;
                }
            }
            
            Scale_State.isMeasuring = !Scale_State.isMeasuring;
            if(Scale_State.isMeasuring) {
                Buzzer_Beep(100);  // 长鸣表示开始测量
            } else {
                Buzzer_Beep(30);   // 短鸣表示停止测量
                StopAlarm();       // 停止测量时也停止警报
            }
            break;
        case KEY_ID_POWER:    // 占位，待后续加入新功能
            Buzzer_Beep(80);
            break;
        default:
            break;
    }
}
