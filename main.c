#include"iap15w4k58s4.h"
#include"oled.h"
#include"delay.h"

sbit LCD_CS =P4^3; //oled片选脚

sbit K1 = P2^0;	//确认
sbit K2 = P2^1;	//返回
sbit K3 = P2^2;	//上调
sbit K4 = P2^3;	//下调

sbit PB = P0^0;	//编码器下调
sbit fmq = P1^2; //蜂鸣器
sbit LED3 = P5^2;//LED灯



unsigned char  scan_flag = 0;//按键标志位

unsigned char  dis_one = 1;//屏幕级别
unsigned char  dis_two = 1;//屏幕分号

unsigned int co = 0; //传感器检测到的一氧化碳浓度值
unsigned int co_ppm = 1;//显示CO的浓度值
unsigned int co_time = 1;//CO的检测时间
char co_ppm_dis[] = {'0','0','0','\0'};//CO浓度的显示

unsigned char uart_flag = 1;//串口1通信标记位
unsigned char co_num[10] = {0};//CO传感器寄存器
unsigned char co_num_n = 0;//CO传感器寄存器内的第几位数

unsigned char S2FLAG = 1;//串口2通信标记位
unsigned char s2_re_data[50] = {0};//通讯模块寄存器
unsigned char s2_count= 0;//通讯模块寄存器内的第几位数

//通讯模块的AT指令数组
unsigned char code str5[] = "AT+CMGF=1\r\n";
unsigned char code str6[] = "AT+CSCS=\"GSM\"\r\n";
unsigned char code str7[] = "AT+CMGS=\"19809050776\"\r\n";

/*----------------------------
MCU-GPIO工作状态初始化
----------------------------*/
void mcu_init()
{
	P1M0=0X00;
	P1M1=0X00;
	P2M0=0X00;
	P2M1=0X00;
	P4M0=0X00;
	P4M1=0X00;
	P0M0=0X00;
	P0M1=0X00;
	P3M0=0X00;
	P3M1=0X00;
	P35 = 1;
}



/*----------------------------
按键子程序
----------------------------*/

char scan()//按键扫描
{
if(K1 == 0)
	{
	DelayMS(200);//延时去抖动
	if(K1 == 0)//两遍确定
		{
		while(K1 == 0);//长按
		return 1;//返回值
		}
	}
if(K2 == 0)
	{
	DelayMS(200);
	if(K2 == 0)
		{
		while(K2 == 0);
		return 2;
		}
	}
if(K3 == 0)
	{
	DelayMS(200);
	if(K3 == 0)
		{
		while(K3 == 0);
		return 3;
		}
	}
if(K4 == 0)
	{
	DelayMS(200);
	if(K4 == 0)
		{
		while(K4 == 0);
		return 4;
		}
	}
return 0;
}

void key_an()//屏幕下的按键功能
{
  if(dis_one == 1)	   //1 确认 2 返回 3 上调 4 下调
  	{
	if(scan_flag == 1) 
		{
		 dis_one++; 
		 scan_flag = 0;
		}
	}
  if(dis_one == 2)
  	{
	if(scan_flag == 1) 
		{
		 dis_one = dis_one+dis_two; 
		 scan_flag = 0;
		}
	if(scan_flag == 2) 
		{
		 dis_one--; 
		 scan_flag = 0;
		}
	if(scan_flag == 3) 
		{
		 dis_two--; 
		 if(dis_two<1) dis_two = 2;
		 scan_flag = 0;
		}
	if(scan_flag == 4) 
		{
		 dis_two++; 
		 if(dis_two>2) dis_two = 1;
		 scan_flag = 0;
		}	
	}
  if((dis_one == 3) || (dis_one == 4))
  	{
	if(scan_flag == 1)
		{
		dis_one = 1;
		dis_two = 1;
		scan_flag = 0;
		}
	if(scan_flag == 3)
		{
		if(dis_two == 1)
			{
			co_ppm++;
			if(co_ppm >500) co_ppm = 1;
			scan_flag = 0;
			}
		if(dis_two == 2)
			{
			co_time++;
			if(co_time >600) co_ppm = 1;
			scan_flag = 0;
			}
		}

	if(scan_flag == 4)
		{
		if(dis_two == 1)
			{
			co_ppm--;
			if(co_ppm <1) co_ppm = 100;
			scan_flag = 0;
			}
		if(dis_two == 2)
			{
			co_time--;
			if(co_time <1) co_ppm = 600;
			scan_flag = 0;
			}
		}
	}
}

/*----------------------------
屏幕显示子程序
----------------------------*/
void oled_init()//屏幕初始化
{
  LCD_CS= 0;
  LCD_Init();   
  LCD_Fill(0xff); 
  DelayMS(100); 
  LCD_Fill(0x00);
  DelayMS(100);     
  LCD_CLS();  
	DelayMS(100);  
	DelayUS(100);
}


void dis1()//一号屏幕显示
{
co_ppm_dis[0] = co%1000/100+'0';
co_ppm_dis[1] = co%100/10+'0';
co_ppm_dis[2] = co%10+'0';
LCD_CLS();
LCD_P8x16Str(25,1,"welcome ");
LCD_P8x16Str(25,3,"co_ppm:");
LCD_P8x16Str(81,3,co_ppm_dis);
LCD_P8x16Str_1(96,5,"CONT");
}

