#pragma once

#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/SharedPtr.h>
#include <Poco/HashMap.h>
#include <Poco/RunnableAdapter.h>
#include <iostream>

#include <Poco/Event.h>
#include "CommStruct.h"

using namespace std;

class UARCCollServer : public Poco::Util::ServerApplication {

public:

	UARCCollServer();
	virtual ~UARCCollServer();

	static Poco::Logger& GetLogger();

	static bool _IEC104;
	static bool _VIEC104;
	static bool _Nx;

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

		RedisConnectType_TCP,
		RedisConnectType_UNIX
	};
	bool _helpRequested;
};

bool setReceiveCallBack(Receive_F receive);
bool send(int deviceId, int sendType, const vector<DataItem> &dataItems);

inline Poco::Logger & UARCCollServer::GetLogger()
{
	return dynamic_cast<UARCCollServer&>( UARCCollServer::instance() ).logger();
}

extern Poco::SharedPtr<Poco::Event> g_pevtWakeup;
