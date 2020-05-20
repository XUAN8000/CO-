#include"iap15w4k58s4.h"
#include"oled.h"
#include"delay.h"

sbit LCD_CS =P4^3; //oledƬѡ��

sbit K1 = P2^0;	//ȷ��
sbit K2 = P2^1;	//����
sbit K3 = P2^2;	//�ϵ�
sbit K4 = P2^3;	//�µ�

sbit PB = P0^0;	//�������µ�
sbit fmq = P1^2; //������
sbit LED3 = P5^2;//LED��



unsigned char  scan_flag = 0;//������־λ

unsigned char  dis_one = 1;//��Ļ����
unsigned char  dis_two = 1;//��Ļ�ֺ�

unsigned int co = 0; //��������⵽��һ����̼Ũ��ֵ
unsigned int co_ppm = 1;//��ʾCO��Ũ��ֵ
unsigned int co_time = 1;//CO�ļ��ʱ��
char co_ppm_dis[] = {'0','0','0','\0'};//COŨ�ȵ���ʾ

unsigned char uart_flag = 1;//����1ͨ�ű��λ
unsigned char co_num[10] = {0};//CO�������Ĵ���
unsigned char co_num_n = 0;//CO�������Ĵ����ڵĵڼ�λ��

unsigned char S2FLAG = 1;//����2ͨ�ű��λ
unsigned char s2_re_data[50] = {0};//ͨѶģ��Ĵ���
unsigned char s2_count= 0;//ͨѶģ��Ĵ����ڵĵڼ�λ��

//ͨѶģ���ATָ������
unsigned char code str5[] = "AT+CMGF=1\r\n";
unsigned char code str6[] = "AT+CSCS=\"GSM\"\r\n";
unsigned char code str7[] = "AT+CMGS=\"19809050776\"\r\n";

/*----------------------------
MCU-GPIO����״̬��ʼ��
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
�����ӳ���
----------------------------*/

char scan()//����ɨ��
{
if(K1 == 0)
	{
	DelayMS(200);//��ʱȥ����
	if(K1 == 0)//����ȷ��
		{
		while(K1 == 0);//����
		return 1;//����ֵ
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

void key_an()//��Ļ�µİ�������
{
  if(dis_one == 1)	   //1 ȷ�� 2 ���� 3 �ϵ� 4 �µ�
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
��Ļ��ʾ�ӳ���
----------------------------*/
void oled_init()//��Ļ��ʼ��
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


void dis1()//һ����Ļ��ʾ
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

void str_dis()//��Ļ��ת����
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
ͨ�����ж�0���ͱ���������
----------------------------*/
void int0_init()//�ж�0�ĳ�ʼ��
{
TCON = TCON|0X01;
EX0 = 1;
EA = 1;
}

void int0_inter()interrupt 0//���������ն˷������
{
if(PB == 0)	scan_flag = 3;
if(PB == 1) scan_flag = 4;
}

/*----------------------------
ͨ������1���ʹ���������
----------------------------*/
void UartInit1(void)		//����1�жϵĳ�ʼ��
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	AUXR1 |= 0x7F;      //�л�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xE0;		    //�趨��ʱ��ֵ
	TH1 = 0xFE;		    //�趨��ʱ��ֵ
	ET1 = 0;		    //��ֹ��ʱ��1�ж�
	TR1 = 1;		    //������ʱ��1
	ES = 1;             //�����п�1�ж�                   
	EA = 1;             //���ж�����
}

void uart1_inter()interrupt 4//��������ͨ������
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
		co = (co_num[4]<<8)+co_num[5];//��ȡ����Ũ��ֵ
		if(dis_one == 1)
			{
			dis1();
			}
		}
	}
}

/*----------------------------
ͨ������2����ATָ��
----------------------------*/
void UartInit2(void)		//115200bps@11.0592MHz
{

	S2CON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xE8;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
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

void u2_send_char(unsigned char s2_data)//ͨ������2���ʹ�������
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

void u2_send_str(unsigned char *p)//ͨ������2�����ַ���
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
��������
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
������
----------------------------*/
void main(void)//������
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












