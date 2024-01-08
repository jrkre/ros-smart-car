#include "SerialProtocol.h"
#include "DeviceDriverSet.h"
#include "MPU6050_getdata.h"


class SerialDriver : public SerialProtocol
{

public:
    SerialDriver();
    ~SerialDriver();

    void SerialDriver_Init(void);

    enum state_codes run_cmd(void);

    void UpdateSensors();

    int advance_state() override;

    void ApplicationFunctionSet_RGB(void);


    DeviceDriverSet_RBGLED RBG_LED;
    DeviceDriverSet_Servo Servo;
    DeviceDriverSet_ITR20001 LineSensor;
    DeviceDriverSet_ULTRASONIC Ultrasonic;
    DeviceDriverSet_Voltage Voltage;
    

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

    int line_sensor_left;
    int line_sensor_right;
    int line_sensor_middle;

    bool left_ground;

    float current_voltage;

    bool robot_enable;

    uint8_t servo_angle;

    bool ap_created;
    bool ap_connected;
    bool ap_fail;


};