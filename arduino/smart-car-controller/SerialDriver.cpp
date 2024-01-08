#include "SerialDriver.h"
#include "SerialPacket.h"


SerialDriver::SerialDriver() : SerialProtocol()
{
    this->serialPacket = new SerialPacket();
    this->receivedPacket = new SerialPacket();
    this->SerialDriver_Init();
    
    this->robot_enable = false;
    this->servo_angle = 90;
    this->current_voltage = 0;
    this->ap_created = false;
    this->ap_connected = false;
    this->ap_fail = false;
    this->left_ground = false;

    this->accel_x = 0;
    this->accel_y = 0;
    this->gyro_x = 0;
    this->gyro_y = 0;
}

SerialDriver::~SerialDriver()
{
}

void SerialDriver::SerialDriver_Init(void)
{
    RBG_LED.DeviceDriverSet_RBGLED_Init(255);
    Servo.DeviceDriverSet_Servo_Init(90);
    Ultrasonic.DeviceDriverSet_ULTRASONIC_Init();
    Voltage.DeviceDriverSet_Voltage_Init();
    LineSensor.DeviceDriverSet_ITR20001_Init();
}

enum state_codes SerialDriver::run_cmd(void)
{
    enum state_codes ret = wait;
    switch (this->serialPacket->GetCommand())
    {
        case ENABLE:
            this->robot_enable = true;
            ret = wait;
            break;
        case MOVE:
            // move forward, still need to figure out the params on this (might send a vector and do the best it can)
        case SERVO:
            {
                this->servo_angle = this->serialPacket->GetArg()[0];
                this->Servo.DeviceDriverSet_Servo_control(this->servo_angle);

                unsigned char data_to_send[2] = {SERVO, this->servo_angle};

                serialPacket->Build(data_to_send, sizeof(data_to_send));
                ret = send_data;
            }
            break;
        case ULTRASONIC:
            {
                // get ultrasonic distance
                unsigned char data_to_send[1 + sizeof(ultrasonic_distance_m.distance_bytes)] = {ULTRASONIC, ultrasonic_distance_m.distance_bytes[0], ultrasonic_distance_m.distance_bytes[1], ultrasonic_distance_m.distance_bytes[2], ultrasonic_distance_m.distance_bytes[3]};
                this->SendReceive(data_to_send);
            }
            break;
        case JOY_INFO:
            // drive joystick. i dont think this needs a response
            break;
        case STOP:
            this->robot_enable = false;
            break;
        case AP_CREATED:
            // handle other cases too ig
            this->ap_created = true;
            break;
        case AP_CONNECTED:
            this->ap_connected = true;
            break;
        case AP_FAILED:
            this->ap_fail = true;
            break;
        case ACK:
            // do nothing, shouldn't happen
            break;
        case NACK:
            // do nothing, shouldn't happen
            break;
        case KEEPALIVE:
            // do nothing, shouldn't happen
            break;
        case LINE_SENSOR:
            {
                unsigned char data_to_send[4] = {LINE_SENSOR, this->line_sensor_left, this->line_sensor_middle, this->line_sensor_right};
                this->SendReceive(data_to_send);
            }
            break;
        case LEFT_GROUND:
            {
                unsigned char data_to_send[2] = {LEFT_GROUND, this->left_ground};
                this->SendReceive(data_to_send);
            }
            break;
        case VOLTAGE:
            {
                unsigned char data_to_send[5] = {VOLTAGE, this->current_voltage};
                this->SendReceive(data_to_send);
            }
            break;
        case ACCEL:
            {
                unsigned char data_to_send[9] = {ACCEL, this->accel_x, this->accel_y, this->gyro_x, this->gyro_y};
                this->SendReceive(data_to_send);
            }
            break;
        
        
        default:
            sendAck();
            break;
    }
    return ret;
}

void SerialDriver::UpdateSensors()
{
    // this doesnt work but it does in concept
    // this->accel_x = MPU6050_getdata::x();
    // this->accel_y = MPU6050_getdata::GetAccelY();
    // this->gyro_x = MPU6050_getdata::GetGyroX();
    // this->gyro_y = MPU6050_getdata::GetGyroY();

    Ultrasonic.DeviceDriverSet_ULTRASONIC_Get(ultrasonic_distance_cm);
    this->ultrasonic_distance_m.distance = ultrasonic_distance_cm / 100;

    this->line_sensor_left = LineSensor.DeviceDriverSet_ITR20001_getAnaloguexxx_L();
    this->line_sensor_right = LineSensor.DeviceDriverSet_ITR20001_getAnaloguexxx_R();
    this->line_sensor_middle = LineSensor.DeviceDriverSet_ITR20001_getAnaloguexxx_M();

    if (line_sensor_left > 950 && line_sensor_right > 950 && line_sensor_middle > 950)
    {
        this->left_ground = false;
    }
    else
    {
        this->left_ground = true;
    }




    this->current_voltage = Voltage.DeviceDriverSet_Voltage_getAnalogue();
}


int SerialDriver::advance_state()
{
    return SerialProtocol::advance_state();
}