#include <STC15F2K60S2.h>  
#include <onewire.h>

#define uchar unsigned char
#define uint unsigned int 
	
uchar code ndottab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar code dottab[]={0x40,0x79,0x20,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
uint temp=0;

void Init_System();
void DS18B20();
void delayms(uint ms);
void display();
void display_bit(uchar pos,uchar dat);

void main()
{
	Init_System();
	while(1)
	{
		DS18B20();
		display();
	}
}

void Init_System()
{
	P2 = (P2 & 0x1f) | 0x80;
	P0=0xff;
	P2 = (P2 & 0x1f) | 0xa0;
	P0=0x00;
	P2 = (P2 & 0x1f) | 0xc0;
	P0=0xff;
	P2 = (P2 & 0x1f) | 0xe0;
	P0=0xff;
}

void display_bit(uchar pos,uchar dat)
{
	P2 = (P2 & 0x1f) | 0xc0;
	P0=0x01 << pos;
	P2 = (P2 & 0x1f) | 0xe0;
	P0=dat;
}

void display()
{
	display_bit(0,0xff);
	delayms(1);
	display_bit(1,0xff);
	delayms(1);
	display_bit(2,0xff);
	delayms(1);
	display_bit(3,0xff);
	delayms(1);
	display_bit(4,0xff);
	delayms(1);
	display_bit(5,ndottab[temp/100]);
	delayms(1);
	display_bit(6,dottab[temp%100/10]);
	delayms(1);
	display_bit(7,ndottab[temp%10]);
	delayms(1);
	P2 = (P2 & 0x1f) | 0xc0;
	P0=0xff;
	P2 = (P2 & 0x1f) | 0xe0;
	P0=0xff;
}
void delayms(uint ms)
{
	uint i,j;
	for(i=ms;i>0;i--)
		for(j=810;j>0;j--);
}

void DS18B20()
{
	uchar LSB,MSB;
	init_ds18b20();      //DS18B20复位
	Write_DS18B20(0xcc); //跳过ROM操作指令
	Write_DS18B20(0x44); // 开始温度转换
	
	delayms(1);       //等待温度转化完成,1000us
	
	init_ds18b20();     // 复位
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe); // 读取高速寄存器
	
	LSB=Read_DS18B20();  //读取温度数据低八位
	MSB=Read_DS18B20();  //读取温度数据高八位
	init_ds18b20();
	temp=MSB;
	temp=(temp << 8) | LSB;
	if((temp & 0xf800) == 0x0000)  //等号前括号没加给了一个警告, 高五位全为0是正数，全为1是负数
	{
		temp >>= 4;   //如果想要显示整数，只需要这一句代码就行
		temp = temp * 10;  //此处显示是带小数，要将整数*10
		temp=temp + (LSB & 0X0F)*0.625;  //LSB低四位是小数，(LSB & 0X0F)*0.625=(LSB & 0X0F)*0.0625*10
	}
}