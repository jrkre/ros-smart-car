#include "SerialDriver.h"

SerialDriver::SerialDriver(/* args */) : SerialProtocol()
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
    Serial.begin(115200);
    Serial.setTimeout(100);
}

void SerialDriver::UpdateSensors()
{
    unsigned char data[2] = {ULTRASONIC, 0};
    
    this->SendReceive(data);

    

    
    this->ultrasonic_distance_cm = this->receivedPacket->GetArg()[0];

    //just ultrasonic for now
}

int SerialDriver::advance_state()
{
    return SerialProtocol::advance_state();
}

// a function that returns a string of valid json from the recievedPacket data
std::string SerialDriver::GetJson()
{
    std::string json = "{";

    json += "\"accel_x\":";
    json += std::to_string(this->accel_x);
    json += ",";

    json += "\"accel_y\":";
    json += std::to_string(this->accel_y);
    json += ",";

    json += "\"gyro_x\":";
    json += std::to_string(this->gyro_x);
    json += ",";

    json += "\"gyro_y\":";
    json += std::to_string(this->gyro_y);
    json += ",";

    json += "\"ultrasonic_distance_cm\":";
    json += std::to_string(this->ultrasonic_distance_cm);
    json += ",";

    json += "\"line_sensor_left\":";
    json += std::to_string(this->line_sensor_left);
    json += ",";

    json += "\"line_sensor_right\":";
    json += std::to_string(this->line_sensor_right);
    json += ",";

    json += "\"line_sensor_middle\":";
    json += std::to_string(this->line_sensor_middle);
    json += ",";

    json += "\"left_ground\":";
    json += std::to_string(this->left_ground);
    json += ",";

    json += "\"current_voltage\":";
    json += std::to_string(this->current_voltage);
    json += ",";

    json += "\"robot_enable\":";
    json += std::to_string(this->robot_enable);
    json += ",";

    json += "\"servo_angle\":";
    json += std::to_string(this->servo_angle);
    json += ",";

    json += "\"ap_created\":";
    json += std::to_string(this->ap_created);
    json += ",";

    json += "\"ap_connected\":";
    json += std::to_string(this->ap_connected);
    json += ",";

    json += "\"ap_fail\":";
    json += std::to_string(this->ap_fail);

    json += "}";

    return json;
}

state_codes SerialDriver::run_cmd()
{
    switch (this->receivedPacket->GetCommand())
    {
        case ULTRASONIC:
            this->ultrasonic_distance_m.distance_bytes[0] = this->receivedPacket->GetArg()[0];
            this->ultrasonic_distance_m.distance_bytes[1] = this->receivedPacket->GetArg()[1];
            this->ultrasonic_distance_m.distance_bytes[2] = this->receivedPacket->GetArg()[2];
            this->ultrasonic_distance_m.distance_bytes[3] = this->receivedPacket->GetArg()[3];
            this->ultrasonic_distance_cm = this->ultrasonic_distance_m.distance * 100;
            break;
        case LINE_SENSOR:
            this->line_sensor_left = this->receivedPacket->GetArg()[0];
            this->line_sensor_right = this->receivedPacket->GetArg()[1];
            this->line_sensor_middle = this->receivedPacket->GetArg()[2];
            break;
        case LEFT_GROUND:
            this->left_ground = this->receivedPacket->GetArg()[0];
            break;
        case VOLTAGE:
            this->current_voltage = this->receivedPacket->GetArg()[0];
            break;
        case ENABLE:
            this->robot_enable = true;
            break;
        case SERVO:
            this->servo_angle = this->receivedPacket->GetArg()[0];
            break;
        case AP_CREATED:
            this->ap_created = this->receivedPacket->GetArg()[0];
            break;
        case AP_CONNECTED:
            this->ap_connected = this->receivedPacket->GetArg()[0];
            break;
        case AP_FAILED:
            this->ap_fail = this->receivedPacket->GetArg()[0];
            break;
        case ACCEL:
            // need case for negative numbers - might need to loop for multiple packets
            this->accel_x = this->receivedPacket->GetArg()[0];
            this->accel_y = this->receivedPacket->GetArg()[1];
            this->gyro_x = this->receivedPacket->GetArg()[2];
            this->gyro_y = this->receivedPacket->GetArg()[3];

            break;
        
        default:
            break;
    }
    return wait;
}