void dis2()
{
LCD_CLS();
if(dis_two == 1) LCD_P8x16Str_1(25,1,"co_ppm");
else LCD_P8x16Str(25,1,"co_ppm");
if(dis_two == 2) LCD_P8x16Str_1(25,3,"co_time");
else LCD_P8x16Str(25,3,"co_time");
LCD_P8x16Str_1(1,5,"BACK");
LCD_P8x16Str_1(96,5,"CONT");	
}

void dis3()
{
LCD_CLS();	
LCD_P8x16Str_1(25,2,"co_ppm:");
co_ppm_dis[0] = co_ppm%1000/100+'0';
co_ppm_dis[1] = co_ppm%100/10+'0';
co_ppm_dis[2] = co_ppm%10+'0';
LCD_P8x16Str_1(81,2,co_ppm_dis);
LCD_P8x16Str_1(88,5,"ENTER");	
}

void dis4()
{
LCD_CLS();	
LCD_P8x16Str_1(25,2,"co_time:");
co_ppm_dis[0] = co_time%1000/100+'0';
co_ppm_dis[1] = co_time%100/10+'0';
co_ppm_dis[2] = co_ppm%10+'0';
LCD_P8x16Str_1(89,2,co_ppm_dis);
LCD_P8x16Str_1(88,5,"ENTER");	
}

void str_dis()//屏幕跳转函数
{
if(dis_one == 1) dis1();
if(dis_one == 2) dis2();
if(dis_one == 3) 
	{
	if(dis_two == 1) dis3();
	if(dis_two == 2) dis4();
	}
}


/*----------------------------
通过打开中断0发送编码器数据
----------------------------*/
void int0_init()//中断0的初始化
{
TCON = TCON|0X01;
EX0 = 1;
EA = 1;
}

void int0_inter()interrupt 0//编码器的终端服务程序
{
if(PB == 0)	scan_flag = 3;
if(PB == 1) scan_flag = 4;
}

/*----------------------------
通过串口1发送传感器数据
----------------------------*/
void UartInit1(void)		//串口1中断的初始化
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	AUXR1 |= 0x7F;      //切换串口
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xE0;		    //设定定时初值
	TH1 = 0xFE;		    //设定定时初值
	ET1 = 0;		    //禁止定时器1中断
	TR1 = 1;		    //启动定时器1
	ES = 1;             //允许串行口1中断                   
	EA = 1;             //总中断允许
}

void uart1_inter()interrupt 4//传感器的通信数据
{
if(TI)
	{
	TI = 0;
	uart_flag = 1;
	}
if(RI)
	{
	RI = 0;
	co_num[co_num_n] = SBUF;
	co_num_n++;
	if(co_num_n == 9)
		{
		co_num_n = 0;
		co = (co_num[4]<<8)+co_num[5];//获取气体浓度值
		if(dis_one == 1)
			{
			dis1();
			}
		}
	}
}

/*----------------------------
通过串口2发送AT指令
----------------------------*/
void UartInit2(void)		//115200bps@11.0592MHz
{

	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE8;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	IE2|=0X01;
	EA = 1;
}

void uart2_inter()interrupt 8
{
if(S2CON&0X01)
	{
	S2CON&=0XFE;
	s2_re_data[s2_count] = S2BUF;
	s2_count++;
	if(s2_count == 50) s2_count = 0;
	}
	if(S2CON&0X02)
	{
	S2CON&=0XFD;
	S2FLAG = 1;
	}
}

void u2_send_char(unsigned char s2_data)//通过串口2发送串口数据
{
while(S2FLAG == 0);
S2BUF = s2_data;
S2FLAG = 0;
}

void u2_clr()
{
unsigned char i;
for(i=0;i<50;i++)
	s2_re_data[i] = 0;
s2_count = 0;	
}

void u2_send_str(unsigned char *p)//通过串口2发送字符串
{
while(*p) u2_send_char(*p++);
}

void send_dt()
{
u2_send_str(str5);
DelayMS(100);
u2_send_str(str6);
DelayMS(100);
u2_send_str(str7);
DelayMS(100);
}	

/*----------------------------
报警函数
----------------------------*/
void Baojing()
{
		
    fmq = 1;	
    LED3 = 1;	
	DelayMS(100);
	fmq = 0;
	LED3 = 0;
	DelayMS(1000);
}

/*----------------------------
主函数
----------------------------*/
void main(void)//主函数
{
mcu_init();
oled_init();
UartInit2();
int0_init();
UartInit1();
DelayMS(100);
dis1();	
  while(1)
 { 
  scan_flag = scan();
  if(scan_flag != 0)
  	{
	    key_an();
  	    str_dis();
    }
   if(co > co_ppm )
	{
		DelayMS(100);
        Baojing();
		DelayMS(100);
		send_dt();
	    DelayMS(50);
		u2_send_str("SOS");
		u2_send_char(0X1A);	
		DelayMS(2000);	
    }	
 } 
}












