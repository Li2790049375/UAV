#include "NRF24L01.h"

#undef SUCCESS
#define SUCCESS 0
#undef FAILED
#define FAILED  1

//地址5
const uint8_t TX_ADDRESS[]= {0xE1,0xE2,0xE3,0xE4,0xE5};	//本地地址
const uint8_t RX_ADDRESS[]= {0xE1,0xE2,0xE3,0xE4,0xE5};	//接收地址RX_ADDR_P0 == RX_ADDR

//上电检测NRF24L01是否在位
//写5个数据然后再读回来进行比较，
//相同时返回值:0，表示在位;否则返回1，表示不在位	
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t buf1[5];
	uint8_t i;   	 
	NRF24L01_Write_Buf(SPI_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf1,5);              //读出写入的地址  	
	for(i=0;i<5;i++)if(buf1[i]!=0XA5)break;					   
	if(i!=5)return 1;                               //NRF24L01不在位	
	return 0;		                                //NRF24L01在位
}	 	 
//通过SPI写寄存器
uint8_t NRF24L01_Write_Reg(uint8_t regaddr,uint8_t data)
{
	uint8_t status;	
    Clr_NRF24L01_CSN;                    //使能SPI传输
  	status =SPI_RW(regaddr); //发送寄存器号 
  	SPI_RW(data);            //写入寄存器的值
  	Set_NRF24L01_CSN;                    //禁止SPI传输	   
  	return(status);       		         //返回状态值
}
//读取SPI寄存器值 ，regaddr:要读的寄存器
uint8_t NRF24L01_Read_Reg(uint8_t regaddr)
{
	uint8_t reg_val;	    
	Clr_NRF24L01_CSN;                //使能SPI传输		
  	SPI_RW(regaddr);     //发送寄存器号
  	reg_val=SPI_RW(0XFF);//读取寄存器内容
  	Set_NRF24L01_CSN;                //禁止SPI传输		    
  	return(reg_val);                 //返回状态值
}	
//在指定位置读出指定长度的数据
//*pBuf:数据指针
//返回值,此次读到的状态寄存器值 
uint8_t NRF24L01_Read_Buf(uint8_t regaddr,uint8_t *pBuf,uint8_t datalen)
{
	uint8_t status,uint8_t_ctr;	       
  	Clr_NRF24L01_CSN;                     //使能SPI传输
  	status=SPI_RW(regaddr);   //发送寄存器值(位置),并读取状态值   	   
	for(uint8_t_ctr=0;uint8_t_ctr<datalen;uint8_t_ctr++)pBuf[uint8_t_ctr]=SPI_RW(0XFF);//读出数据
  	Set_NRF24L01_CSN;                     //关闭SPI传输
  	return status;                        //返回读到的状态值
}
//在指定位置写指定长度的数据
//*pBuf:数据指针
//返回值,此次读到的状态寄存器值
uint8_t NRF24L01_Write_Buf(uint8_t regaddr, uint8_t *pBuf, uint8_t datalen)
{
	uint8_t status,uint8_t_ctr;	    
	Clr_NRF24L01_CSN;                                    //使能SPI传输
  	status = SPI_RW(regaddr);                //发送寄存器值(位置),并读取状态值
  	for(uint8_t_ctr=0; uint8_t_ctr<datalen; uint8_t_ctr++)SPI_RW(*pBuf++); //写入数据	 
  	Set_NRF24L01_CSN;                                    //关闭SPI传输
  	return status;                                       //返回读到的状态值
}				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t state;   
	uint16_t timeout = 1000;
	Clr_NRF24L01_CE;
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
	Set_NRF24L01_CE;                                     //启动发送	   
	while(READ_NRF24L01_IRQ!=0 && timeout--);            //等待发送完成，超时退出
	state=NRF24L01_Read_Reg(STATUS);                     //读取状态寄存器的值	   
	NRF24L01_Write_Reg(SPI_WRITE_REG+STATUS,state);      //清除TX_DS或MAX_RT中断标志
	if(state&MAX_TX)                                     //达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);               //清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(state&TX_OK)                                      //发送完成
	{
		return TX_OK;
	}
	return 0xff;                                         //其他原因发送失败
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;		    							      
	state=NRF24L01_Read_Reg(STATUS);                //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(SPI_WRITE_REG+STATUS,state); //清除TX_DS或MAX_RT中断标志
	if(state&RX_OK)                                 //接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);          //清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;                                      //没收到任何数据
}

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
//void RX_Mode(void)
//{
//	Clr_NRF24L01_CE;	  
//    //写RX节点地址
//  	NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);

//    //使能通道0的自动应答    
//  	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);    
//    //使能通道0的接收地址  	 
//  	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
//    //设置RF通信频率		  
//  	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_CH,40);	     
//    //选择通道0的有效数据宽度 	    
//  	NRF24L01_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
//    //设置TX发射参数,0db增益,1Mbps,低噪声增益开启   
//  	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x0f);
//    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX接收模式 
//  	NRF24L01_Write_Reg(SPI_WRITE_REG+NCONFIG, 0x0f); 
//    //CE为高,进入接收模式 
//  	Set_NRF24L01_CE;                                
//}			
//		


//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,
//选择RF频道,波特率和LNA HCURR PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void TX_Mode(void)
{														 
	Clr_NRF24L01_CE;	    
    //写TX节点地址 
  	NRF24L01_Write_Buf(SPI_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);    
    //设置TX节点地址,主要为了使能ACK	  
  	NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); 

    //使能通道0的自动应答    
  	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);     
    //使能通道0的接收地址  
  	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01); 
    //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_Write_Reg(SPI_WRITE_REG+SETUP_RETR,0x1a);
    //设置RF通道为1
  	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_CH,1);       
    //设置TX发射参数,0db增益,1Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x07);  //0x27  250K   0x07 1M
    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX发送模式,开启所有中断
  	NRF24L01_Write_Reg(SPI_WRITE_REG+NCONFIG,0x0e);    
    // CE为高,10us后启动发送
	Set_NRF24L01_CE;                                  
}	

void NRF24L01_Init(void)
{	
	while(NRF24L01_Check()){};
	TX_Mode();
}

