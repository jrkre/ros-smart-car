#include "SerialPacket.h"
#include <memory>
#include <new>
#include <fcntl.h>

SerialPacket::SerialPacket()
{
    // Initialize the serial port
    init();
}

SerialPacket::SerialPacket(std::string port)
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

    if (Serial2)
    {
        Serial.println("SerialPacket::init():open():");
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
        Serial.println("Packet to arduino can't be larger than 200 bytes");
        return 1;
    }

    packet[2] = datasize + 2;

    memcpy(&packet[3], data, datasize);

    chksum = CalcCheckSum();
    packet[3 + datasize] = chksum >> 8;
    packet[3 + datasize + 1] = chksum & 0xFF;

    if (!Check())
    {
        Serial.println("DAMN");
        return 1;
    }
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
    // Read from the serial port
    // ...


    unsigned long timeout = millis();

    unsigned char prefix[3];
    int cnt = 0;

    ::memset(packet, 0, sizeof(packet)); // clear the packet

    // while (Serial2.available() < 1)
    // {
    //     Serial.println("!SerialPacket::Receive():Serial2: waiting...");
    //     delay(50);
    // }

    if (Serial2.available() > 0)
    {
        Serial.println("SerialPacket::Receive():Serial2: available");
        do
        {
            ::memset(prefix, 0, sizeof(prefix)); // clear the prefix

            while (1)
            {
                cnt = 0;
                while (cnt != 1 && Serial2.available() > 0)
                {

                    if ((cnt += Serial2.readBytes(&prefix[2], 1)) < 1)
                    {
                        Serial.println("Error reading packet.hppeader from robot connection: Serial():Receive():read():");
                        return 1;
                    }

                    //i think this line is breaking things so its going out for now..
                    // if (prefix[2] == 0)
                    // {
                    //     cnt = 0;
                    // }
                    Serial.print("Recieve():prefix[2]:");
                    Serial.println(prefix[2]);
                }

                Serial.print("Check for header...");
                Serial.print(prefix[0]);
                Serial.println(prefix[1]);

                if (prefix[0] == 0xFA && prefix[1] == 0xFB)
                {
                    Serial.println("Header found!");
                    break;
                }

                if (millis() - timeout > 500)
                {
                    return 1;
                }

                // if(cnt == 0)
                // {
                //   Serial.println("SerialPacket:Receive(): No bytes to read");
                //   return 1;
                // }
                // Serial.println("Header not found... shifting bits");

                prefix[0] = prefix[1]; // shift bits into header position - if a header is missed it will ignore the packet
                prefix[1] = prefix[2];
            }

            size = prefix[2] + 3;
            memcpy(packet, prefix, 3); // copy prefix to header of packet

            Serial.println("reading body...");

            cnt = 0;
            while (cnt != prefix[2] && Serial2.available())
            {
                // this fucking line is going to break everything im sure
                if ((cnt += Serial2.readBytes(&packet[3 + cnt], prefix[2])) < 1)
                {
                    Serial.println("Error reading packet body from robot connection: P2OSPacket():Receive():read():");
                    return 1;
                }
            }

            // printPacket();

        } while (!Check() && Serial2.available());


        Serial.println("SerialPacket->PACKET RECEIVED!");
        printPacket();
        return 0;
    }
}

int SerialPacket::Send()
{
    // Write to the serial port
    // ...

    Serial.println("SerialPacket->Sending packet...");

    int cnt = 0;

    if (Serial2)
    {
        while (cnt != size)
        {
            Serial.print("Sending ");
            Serial.print(size);
            Serial.println(" bytes");
            //if ((cnt += Serial2.write(packet, packet[2])) < 0) -> this should work because it sends the defined length of the array, 
            //but im not sure at what speed it will send or if it will match up with baud
            // one thing is certain -> it will send what is neccessary and not the full 255 bytes every time hehe
            if ((cnt += Serial2.write(packet, size)) < 0) //TODO : determine if this will work
            {
                Serial.println("SerialPacket->Send errored out");
                return 1;
            }
        }
        return 0;
    }
    return 1;
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

