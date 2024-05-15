// This library is used to easily drive class 42 stepper motors on ESP32 series chips using PWM technology and the drivers listed below.

// 1) DRV8825
//===================================================================================
//| 1  | 2  | 3  |                step mode
//-----------------------------------------------------------------------------------
//| 0  | 0  | 0	 | Full step (2-phase excitation) with 71% current  |
//-----------------------------------------------------------------------------------
//| 1  | 0  | 0  |      1/2 step (1-2 phase excitation)             |
//-----------------------------------------------------------------------------------
//| 0  | 1  | 0  |      1/4 step (W1-2 phase excitation)            |
//-----------------------------------------------------------------------------------
//| 1  | 1  | 0  |         8 microsteps/step                        |
//-----------------------------------------------------------------------------------
//| 0  | 0  | 1  |         16 microsteps/step                       |
//-----------------------------------------------------------------------------------
//| 1  | 0  | 1  |         32 microsteps/step                       |
//-----------------------------------------------------------------------------------
//| 0  | 1  | 1  |         32 microsteps/step                       |
//-----------------------------------------------------------------------------------
//| 1  | 1  | 1  |         32 microsteps/step                       |
//===================================================================================
// 2) A4988
//===================================================================================
//| 1  | 2  | 3  |                step mode                         |
//-----------------------------------------------------------------------------------
//| 0  | 0  | 0  |                Full step                         |
//-----------------------------------------------------------------------------------
//| 1  | 0  | 0  |                1/2 step                          |
//-----------------------------------------------------------------------------------
//| 0  | 1  | 0  |                1/4 step                          |
//-----------------------------------------------------------------------------------
//| 1  | 1  | 0  |              8 microsteps/step                   |
//-----------------------------------------------------------------------------------
//| 1  | 1  | 1  |              16 microsteps/step                  |
//===================================================================================
// 3) TMC 2208
//| 1  | 2  | 3  |                step mode                         |
//-----------------------------------------------------------------------------------
//| 1  | 0  | 0  |                  2 step                          |
//-----------------------------------------------------------------------------------
//| 0  | 1  | 0  |                  4 step                          |
//-----------------------------------------------------------------------------------
//| 0  | 0  | 0  |                  8 step                          |
//-----------------------------------------------------------------------------------
//| 1  | 1  | 0  |                  16 step                         |
//===================================================================================
/* PWM Resolution
8 bit size max freq = 312,500 Hz
10 bit size max freq = 78,125 HZ
12 bit size max freq = 19,531 HZ
16 bit size max freq = 1,220 HZ
*/
//===================================================================================

#ifndef __Esp_32_Stepper_H__
#define __Esp_32_Stepper_H__
#include "Arduino.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#define __Esp_32_Stepper_Version "1.0.0"

class Esp32Stepper
{
private:
    // OUTPUT Pins
    uint8_t _pin_enable;
    uint8_t _pin_direction;
    uint8_t _pin_step; // pwm output pin

    // values
    uint8_t _value_enable;    // LOW is enabled
    uint8_t _value_direction; // Direction depend on Driver chip.
    // uint64_t _value_step;     // without Micro stepping, one step rotation 1.8°, one cycle is 200 steps

    // PWM values
    uint32_t _pwm_freq; // freq HZ speed
    float _duty_cycle;
    mcpwm_unit_t _unit;
    mcpwm_timer_t _timer;
    mcpwm_operator_t _operatorIndex;
    mcpwm_io_signals_t getMcpwmSignal()
    {
        if (this->_unit == MCPWM_UNIT_0)
        {
            return (this->_operatorIndex == MCPWM_OPR_A) ? MCPWM0A : MCPWM0B;
        }
        else if (this->_unit == MCPWM_UNIT_1)
        {
            return (this->_operatorIndex == MCPWM_OPR_A) ? MCPWM1A : MCPWM1B;
        }
        else
        {
            return (this->_operatorIndex == MCPWM_OPR_A) ? MCPWM2A : MCPWM2B;
        }
    }

public:
    // Constructors
    Esp32Stepper() = delete;
    Esp32Stepper(uint8_t pin_enable,
                 uint8_t pin_direction,
                 uint8_t pin_step,
                 uint32_t pwm_freq);
    Esp32Stepper(uint8_t pin_enable,
                 uint8_t pin_direction,
                 uint8_t pin_step,
                 uint32_t pwm_freq,
                 uint8_t value_enable,
                 uint8_t value_direction);
    Esp32Stepper(uint8_t pin_enable,
                 uint8_t pin_direction,
                 uint8_t pin_step,
                 uint32_t pwm_freq,
                 uint8_t value_enable,
                 uint8_t value_direction,
                 float duty_cycle,
                 mcpwm_unit_t unit,
                 mcpwm_timer_t timer,
                 mcpwm_operator_t operatorIndex);

    // Disable copy Constructor
    Esp32Stepper(const Esp32Stepper &) = delete;

    // Disable copy Operation
    Esp32Stepper &operator=(const Esp32Stepper &) = delete;

    // Disable move Constructor
    Esp32Stepper(Esp32Stepper &&) = delete;

    // Disable move Operation
    Esp32Stepper &operator=(Esp32Stepper &&) = delete;

    ~Esp32Stepper();

    // member methods
    void EnableStepper();
    void DisableStepper();
    uint8_t GetStepperEnabledStatus() const;

    void SetStepperDirection(const uint8_t direction);
    uint8_t GetStepperDirection() const;

    // void SetStepperStep(const uint64_t value_step);
    // uint64_t GetStepperStep() const;

    void SetStepperSpeed(const uint32_t value);
    uint32_t GetStepperSpeed() const;

    void PauseStepper();
    void ResumeStepper();

    void SetStepperDutyCycle(const uint32_t duty_cycle);
    uint32_t GetStepperDutyCycle() const;

    void RunSteps(uint64_t steps);
};

#endif