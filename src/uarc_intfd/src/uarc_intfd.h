#pragma once

#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/SharedPtr.h>
#include <Poco/HashMap.h>
#include <Poco/RunnableAdapter.h>
#include <iostream>

class UARCIntfServer : public Poco::Util::ServerApplication {

public:

	UARCIntfServer();
	virtual ~UARCIntfServer();

	static Poco::Logger& GetLogger();

	static std::string rtdbms_ip;
	static int		   rtdbms_port;

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

inline Poco::Logger & UARCIntfServer::GetLogger()
{
	return dynamic_cast<UARCIntfServer&>( UARCIntfServer::instance() ).logger();
}

