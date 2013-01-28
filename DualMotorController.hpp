/// NOTE: the code is basically a reimplementation of the Arduino driver 
/// available at pololu website at http://www.pololu.com/catalog/product/1110

#ifndef DUAL_MOTOR_CONTROLLER_HPP
#define DUAL_MOTOR_CONTROLLER_HPP 

#include "UARTDevice.hpp"

class DualMotorController
{
public:
    enum QikConfig 
    {
        QIK_CONFIG_DEVICE_ID                        = 0,
        QIK_CONFIG_PWM_PARAMETER                    = 1,
        QIK_CONFIG_SHUT_DOWN_MOTORS_ON_ERROR        = 2,
        QIK_CONFIG_SERIAL_TIMEOUT                   = 3,
        QIK_CONFIG_MOTOR_M0_ACCELERATION            = 4,
        QIK_CONFIG_MOTOR_M1_ACCELERATION            = 5,
        QIK_CONFIG_MOTOR_M0_BRAKE_DURATION          = 6,
        QIK_CONFIG_MOTOR_M1_BRAKE_DURATION          = 7,
        QIK_CONFIG_MOTOR_M0_CURRENT_LIMIT_DIV_2     = 8,
        QIK_CONFIG_MOTOR_M1_CURRENT_LIMIT_DIV_2     = 9,
        QIK_CONFIG_MOTOR_M0_CURRENT_LIMIT_RESPONSE  = 10,
        QIK_CONFIG_MOTOR_M1_CURRENT_LIMIT_RESPONSE  = 11
    };

    enum QikCoast 
    {
        QIK_2S9V1_MOTOR_M0_COAST         = 0x86,
        QIK_2S9V1_MOTOR_M1_COAST         = 0x87
    };

    enum QikMotors 
    {
        QIK_MOTOR_M0_FORWARD             = 0x88,
        QIK_MOTOR_M0_FORWARD_8_BIT       = 0x89,
        QIK_MOTOR_M0_REVERSE             = 0x8A,
        QIK_MOTOR_M0_REVERSE_8_BIT       = 0x8B,
        QIK_MOTOR_M1_FORWARD             = 0x8C,
        QIK_MOTOR_M1_FORWARD_8_BIT       = 0x8D,
        QIK_MOTOR_M1_REVERSE             = 0x8E,
        QIK_MOTOR_M1_REVERSE_8_BIT       = 0x8F
    };
    
    enum QikCommands 
    {
        QIK_GET_FIRMWARE_VERSION         = 0x81,
        QIK_GET_ERROR_BYTE               = 0x82,
        QIK_GET_CONFIGURATION_PARAMETER  = 0x83,
        QIK_SET_CONFIGURATION_PARAMETER  = 0x84
    };

public:
    DualMotorController(UARTDevice& device);

    uint8_t getFirmwareVersion();
    uint8_t getErrors();

    uint8_t getConfigurationParameter(uint8_t parameter);
    uint8_t setConfigurationParameter(uint8_t parameter, uint8_t value);

    void setM0Speed(int speed);
    void setM1Speed(int speed);
    void setSpeeds(int m0Speed, int m1Speed);

    void setM0Coast();
    void setM1Coast();
    void setCoasts();

protected:
    UARTDevice& mDevice;
};

#endif // DUAL_MOTOR_CONTROLLER_HPP
