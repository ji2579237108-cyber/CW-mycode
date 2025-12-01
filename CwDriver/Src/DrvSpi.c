/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvSpi.c
作者: LXD
版本号:1.0
生成日期:2023.4.20
概述: SPI驱动
修改日志：
*********************************************************************/
#include "DrvSpi.h"


#define SPI_NUM_MAX		        (3U)		   	///< 模块内SPI最大计数

static S32 sSpiWorkMode[SPI_NUM_MAX] = {0};    	///< 0 : 单工模式; -1: 双工模式; -2: 4线模式


typedef struct 
{   
	SPI_WorkMode 	WorkMode;
	rcu_periph_enum mCS_RCU;
    uint32_t mCSPort;
    uint32_t mCSPin;
	rcu_periph_enum mSCK_RCU;
    uint32_t mSCKPort;
    uint32_t mSCKPin;
	rcu_periph_enum mMISO_RCU;
	uint32_t mMISOPort;
    uint32_t mMISOPin;
	rcu_periph_enum mMOSI_RCU;
	uint32_t mMOSIPort;
    uint32_t mMOSIPin;
    uint32_t mRemap;   
    rcu_periph_enum mRCU_SPI;
}SPI_RESOURCE;


/**
* @brief SPI外设基地址
*/
const static uint32_t sSpiBase[]=
{
	SPI0, SPI1, SPI2
};



/**
 * @brief SPI资源配置
 */

SPI_RESOURCE SPI_GPIO_Resource[]=
{
	{ 	 
		m4WireMode,
		RCU_GPIOA,GPIOA, GPIO_PIN_4,	//CS
		RCU_GPIOA,GPIOA, GPIO_PIN_5,	//SCK
		RCU_GPIOA,GPIOA, GPIO_PIN_6,	//MISO
		RCU_GPIOA,GPIOA, GPIO_PIN_7,	//MOSI
		NULL,							//remap
		RCU_SPI0,
	},//SPI0
};



/**
 * @brief SPI GPIO 初始化
 *
 * @param SpiId     SPI外设
 *
 */
//
void SpiInitPins(DRV_SPI_ID SpiId)
{	
	rcu_periph_clock_enable(SPI_GPIO_Resource[SpiId].mMISO_RCU | SPI_GPIO_Resource[SpiId].mMOSI_RCU | \
							SPI_GPIO_Resource[SpiId].mSCK_RCU | SPI_GPIO_Resource[SpiId].mCS_RCU);
	
	rcu_periph_clock_enable(SPI_GPIO_Resource[SpiId].mRCU_SPI);
	
	//CS
	DrvGpioInit(SPI_GPIO_Resource[SpiId].mCSPort, SPI_GPIO_Resource[SpiId].mCSPin, GPIO_MODE_OUT_PP);
	DrvGpioOperat(SPI_GPIO_Resource[SpiId].mCSPort, SPI_GPIO_Resource[SpiId].mCSPin,1);
		
	//SCK
	DrvGpioInit(SPI_GPIO_Resource[SpiId].mSCKPort, SPI_GPIO_Resource[SpiId].mSCKPin, GPIO_MODE_AF_PP);
	
	//MOSI
	DrvGpioInit(SPI_GPIO_Resource[SpiId].mMOSIPort, SPI_GPIO_Resource[SpiId].mMOSIPin, GPIO_MODE_AF_PP);
	
	//MISO
	DrvGpioInit(SPI_GPIO_Resource[SpiId].mMISOPort, SPI_GPIO_Resource[SpiId].mMISOPin,GPIO_MODE_IPU);
	
}



/**
 * @brief SPI初始化
 *
 * @param SpiId     SPI外设
 *
 */
S32 DrvSpiDeviceInit(DRV_SPI_ID SpiId )
{
	
	spi_parameter_struct    SPI_InitStructure;

	SpiInitPins(SpiId);
	
	SPI_InitStructure.clock_polarity_phase 	= SPI_CK_PL_HIGH_PH_2EDGE;
	SPI_InitStructure.device_mode 			= SPI_MASTER;
	SPI_InitStructure.endian 				= SPI_ENDIAN_MSB;
	SPI_InitStructure.frame_size 			= SPI_FRAMESIZE_8BIT;
	SPI_InitStructure.nss 					= SPI_NSS_SOFT;
	SPI_InitStructure.prescale 				= SPI_PSC_4;
	SPI_InitStructure.trans_mode 			= SPI_TRANSMODE_FULLDUPLEX;
	
	spi_init(sSpiBase[SpiId], &SPI_InitStructure);
	
	spi_enable(sSpiBase[SpiId]);
	
	return 0;
	
}


