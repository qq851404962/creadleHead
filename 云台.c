#include "mbed.h"

Serial pc(USBTX, USBRX);
Serial uart(P0_0, P0_1);
Serial motor(P4_28, P4_29);
//Serial motor2(P4_28, P4_29);
const uint16_t LENGTH = 15;
unsigned char order[LENGTH] = {0,'1','1','1','1','1','1','1','1','1','1','1','1','1','1'};
bool refreshFlag = false;

int16_t Gyro[3], Acc[3], Angle[3], Mag[3];
int32_t Altitude, Pressure;
float Temper, GyroDPS[3], AccG[3], MagGauss[3], AngleDeg[3];

unsigned char BufC;
uint16_t BufW; 
int16_t temp = 0;
int16_t speed[3] = {0};

float P[3] = {0.001, 0.001, 0.001};
float I[3] = {0};
float D[3] = {0};

/*读两个字节组成一个16位数*/
int16_t ReadW(void)
{
	unsigned char BufC;
    int16_t BufW;
    
  	BufC = uart.getc();
  	BufW = (uint16_t)BufC;
	BufC = uart.getc();
	BufW = (int16_t)(((uint16_t)BufC  << 8) | BufW);
	return BufW;
}

void getAngle(void)
{
	if(uart.readable()) //检测是否收到LEADIY-M3数据
	{  
		BufC = uart.getc(); //读一个字节
		if((BufC==0xA7))
		{  //判断是否为帧头
			BufC = uart.getc(); //读一个字节
			if (BufC==0x7A) 
			{ //判断是否为帧头
				BufC = uart.getc(); //读一个字节
	      		switch(BufC) //帧类型判断
	            {
	                case 0x60: //标识为角速度，姿态帧

	                	//跳过一开始那些帧，共十四个
	                	for(int k = 0; k < 14; k++)
	                	{
	                		BufC = uart.getc();
	                	}//帧长度，可不用

	                    Angle[0] = ReadW();   //X轴角度(横滚)
	                    Angle[1] = ReadW();   //Y轴角度(俯仰)
	                    Angle[2] = ReadW();   //Z轴角度(偏航)
	    
	                    AngleDeg[0] = (float)Angle[0] / 100;
	                    AngleDeg[1] = (float)Angle[1] / 100;
	                    AngleDeg[2] = (float)Angle[2] / 100;

	                    if (AngleDeg[2]<0) 
	                    {
	                        AngleDeg[2] += 360; //将航向值转化到0---360度区间
	                    }
	                    refreshFlag = true;
	                    break;

	                default:
	                    break;
	            }
	    	}
		} 
	}
}

//把指令全部都发出去
void sendOrder(void)
{
	for (int i = 0; i < LENGTH; ++i)
	{
		motor.putc(order[i]);
	}
}

//电机序号为0~3；
void setOrder(float speed, uint16_t motor)
{
	char check = 0;
	if (refreshFlag)
	{
		int speedInt = (int)(speed);
		int speedTemp = 0;
		if (speed < 0)
		{
			order[motor*3 + 2] = '-';
		}
		else
		{
			order[motor*3 + 2] = '+';	
		}

		//个位
		speedTemp = speedInt % 10;
		speedInt = speedInt / 10;
		order[motor*3 + 3] = '0' + speedTemp;

		//十位
		speedTemp = speedInt % 10;
		speedInt = speedInt / 10;
		order[motor*3 + 4] = '0' + speedTemp;

		//百位
		speedTemp = speedInt % 10;
		order[motor*3 + 5] = '0' + speedTemp;	
		refreshFlag = false;

		for (int i = 2; i < LENGTH; ++i)
		{
			check ^= order[i];
		}
		order[1] = check;
	}
}

float sb;

int main(void)
{
    uart.baud(115200);
    pc.baud(115200);
    motor.baud(115200);

    while (1)
    {
    	getAngle();

		char check = 0;
		if (refreshFlag)
		{
			int speedInt = (int)(speed);
			int speedTemp = 0;
			if (speed < 0)
			{
				order[motor*3 + 2] = '-';
			}
			else
			{
				order[motor*3 + 2] = '+';	
			}

			//个位
			speedTemp = speedInt % 10;
			speedInt = speedInt / 10;
			order[motor*3 + 3] = '0' + speedTemp;

			//十位
			speedTemp = speedInt % 10;
			speedInt = speedInt / 10;
			order[motor*3 + 4] = '0' + speedTemp;

			//百位
			speedTemp = speedInt % 10;
			order[motor*3 + 5] = '0' + speedTemp;	

			refreshFlag = false;

			for (int i = 2; i < LENGTH; ++i)
			{
				check ^= order[i];
			}
			order[1] = check;

			for (int i = 0; i < LENGTH; ++i)
			{
				motor.putc(order[i]);
			}
		}





    	// setOrder((AngleDeg[1]) , 0);
    	// sb = AngleDeg[1] * 10;
    	// motor.printf("%f\n", AngleDeg[1]);
    	// motor.printf("%f\n", setOrder(AngleDeg[1], 0)); 
    	// motor.printf("%f\n", sb);

    	// sendOrder();
    	wait(0.01);
    	// wait(0.1);

    	// if (uart.readable())
    	// {
    	// 	motor.putc(uart.getc());
    	// }
    	// getAngle();
    	// motor.printf("%f\n", AngleDeg[1]);
    	
    }
    return 0;
}
