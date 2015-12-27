#include "mbed.h"

Serial pc(USBTX, USBRX);
Serial uart(P0_0, P0_1);
Serial motor(P2_0, P2_1);

const int BUFFERSIZE = 23;
const int ORDERSIZE  = 14;
uint16_t rxBuffer[BUFFERSIZE];

volatile int rxIn = 0;
volatile int rxOut = 0;

int16_t angle[3];
float   angleDegree[3];
uint8_t orderBuffer[ORDERSIZE] = {0};
uint8_t orderTemp;
int angleInt;

unsigned char BufC;
int16_t BufW;

/*中断服务函数*/
void rxInterrupt();

/*解码*/
void deCode();


void enCode();
int main(void)
{
    uart.baud(115200);
    pc.baud(115200);
    motor.baud(115200);
    uart.attach(&rxInterrupt);
//    pc.putc('s');
    while (1)
    {
//        motor.printf("%f\n", angleDegree[1]);
//        pc.printf("%f\n", angleDegree[1]);
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
    enCode();
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

void enCode()
{
    orderBuffer[0] = 0x00;
    if (angleDegree[1] < 0)
    {
        orderBuffer[2] = '-';
        angleDegree[1] = angleDegree[1] * (-1);
    }
    else
    {
        orderBuffer[2] = '+';
    }
    
    angleInt = (int)angleDegree[1];
    orderTemp = angleInt % 10;
    orderBuffer[3] = '0' + orderTemp;

    angleInt = angleInt / 10;
    orderTemp = angleInt % 10;
    orderBuffer[4] = '0' + orderTemp;

    angleInt = angleInt / 10;
    orderTemp = angleInt % 10;
    orderBuffer[5] = '0' + orderTemp;

    for (int i = 0; i < ORDERSIZE; ++i)
    {
        motor.putc(orderBuffer[i]);
    }

    return;
}