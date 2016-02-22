/*
 * global_104.h
 *
 *  Created on: 2013-5-6
 *      Author: fes
 */

#ifndef GLOBAL_104_H_
#define GLOBAL_104_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include <errno.h>
#include <list>
#include <fcntl.h>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

#define LOG_FES_104						"fes_104.log"

/*****************************************************************************
标准传输原因定义 Start
***********************************************/
#define REASON_NOT_USED					0			//未用
#define	REASON_PER_CYC					1			//周期、循环
#define	REASON_BACK						2			//背景扫描
#define	REASON_SPONT					3			//突发
#define	REASON_INIT						4			//初始化
#define	REASON_REQ						5			//请求或者被请求
#define	REASON_ACT						6			//激活
#define	REASON_ACT_CON					7			//激活确认
#define	REASON_DEACT					8			//停止激活
#define	REASON_DEACT_CON				9			//停止激活确认
#define	REASON_ACT_TERM					10			//激活停止
#define	REASON_RET_REM					11			//远方命令引起的返送信息
#define	REASON_RET_LOC					12			//当地命令引起的返送信息
#define	REASON_FILE						13			//文件传输

//14～19	保留
#define	REASON_INRO_EN					20			//响应站召唤
#define	REASON_INRO_1					21			//响应第1组召唤
#define	REASON_INRO_2					22			//响应第2组召唤
#define	REASON_INRO_3					23			//响应第3组召唤
#define	REASON_INRO_4					24			//响应第4组召唤
#define	REASON_INRO_5					25			//响应第5组召唤
#define	REASON_INRO_6					26			//响应第6组召唤
#define	REASON_INRO_7					27			//响应第7组召唤
#define	REASON_INRO_8					28			//响应第8组召唤
#define	REASON_INRO_9					29			//响应第9组召唤
#define	REASON_INRO_10					30			//响应第10组召唤
#define	REASON_INRO_11					31			//响应第11组召唤
#define	REASON_INRO_12					32			//响应第12组召唤
#define	REASON_INRO_13					33			//响应第13组召唤
#define	REASON_INRO_14					34			//响应第14组召唤
#define	REASON_INRO_15					35			//响应第15组召唤
#define	REASON_INRO_16					36			//响应第16组召唤
#define	REASON_REQ_COG_EN				37			//响应计数量站召唤
#define	REASON_REQ_CO_1					38			//响应笫1组计数量召唤
#define	REASON_REQ_CO_2					39			//响应笫2组计数量召唤
#define	REASON_REQ_CO_3					40			//响应笫3组计数量召唤
#define	REASON_REQ_CO_4					41			//响应笫4组计数量召唤

//42～43	为配套标准保留(兼容范围)
#define	REASON_UNKOWN_TYPE				44			//未知的类型标识
#define	REASON_UNKOWN					45			//未知的传输原因
#define	REASON_UNKOWN_ADDR				46			//未知的应用服务数据单元公共地址
#define	REASON_UNKOWN_OBJ_ADDR			47			//未知的信息对象地址
//48～63	特殊应用保留（专用范围）

/*******************************************
标准传输原因定义 End
*****************************************************************************/

/*****************************************************************************
类型标志 Start
***********************************************/
#define M_SP_NA_1						1			//单点信息
#define M_DP_NA_1						3			//双点信息

#define M_ME_NA_1						9			//测量值，规一化值
#define M_ME_NB_1						11			//测量值，标度化值
#define M_ME_NC_1						13			//测量值，短浮点数
#define M_ME_ND_1						21			//测量值，规一化值，不带品质描述

#define M_IT_NA_1						15			//累计量

#define M_SP_TB_1						30			//带长时标，单点信息
#define M_DP_TB_1						31			//带长时标，双点信息
#define M_ME_TF_1						36			//带长时标，测量值，短浮点数

#define M_EP_TD_1						38			//带长时标的继电保护装置事件

///////
#define C_SC_NA_1						45			//单点命令
#define C_DC_NA_1						46			//双点命令

#define C_SE_NA_1						48			//归一化设点命令
#define C_SE_NB_1						49			//标度化设点命令
#define C_SE_NC_1						50			//短浮点数设点命令

