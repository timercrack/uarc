/*
 * ClearNotofication.h
 *
 *  Created on: 2015-3-2
 *      Author: root
 */

#ifndef CLEARNOTOFICATION_H_
#define CLEARNOTOFICATION_H_
#include "rdbms.h"
#include "Poco/TimedNotificationQueue.h"
#include "Poco/Notification.h"
#include "Poco/Timestamp.h"


using Poco::TimedNotificationQueue;
using Poco::Notification;
using Poco::Timestamp;


class ClearNotofication: public Notification
{
public:
	ClearNotofication(int ClearTime) :_ClearTime(ClearTime)
	{
	}

	const int ClearTime() const
	{
		return _ClearTime;
	}
/*
	std::string ClearString() const
	{
		return _ClearString;
	}
*/

private:

	int _ClearTime;
//	std::string _ClearString;

};


#endif /* CLEARNOTOFICATION_H_ */
