#include "mbed.h"

Serial pc(USBTX, USBRX);
Serial uart(P0_0, P0_1);
//Serial motor(P4_28, P4_29);

const int BUFFERSIZE = 23;
uint16_t rxBuffer[BUFFERSIZE];

volatile int rxIn = 0;
volatile int rxOut = 0;

int16_t angle[3];
float   angleDegree[3];

unsigned char BufC;
int16_t BufW;

/*中断服务函数*/
void rxInterrupt();

/*解码*/
void deCode();

int main(void)
{
    uart.baud(115200);
    pc.baud(115200);
//    motor.baud(115200);
    uart.attach(&rxInterrupt);
//    pc.putc('s');
    while (1)
    {
        pc.printf("%f\n", angleDegree[1]);
        wait(0.1);
    }
    return 0;
}

void rxInterrupt()
{
//    pc.putc('r');
    while(uart.readable() || (((rxIn + 1) % BUFFERSIZE) == rxOut))
    {
        rxBuffer[rxIn] = uart.getc();
        rxIn = (rxIn + 1) % BUFFERSIZE;
//        pc.putc('r');
    }
    deCode();
//    return;
}

void deCode()
{
    if (rxBuffer[0] == 0xA7 && rxBuffer[1] == 0x7A && rxBuffer[2] == 0x60)
    {
        BufW = (uint16_t)rxBuffer[17];
        BufC = rxBuffer[18];
        angle[0] = (int16_t)(((uint16_t)BufC  << 8) | BufW);
        angleDegree[0] = (float)angle[0] / 100;

        BufW = (uint16_t)rxBuffer[19];
        BufC = rxBuffer[20];
        angle[1] = (int16_t)(((uint16_t)BufC  << 8) | BufW);
        angleDegree[1] = (float)angle[1] / 100;
        
        BufW = (uint16_t)rxBuffer[21];
        BufC = rxBuffer[22];
        angle[2] = (int16_t)(((uint16_t)BufC  << 8) | BufW);
        angleDegree[2] = (float)angle[2] / 100;
    }

    return;
}