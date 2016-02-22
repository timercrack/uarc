/*
 * StoreNotification.h
 *
 *  Created on: 2015-1-31
 *      Author: root
 */

#ifndef STORENOTIFICATION_H_
#define STORENOTIFICATION_H_

#include "rdbms.h"
#include "Poco/TimedNotificationQueue.h"
#include "Poco/Notification.h"
#include "Poco/Timestamp.h"


using Poco::TimedNotificationQueue;
using Poco::Notification;
using Poco::Timestamp;


namespace
{
class StoreNotification: public Notification
{
public:
	StoreNotification(int StoreTime, int SecStoreFrequent) :_StoreTime(StoreTime), _SecStoreFrequent(SecStoreFrequent)
	{
	}
/*
	 ~StoreNotification()
	 {
	 }
*/
	const int StoreTime() const
	{
		return _StoreTime;
	}
	const int SecStoreFrequent() const
	{
		return _SecStoreFrequent;
	}

	//加入归集时间？归集数量对象

private:

	int _StoreTime;
	int _SecStoreFrequent;

};
}
#endif /* STORENOTIFICATION_H_ */