#define C_SC_TA_1						58			//带长时标，单点命令
#define C_DC_TA_1						59			//带长时标，双点命令

#define C_SE_TA_1						61			//带长时标，归一化设点命令
#define C_SE_TB_1						62			//带长时标，标度化设点命令
#define C_SE_TC_1						63			//带长时标，短浮点数设点命令

#define C_RD_NA_1						102			//单点召唤
/*******************************************
类型标志 End
*****************************************************************************/

//自定义传输原因，用来判断链路及数据请求传输
#define	REASON_CUS_TEST_LINK			101			//测试链路请求
#define REASON_CUS_REQ_TEST_LINK		102			//测试链路回应
#define REASON_CUS_ACTIVATION			103			//激活请求
#define REASON_CUS_REQ_ACTIVATION		104			//激活请求回应

#define REASON_CUS_STOP_LINK			105			//断开连接
#define REASON_CUS_REQ_STOP_LINK		106			//断开连接回应

//下一步要发送的帧类型
#define STATUS_WAIT_SEND_TEST			0			//等待发送测试链路
#define STATUS_WAIT_SEND_DATA_TRANSFER	1			//等待发送数据请求
#define STATUS_WAIT_SEND_CALLALL		2			//等待发送总召
#define STATUS_WAIT_SEND_S				3			//等待发送S帧
#define STATUS_WAIT_SEND_U				4			//等待发送U帧
#define STATUS_WAIT_SEND_I				5			//等待发送I帧

#define STATUS_OFF						0			//TCP断开
#define STATUS_ON						1			//连接正常

//帧格式
#define FRAME_S							100			//S帧
#define FRAME_U							101			//U帧
#define FRAME_I							103			//I帧

//配置文件值含义
#define CFG_VAL_INTERVAL				0
#define CFG_VAL_K						1
#define CFG_VAL_W						2
#define CFG_VAL_T0						3
#define CFG_VAL_T1						4
#define CFG_VAL_T2						5
#define CFG_VAL_T3						6

//长度定义
#define LENGTH_APCI						6
#define LENGTH_OBJADDR					3
#define LENGTH_REASON					2
#define LENGTH_ADDR						2

//命令报文状态
#define SELECT_CMD						1
#define EXEC_CMD						2
#define UNDO_CMD						3

#pragma pack(1)
union stAddr//信息体地址
{
	struct
	{
		uint8_t objAddrChar1;
		uint8_t objAddrChar2;
		uint8_t objAddrChar3;
	};
	uint8_t objAddr[3];
};

union stQualityDes//品质描述
{
	struct
	{
		uint8_t	ov_d0		:	1;
		uint8_t	res_d1_d3	:	3;
		uint8_t	bl_d4		:	1;
		uint8_t	sb_d5		:	1;
		uint8_t	nt_d6		:	1;
		uint8_t	iv_d7		:	1;
	};
	uint8_t		val_des;
};

union stTimeStamp//时标
{
	struct
	{
		union//millisec
		{
			struct
			{
				uint8_t low_millisec;
				uint8_t high_millisec;
			};
			uint16_t	millisec;
		};
		union//minute
		{
			struct
			{
				uint8_t min_val_d0_d5	:	6;
				uint8_t min_res_d6	:	1;
				uint8_t min_iv_d7	:	1;
			};
			uint8_t		min;
		};
		union//hour
		{
			struct
			{
				uint8_t hour_val_d0_d4		:	5;
				uint8_t hour_res_d5_d6	:	2;
				uint8_t hour_su_d7		:	1;
			};
			uint8_t		hour;
		};
		union//day
		{
			struct
			{
				uint8_t day_val_d0_d4	:	5;
				uint8_t	day_d5_d7		:	3;
			};
			uint8_t		day;
		};
		union//month
		{
			struct
			{
				uint8_t month_val_d0_d3		:	4;
				uint8_t month_res_d4_d7		:	4;
			};
			uint8_t		month;
		};
		union//year
		{
			struct
			{
				uint8_t year_val_d0_d6	:	7;
				uint8_t year_res_d7		:	1;
			};
			uint8_t		year;
		};
	};
};

