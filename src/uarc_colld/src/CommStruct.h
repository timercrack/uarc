/*
 * CommStruct.h
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#ifndef COMMSTRUCT_H_
#define COMMSTRUCT_H_

#include <vector>

using std::vector;

typedef enum
{
    SEND_TRANS_NONE_TYPE,         // 发送数据不明
    SEND_COLLECTDATA_TYPE,        // 数据采集发送
    SEND_ZHAOCEARGC_TYPE,         // 参数召测发送
    SEND_CTRLCOMMAND_TYPE,         // 控制命令发送
    SEND_SCHEDULECMD_TYPE			//配比计划下发
}   SendType;

typedef enum
{
	RECV_TRANS_NONE_TYPE,         // 接收原因不明
	RECV_COLLECT_DATA_TYPE,   	  // 接收采集数据
	RECV_ZHAOCE_DATA_TYPE,        // 接收召测数据
	RECV_CTRL_COMMAND_TYPE,       // 接收控制数据
}   RecvType;

typedef struct {
	int termId;//终端表号
	int itemId;//指标id
	double value;//指标值
	long int time;//采集时间
}DataItem;

typedef void (*Receive_F)(int deviceId, int reason, const vector<DataItem> &dataItems);

#endif /* COMMSTRUCT_H_ */
