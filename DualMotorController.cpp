#include "DualMotorController.hpp"


DualMotorController::DualMotorController(SerialDevice& device) 
 : mDevice(device)
{
}

uint8_t 
DualMotorController::getFirmwareVersion()
{
    uint8_t firmwareVersion;
    uint8_t data = QIK_GET_FIRMWARE_VERSION;
    mDevice.write(&data, 1);
    mDevice.read(&firmwareVersion, 1);
    return firmwareVersion;
}

uint8_t 
DualMotorController::getErrors()
{
    uint8_t error;
    uint8_t data = QIK_GET_ERROR_BYTE;
    mDevice.write(&data, 1);
    mDevice.read(&error, 1);
    return error;
}

uint8_t 
DualMotorController::getConfigurationParameter(uint8_t parameter)
{
    uint8_t param;
    uint8_t cmd[2];
    cmd[0] = QIK_GET_CONFIGURATION_PARAMETER;
    cmd[1] = parameter;
    mDevice.write(cmd, 2);
    mDevice.read(&param, 1);
    return param;
}

uint8_t 
DualMotorController::setConfigurationParameter(uint8_t parameter, uint8_t value)
{
    uint8_t param;
    uint8_t cmd[5];
    cmd[0] = QIK_SET_CONFIGURATION_PARAMETER;
    cmd[1] = parameter;
    cmd[2] = value;
    cmd[3] = 0x55;
    cmd[4] = 0x2A;
    mDevice.write(cmd, 5);
    mDevice.read(&param, 1);
    return param;
}

void 
DualMotorController::setM0Speed(int speed)
{
    bool reverse = 0;
    uint8_t cmd[2];

    if (speed < 0) {
        speed = -speed; // make speed a positive quantity
        reverse = 1; // preserve the direction
    }

    if (speed > 255) {
        speed = 255;
    }

    if (speed > 127) {
        // 8-bit mode: actual speed is (speed + 128)
        cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE_8_BIT : QIK_MOTOR_M0_FORWARD_8_BIT;
        cmd[1] = speed - 128;
    } else {
        cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE : QIK_MOTOR_M0_FORWARD;
        cmd[1] = speed;
    }

    mDevice.write(cmd, 2);
}

void 
DualMotorController::setM1Speed(int speed)
{
    bool reverse = 0;
    uint8_t cmd[2];

    if (speed < 0) {
        speed = -speed; // make speed a positive quantity
        reverse = 1; // preserve the direction
    }

    if (speed > 255) {
        speed = 255;
    }

    if (speed > 127) {
        // 8-bit mode: actual speed is (speed + 128)
        cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE_8_BIT : QIK_MOTOR_M1_FORWARD_8_BIT;
        cmd[1] = speed - 128;
    } else {
        cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE : QIK_MOTOR_M1_FORWARD;
        cmd[1] = speed;
    }

    mDevice.write(cmd, 2);
}

void 
DualMotorController::setSpeeds(int m0Speed, int m1Speed)
{
    setM0Speed(m0Speed);
    setM1Speed(m1Speed);
}

void 
DualMotorController::setM0Coast()
{
    uint8_t data = QIK_2S9V1_MOTOR_M0_COAST;
    mDevice.write(&data, 1);
}

void 
DualMotorController::setM1Coast()
{
    uint8_t data = QIK_2S9V1_MOTOR_M1_COAST;
    mDevice.write(&data, 1);
}

void 
DualMotorController::setCoasts()
{
    setM0Coast();
    setM1Coast();
}