//////////////////
struct stHead
{
	uint8_t				start;
	uint8_t				length;
	union
	{
		struct
		{
			uint8_t		frameI_D0		:1;
			uint8_t		lsb_D1_D7		:7;
		};
		struct
		{
			uint8_t		frameSU_D0_D1	:2;
			uint8_t		lsb_D2_D7		:6;
		};
		uint8_t			control1;
	};
	uint8_t				control2;
	uint8_t				control3;
	uint8_t				control4;

	//ASDU
	uint8_t type;
	union
	{
		struct
		{
			uint8_t itemCount_D0_D6 :7;
			uint8_t vsq_D7 :1;
		};
		uint8_t vsq;
	};
	uint16_t reason;
	uint16_t addr;
	stAddr objAddr;
};

struct stFrame
{
	stHead		head;
	uint8_t		buf[255];
};

/*****************************************************************************
数据单元类型结构体 Start
 ***********************************************/
struct stM_SP_NA_1									//单点信息,信息体地址非连续
{
	stAddr objAddr;
	union
	{
		struct
		{
			uint8_t	spi_d0		:	1;
			uint8_t	res_d1_d3	:	3;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
};

struct stM_SP_NA_1_SERIAL							//单点信息,信息体地址连续
{
	union
	{
		struct
		{
			uint8_t	spi_d0		:	1;
			uint8_t	res_d1_d3	:	3;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
};

struct stM_DP_NA_1									//双点信息,信息体地址非连续
{
	stAddr objAddr;
	union
	{
		struct
		{
			uint8_t	dpi_d0_d1	:	2;
			uint8_t	res_d2_d3	:	2;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
};

struct stM_DP_NA_1_SERIAL							//双点信息,信息体地址连续
{
	union
	{
		struct
		{
			uint8_t	dpi_d0_d1	:	2;
			uint8_t	res_d2_d3	:	2;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
};

struct stM_ME_NA_1									//测量值，规一化值,信息体地址非连续
{
	stAddr 			objAddr;
	uint16_t 		val;
	stQualityDes	quality_des;
};

struct stM_ME_NA_1_SERIAL							//测量值，规一化值,信息体地址连续
{
	uint16_t 		val;
	stQualityDes	quality_des;
};

struct stM_ME_NB_1									//测量值，标度化值，信息体地址非连续
{
	stAddr			objAddr;
	uint16_t 		val;
	stQualityDes	quality_des;
};

struct stM_ME_NB_1_SERIAL							//测量值，标度化值，信息体地址连续
{
	uint16_t 		val;
	stQualityDes	quality_des;
};

struct stM_ME_NC_1									//测量值，短浮点数，信息体地址非连续
{
	stAddr 			objAddr;
	float		val;
	stQualityDes	quality_des;
};

struct stM_ME_NC_1_SERIAL							//测量值，短浮点数，信息体地址连续
{
	float		val;
	stQualityDes	quality_des;
};

struct stM_ME_ND_1									//测量值，规一化值，不带品质描述，信息体地址非连续
{
	stAddr 			objAddr;
	uint16_t 		val;
};

struct stM_ME_ND_1_SERIAL							//测量值，规一化值，不带品质描述，信息体地址连续
{
	uint16_t val;
};

struct stM_IT_NA_1									//累计量,信息体地址非连续
{
	stAddr objAddr;
	union
	{
		struct
		{
			uint8_t	lowChar_d0_d6:	7;
			uint8_t	lowChar_d7	 :	1;
			uint8_t	highChar;
		};
		uint16_t		val;
	};
};

struct stM_IT_NA_1_SERIAL							//累计量,信息体地址连续
{
	union
	{
		struct
		{
			uint8_t	lowChar_d0_d6:	7;
			uint8_t	lowChar_d7	 :	1;
			uint8_t	highChar;
		};
		uint16_t		val;
	};
};

struct stM_SP_TB_1									//带长时标，单点信息，信息体地址非连续
{
	stAddr objAddr;
	union//data
	{
		struct
		{
			uint8_t	spi_d0		:	1;
			uint8_t	res_d1_d3	:	3;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
	stTimeStamp time_stamp;
};

struct stM_SP_TB_1_SERIAL							//带长时标，单点信息，信息体地址连续
{
	union//data
	{
		struct
		{
			uint8_t	spi_d0		:	1;
			uint8_t	res_d1_d3	:	3;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
	stTimeStamp time_stamp;
};

struct stM_DP_TB_1									//带长时标，双点信息，信息体地址非连续
{
	stAddr objAddr;
	union
	{
		struct
		{
			uint8_t	dpi_d0_d1	:	2;
			uint8_t	res_d2_d3	:	2;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
	stTimeStamp time_stamp;
};

struct stM_DP_TB_1_SERIAL							//带长时标，双点信息，信息体地址连续
{
	union
	{
		struct
		{
			uint8_t	dpi_d0_d1	:	2;
			uint8_t	res_d2_d3	:	2;
			uint8_t	bl_d4		:	1;
			uint8_t	sb_d5		:	1;
			uint8_t	nt_d6		:	1;
			uint8_t	iv_d7		:	1;
		};
		uint8_t		val;
	};
	stTimeStamp time_stamp;
};

struct stM_ME_TF_1									//测量值，短浮点数，信息体地址非连续
{
	stAddr objAddr;
	float 			val;
	stQualityDes	quality_des;
	stTimeStamp time_stamp;
};

struct stM_ME_TF_1_SERIAL							//测量值，短浮点数，信息体地址连续
{
	float 			val;
	stQualityDes	quality_des;
	stTimeStamp time_stamp;
};

struct stM_EP_TD_1									//带长时标的继电保护装置事件，信息体地址非连续
{
	stAddr objAddr;
	uint8_t	val;
	uint16_t action_time;
	stTimeStamp time_stamp;
};

struct stM_EP_TD_1_SERIAL							//带长时标的继电保护装置事件，信息体地址连续
{
	uint8_t	val;
	uint16_t action_time;
	stTimeStamp time_stamp;
};

///////////////命令
struct stC_SC_NA_1						//单点命令
{
	union
	{
		struct
		{
			uint8_t scs_d0		:	1;
			uint8_t res_d1		:	1;
			uint8_t qu_d2_d6	:	5;
			uint8_t se_d7		:	1;
		};
		uint8_t val;
	};
};

struct stC_DC_NA_1						//双点命令
{
	union
	{
		struct
		{
			uint8_t dcs_d0_d1	:	2;
			uint8_t qu_d2_d6	:	5;
			uint8_t se_d7		:	1;
		};
		uint8_t val;
	};
};

union stQos//设定值命令品质限定值
{
	struct
	{
		uint8_t ql_d0_d6	:	7;
		uint8_t se_d7		:	1;
	};
	uint8_t qos;
};

struct stC_SE_NA_1						//归一化设点命令
{
	uint16_t 		val;
	stQos			qos;
};

struct stC_SE_NB_1						//标度化设点命令
{
	uint16_t 		val;
	stQos			qos;
};

struct stC_SE_NC_1						//短浮点数设点命令
{
	float		val;
	stQos			qos;
};

struct stC_SC_TA_1						//带长时标，单点命令
{
	union
	{
		struct
		{
			uint8_t scs_d0		:	1;
			uint8_t res_d1		:	1;
			uint8_t qu_d2_d6	:	5;
			uint8_t se_d7		:	1;
		};
		uint8_t val;
	};
	stTimeStamp time_stamp;
};

struct stC_DC_TA_1						//带长时标，双点命令
{
	union
	{
		struct
		{
			uint8_t dcs_d0_d1	:	2;
			uint8_t qu_d2_d6	:	5;
			uint8_t se_d7		:	1;
		};
		uint8_t val;
	};
	stTimeStamp time_stamp;
};

struct stC_SE_TA_1						//带长时标，归一化设点命令
{
	uint16_t 		val;
	stQos			qos;
	stTimeStamp time_stamp;
};

struct stC_SE_TB_1						//带长时标，标度化设点命令
{
	uint16_t 		val;
	stQos			qos;
	stTimeStamp time_stamp;
};

struct stC_SE_TC_1						//带长时标，短浮点数设点命令
{
	float		val;
	stQos			qos;
	stTimeStamp time_stamp;
};

/*****************************************************************************
数据单元类型结构体 End
***********************************************/
#pragma pack()

#endif /* GLOBAL_104_H_ */