/**
* @brief	片选操作
*
* @param	SpiId		SPI编号
* @param	SpiCs		选择从设备 ，有多个从设备时 ，通过该参数选择  
* @param	CsHoldEn	0:拉低，非0:拉高  
*/
void DrvSpiCsSet(DRV_SPI_ID SpiId, const DRV_SPI_CS SpiCs, uint8_t set)
{
	DrvGpioOperat(SPI_GPIO_Resource[SpiId].mCSPort, SPI_GPIO_Resource[SpiId].mCSPin, set);
}


/**
* @brief	SPI阻塞发送接收单个byte
*
* @param	SpiId		SPI编号
* @param	byte		数据  
*/
//
uint8_t DrvSpiSendByte(DRV_SPI_ID SpiId, uint8_t byte)
{
	/*! Loop while DR register in not emplty */
	while (spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_TBE) == RESET);
	 
	/*!Send byte through the SPI1 peripheral */
	spi_i2s_data_transmit(sSpiBase[SpiId], byte);
	 
	/*! Wait to receive a byte */
	while (spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_RBNE) == RESET);
	 
	/*! Return the byte read from the SPI bus */
	return spi_i2s_data_receive(sSpiBase[SpiId]);
}



/**
* @brief	SPI阻塞传输
* @param	SpiId		SPI编号
* @param	SpiCs		SPI片选选择
* @param	CsHoldEn	SPI片选控制使能						
* @param	SendBuf		发送缓冲区，只接收时可为NULL
* @param	RecvBuf		接收缓冲区，只发送时可为NULL
* @param	Length		传输长度
* @return	成功返回0，失败返回非0
*/ 
S32 DrvSpiBlockTransfer(const DRV_SPI_ID SpiId, const DRV_SPI_CS SpiCs, const DRV_SPI_HOLD CsHoldEn, uint8_t *const SendBuf, uint8_t *const RecvBuf, uint32_t Length)
{
    uint32_t i;

    if((SendBuf == NULL) && (RecvBuf == NULL))
    {
        return -1;
    }

    if(DRV_SPI_HOLD_ACTIVE & CsHoldEn)											///< 拉低片选线, 选通
    {
        DrvSpiCsSet(SpiId, SpiCs, DRV_SPI_HOLD_ACTIVE);
    }
	
	while(spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_RBNE) != RESET)
	{
		spi_i2s_data_receive(sSpiBase[SpiId]);
	}

    i = 0;
    if(SendBuf != NULL && (RecvBuf == NULL))									///< 阻塞发送
    {   
        while (i < Length)
        {
            while(RESET == spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_TBE));
            spi_i2s_data_transmit(sSpiBase[SpiId], SendBuf[i++]);
            if(sSpiWorkMode[SpiId] == -1)										///< 双工模式
            {
                while (spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_RBNE) == RESET);
                spi_i2s_data_receive(sSpiBase[SpiId]);
            }
        }
        while(SET == spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_TRANS));
    }
	
    i = 0;
    if(RecvBuf != NULL )						
    { 
        if(SendBuf != NULL)														///< 阻塞发送并接收
        {
            while (i < Length)
            {
                while (spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_TBE) == RESET);
                spi_i2s_data_transmit(sSpiBase[SpiId], SendBuf[i]);

                while (spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_RBNE) == RESET);
                RecvBuf[i++] = spi_i2s_data_receive(sSpiBase[SpiId]);
            }
        }
        else
        {
            while (i < Length)													///< 阻塞接收
            {
                while (spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_TBE) == RESET);
                spi_i2s_data_transmit(sSpiBase[SpiId], 0xff);

                while (spi_i2s_flag_get(sSpiBase[SpiId], SPI_FLAG_RBNE) == RESET);
                RecvBuf[i++] = spi_i2s_data_receive(sSpiBase[SpiId]);
            }
        }
    } 
	
    if(DRV_SPI_HOLD_CLR & CsHoldEn)												///<  拉高片选线, 禁止片选
    {
        DrvSpiCsSet(SpiId, SpiCs, DRV_SPI_HOLD_CLR);
    }	
	
    return 0;
}






