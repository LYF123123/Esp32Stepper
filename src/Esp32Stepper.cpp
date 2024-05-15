#include "Esp32Stepper.h"

Esp32Stepper::Esp32Stepper(uint8_t pin_enable,
                           uint8_t pin_direction,
                           uint8_t pin_step,
                           uint32_t pwm_freq,
                           uint8_t value_enable = LOW,
                           uint8_t value_direction = LOW,
                           float duty_cycle = 50.0f,
                           mcpwm_unit_t unit = MCPWM_UNIT_0,
                           mcpwm_timer_t timer = MCPWM_TIMER_0,
                           mcpwm_operator_t operatorIndex=MCPWM_OPR_A)
{
    this->_pin_enable = pin_enable;
    this->_pin_direction = pin_direction;
    this->_pin_step = pin_step;
    this->_pwm_freq = pwm_freq;
    this->_value_enable = value_enable;
    this->_value_direction = value_direction;
    this->_duty_cycle = duty_cycle;
    this->_unit = unit;
    this->_timer = timer;
    this->_operatorIndex = operatorIndex;

    pinMode(this->_pin_enable, OUTPUT);
    pinMode(this->_pin_direction, OUTPUT);
    pinMode(this->_pin_step, OUTPUT);
    digitalWrite(this->_pin_enable, this->_value_enable);
    digitalWrite(this->_pin_direction, this->_value_direction);

    // 选择正确的 MCPWM 信号
    mcpwm_io_signals_t mcpwm_signal = this->getMcpwmSignal();

    // Init MCPWM
    mcpwm_gpio_init(this->_unit, mcpwm_signal, this->_pin_step);
    // 配置 MCPWM
    mcpwm_config_t pwm_config;
    pwm_config.frequency = this->_pwm_freq;                                       // 设置频率
    pwm_config.cmpr_a = (operatorIndex == MCPWM_OPR_A) ? this->_duty_cycle : 0.0; // 初始占空比
    pwm_config.cmpr_b = (operatorIndex == MCPWM_OPR_B) ? this->_duty_cycle : 0.0; // 初始占空比
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(unit, timer, &pwm_config);
}

Esp32Stepper::Esp32Stepper(uint8_t pin_enable,
                           uint8_t pin_direction,
                           uint8_t pin_step,
                           uint32_t pwm_freq) : Esp32Stepper(pin_enable,
                                                             pin_direction,
                                                             pin_step,
                                                             pwm_freq,
                                                             LOW,
                                                             LOW, 50.0f, MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A) {}

Esp32Stepper::Esp32Stepper(uint8_t pin_enable,
                           uint8_t pin_direction,
                           uint8_t pin_step,
                           uint32_t pwm_freq,
                           uint8_t value_enable,
                           uint8_t value_direction) : Esp32Stepper(pin_enable,
                                                                   pin_direction,
                                                                   pin_step,
                                                                   pwm_freq,
                                                                   value_enable,
                                                                   value_direction, 50.0f, MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A) {}

Esp32Stepper::~Esp32Stepper()
{
    // 仅停止当前操作员
        if (this->_operatorIndex == MCPWM_OPR_A) {
            mcpwm_set_duty(this->_unit, this->_timer, MCPWM_OPR_A, 0);
        } else {
            mcpwm_set_duty(this->_unit, this->_timer, MCPWM_OPR_B, 0);
        }
        // 停止 MCPWM 单元定时器
        mcpwm_stop(this->_unit, this->_timer);
        mcpwm_gpio_init(this->_unit, this->getMcpwmSignal(), -1); // 解除 GPIO 绑定

}

// Member Methods

void Esp32Stepper::EnableStepper()
{
    digitalWrite(this->_pin_enable, LOW);
    this->_value_enable = LOW;
}
void Esp32Stepper::DisableStepper()
{
    digitalWrite(this->_pin_enable, HIGH);
    this->_value_enable = HIGH;
}

uint8_t Esp32Stepper::GetStepperEnabledStatus() const
{
    return this->_value_enable;
}

void Esp32Stepper::SetStepperDirection(const uint8_t direction)
{
    digitalWrite(this->_pin_direction, direction);
    this->_value_direction = direction;
}

uint8_t Esp32Stepper::GetStepperDirection() const
{
    return this->_value_direction;
}

void Esp32Stepper::SetStepperSpeed(const uint32_t value)
{
    mcpwm_set_frequency(this->_unit, this->_timer, value);
    this->_pwm_freq = value;
}
uint32_t Esp32Stepper::GetStepperSpeed() const
{
    return this->_pwm_freq;
}

void Esp32Stepper::PauseStepper()
{
    this->SetStepperDutyCycle(0.0f);
}

void Esp32Stepper::ResumeStepper()
{
    this->SetStepperDutyCycle(this->_duty_cycle);
}
void Esp32Stepper::SetStepperDutyCycle(const uint32_t duty_cycle)
{
    if (this->_operatorIndex == MCPWM_OPR_A)
    {
        mcpwm_set_duty(this->_unit, this->_timer, MCPWM_OPR_A, duty_cycle);
        mcpwm_set_duty_type(this->_unit, this->_timer, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    }
    else
    {
        mcpwm_set_duty(this->_unit, this->_timer, MCPWM_OPR_B, duty_cycle);
        mcpwm_set_duty_type(this->_unit, this->_timer, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    }
    this->_duty_cycle = duty_cycle;
}
uint32_t Esp32Stepper::GetStepperDutyCycle() const
{
    return this->_duty_cycle;
}
