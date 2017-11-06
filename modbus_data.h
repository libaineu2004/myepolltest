#ifndef MODBUS_DATA_H
#define MODBUS_DATA_H

#include <stdint.h>

class CModbus_Data
{
public:
    CModbus_Data();
    ~CModbus_Data(){;}

public:
    static void init(void);

    static uint8_t* getcmd(int index)
    {
        return s_cmds[index];
    }

    static int getcount(void)
    {
        return s_iCount;
    }

protected:
    static uint16_t crc_cal_value(unsigned char*data_value, int data_length);

private:
    static uint8_t s_cmds[][8];
    static int s_iCount;
};

#endif // MODBUS_DATA_H
