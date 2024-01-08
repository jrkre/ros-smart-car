#ifndef _SERIALDRIVER_H_
#define _SERIALDRIVER_H_

#include "SerialPacket.h"
#include "SerialProtocol.h"
#include "Arduino.h"

class SerialDriver : public SerialProtocol
{

public:
    SerialDriver();
    ~SerialDriver();

    void SerialDriver_Init(void);

    state_codes run_cmd(void);

    void UpdateSensors();

    int advance_state() override;

    std::string GetJson();
    
protected:
    

private:

    /* sensor data */
    uint16_t accel_x;
    uint16_t accel_y;

    uint16_t gyro_x;
    uint16_t gyro_y;

    union float_ultrasonic_distance
    {
        float distance;
        unsigned char distance_bytes[4];
    } ultrasonic_distance_m;
    
    uint8_t ultrasonic_distance_cm;

    bool line_sensor_left;
    bool line_sensor_right;
    bool line_sensor_middle;

    bool left_ground;

    float current_voltage;

    bool robot_enable;

    uint8_t servo_angle;

    bool ap_created;
    bool ap_connected;
    bool ap_fail;
    
};

#endif // _DEVICEDRIVERSET_H_