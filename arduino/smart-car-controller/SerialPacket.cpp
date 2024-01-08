#include "SerialPacket.h"
#include <new>
#include <fcntl.h>

SerialPacket::SerialPacket()
{
    // Initialize the serial port
    init();
}

SerialPacket::SerialPacket(String port)
{
    // Initialize the serial port
    portFilePath = port;
    // init(port);
}

SerialPacket::~SerialPacket()
{
    // Close the serial port
}

bool SerialPacket::init()
{
    // Initialize the serial port
    // ...

    if (!Serial)
    {
        // Serial.println("SerialPacket::init():open():");
        return 1;
    }
    return 0;
}

// bool SerialPacket::init(std::string portFilePath)
// {
//     // Initialize the serial port
//     // ...
//     // Open the serial port
//     this->portFilePath = portFilePath;

//     SerialPacket = serial::Serial(portFilePath, BAUD_RATE, serial::Timeout::simpleTimeout(1000));

//     if (SerialPacket.isOpen())
//     {
//         std::string info_message = "Serial Port initialized on " + portFilePath;
//         // Serial.println(info_message);
//         return true;
//     }
//     else
//     {
//         std::string error_message = "Serial Port could not be initialized on " + portFilePath;
//         // Serial.println(error_message);
//         return false;
//     }
// }

void SerialPacket::Print()
{
    if (packet)
    {
        Serial.println("\"");
        for (int i = 0; i < size; i++)
        {
            Serial.println(packet[i]);
        }
        Serial.println("\"");
    }
}

// void SerialPacket::PrintHex()
// {
//   if (packet) {
//     Serial.println("\"");
//     for (int i = 0; i < size; i++) {
//       Serial.println("0x%.2x ", packet[i]);
//     }
//     Serial.println("\"");
//   }
// }

bool SerialPacket::Build(unsigned char *data, unsigned char datasize)
{

    ::memset(packet, 0, sizeof(packet));

    int16_t chksum;

    size = datasize + 5;

    /* header */
    packet[0] = 0xFA;
    packet[1] = 0xFB;

    if (size > 198)
    {
        // Serial.println("Packet to arduino can't be larger than 200 bytes");
        return 1;
    }

    packet[2] = datasize + 2;

    memcpy(&packet[3], data, datasize);

    chksum = CalcCheckSum();
    packet[3 + datasize] = chksum >> 8;
    packet[3 + datasize + 1] = chksum & 0xFF;

    if (!Check())
    {
        // Serial.println("DAMN");
        return 1;
    }

    // Serial.println("packet built");
    // printPacket();

    return 0;
}

int SerialPacket::CalcCheckSum()
{
    unsigned char *buffer = &packet[3];
    int c = 0;
    int n;

    for (n = size - 5; n > 1;)
    {
        c += (*(buffer) << 8) | *(buffer + 1);
        c = c & 0xffff;
        n -= 2;
        buffer += 2;
    }
    if (n > 0)
    {
        c ^= static_cast<int>(*(buffer++));
    }

    return c;
}

int SerialPacket::Receive()
{
    // Read a packet from the serial port
    // ...]

    unsigned long timeout = millis();

    DeviceDriverSet_RBGLED led;
    led.DeviceDriverSet_RBGLED_Init(255);

    unsigned long timestamp = millis();
    unsigned char prefix[3];
    int cnt = 0;

    ::memset(packet, 0, sizeof(packet)); // clear packet mem

    if (Serial.available() > 0) // if serial has bytes to read
    {

        do // while !Check()
        {
            ::memset(prefix, 0, sizeof(prefix)); // clear prefix

            while (1)
            {
                cnt = 0;
                while (cnt != 1 && Serial.available() > 0)
                {
                    if ((cnt += Serial.readBytes(&prefix[2], 1)) < 1)
                    {
                        // Serial.println("Error reading packet.hppeader from robot connection: Serial():Receive():read():");
                        return 1;
                    }
                    //led.flashLED(10, 255, 0, 0);
                    //Serial.print("here:");
                    //Serial.println(prefix[2]);
                }

                if (prefix[0] == 0xFA && prefix[1] == 0xFB)
                {
                    //led.flashLED(10, 0, 0, 255);
                    break;
                }
                // if(cnt == 0) //no bytes read -> return?
                // {
                //     led.flashLED(10, 255, 0, 0);
                //     return 1;
                // }
                if (millis() - timeout > 300)
                {
                    return 1;
                }

                // GlobalTime->GetTimeDouble(&timestamp);
                //Serial.print("here:");
                //Serial.println(prefix[2]);

                prefix[0] = prefix[1]; // shift bits into header position - if a header is missed it will ignore the packet
                prefix[1] = prefix[2];
                // skipped++;
            }
            // if (skipped>3) Serial.println("Skipped %d bytes\n", skipped);

            size = prefix[2] + 3;      // size = packet length + 3 for this packet and the checksum
            memcpy(packet, prefix, 3); // copy prefix to header of packet

            cnt = 0;
            while (cnt != prefix[2] && Serial.available() > 0) // readbody
            {
                //Serial.println("body");
                // readbyte = Serial.read();

                // if (readbyte == -1)
                // {
                //     Serial.println("Error reading packet body from robot connection: Serial():Receive():read():199");
                //     return -1;
                // }
                // cnt += sizeof(readbyte);

                // packet[3 + cnt] = readbyte;
                if ((cnt += Serial.readBytes(&packet[3 + cnt], prefix[2])) < 1)
                {
                    // Serial.println("Error reading packet body from robot connection: P2OSPacket():Receive():read():");
                    return 1;
                }
            }

            // Serial.println("packet recieved");
            // printPacket();

        } while (!Check() && Serial.available() > 0);

        return 0;
    }
    else
    {
        led.flashLED(20, 255, 0, 0);
#if DEBUG > 0
        Serial.println("No serial data!");
#endif
        return 1;
    }
}

int SerialPacket::Send()
{
    // Write to the serial port
    // ...

    // Serial.print("Send packet");

    int cnt = 0;

    // not worth the memory rn
    //  DeviceDriverSet_RBGLED led;
    //  led.DeviceDriverSet_RBGLED_Init(255);

    if (Serial)
    {

        while (cnt != size)
        {
            if ((cnt += Serial.write(packet, size)) < 0)
            {
                // Serial.println("Send");
                return 1;
            }
        }
        return 0;
    }
}

bool SerialPacket::Check()
{
    const int16_t chksum = CalcCheckSum();
    return (chksum == (packet[size - 2] << 8)) | packet[size - 1];
}

void SerialPacket::printPacket()
{
    Serial.println("printPacket():");

    for (int i = 0; i < 3; i++)
    {
        char buf[64];
        sprintf(buf, "packet header[%d]: %d", i, packet[i]);
        Serial.println(buf);
    }
    for (int i = 3; i < size - 2; i++)
    {
        char buf[64];
        sprintf(buf, "packet body[%d]: %d", i, packet[i]);
        Serial.println(buf);
    }
    for (int i = size - 2; i < size; i++)
    {
        char buf[64];
        sprintf(buf, "packet chksum[%d]: %d", i, packet[i]);
        Serial.println(buf);
    }
}

commands SerialPacket::GetCommand()
{
    return static_cast<commands>(packet[3]);
}

unsigned char SerialPacket::GetArgLength()
{
    return packet[2] - 2;
}

unsigned char SerialPacket::GetArgType()
{
    return packet[4];
}

unsigned char *SerialPacket::GetArg()
{
    return &packet[5];
}

