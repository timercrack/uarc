/*
 * uarc_rmemd.h
 *
 *  Created on: 2014-10-31
 *      Author:
 */

#ifndef UARC_RMEMD_H_
#define UARC_RMEMD_H_

#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/SharedPtr.h>
#include <Poco/HashMap.h>
#include <Poco/RunnableAdapter.h>
#include <iostream>
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include <Poco/TimedNotificationQueue.h>
#include "StoreNotification.h"


using Poco::NotificationQueue;
using Poco::Notification;
using Poco::NotificationQueue;
using Poco::ThreadPool;
using Poco::Thread;


class UarcRmemdServer: public Poco::Util::ServerApplication {

public:

	UarcRmemdServer();
	virtual ~UarcRmemdServer();

	static Poco::Logger& GetLogger();

protected:

	void initialize(Application& self);

	void uninitialize();

	void defineOptions(Poco::Util::OptionSet& options);

	void handleOption(const std::string& name, const std::string& value);

	void handleHelp(const std::string& name, const std::string& value);

	void displayHelp();

	int main(const std::vector<std::string>& args);

private:

	enum {

		RedisConnectType_TCP, RedisConnectType_UNIX
	};
	bool _helpRequested;
};

inline Poco::Logger & UarcRmemdServer::GetLogger() {
	return dynamic_cast<UarcRmemdServer&> (UarcRmemdServer::instance()).logger();
}
extern Poco::SharedPtr<Poco::Event> g_pevtWakeup;

#endif /* UARC_RMEMD_H_ */
