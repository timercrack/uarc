#include "uarc_colld.h"
#include "CollDataSubsystem.h"
#include "CollTaskSubsystem.h"
#include "RecvDataSubsystem.h"
#include "SendCmdSubsystem.h"
#include "SendScheduleSubsystem.h"
#include "SendZhaoceSubsystem.h"
#include "VIEC104/VIEC104Subsystem.h"
#include "IEC104/IEC104Subsystem.h"
#include "NX/NxSubsystem.h"
#include <Poco/Thread.h>
#include <Poco/ThreadPool.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/Process.h>
#include <iostream>
#include <sys/file.h>

using Poco::Process;
using Poco::File;
using Poco::Path;
using Poco::Thread;
using Poco::ThreadPool;
using Poco::Util::Application;
using Poco::Util::ServerApplication;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using Poco::Event;
using Uarc::Protocol::VIEC104::VIEC104Subsystem;
using Uarc::Protocol::IEC104::IEC104Subsystem;
using Uarc::Protocol::NX::NxSubsystem;

using namespace std;

bool UARCCollServer::_IEC104 = false;
bool UARCCollServer::_VIEC104 = false;
bool UARCCollServer::_Nx = false;

UARCCollServer::UARCCollServer() : _helpRequested(false) {

	int pid_file = ::open("/var/lock/uarc_colld.lock", O_CREAT | O_RDWR, 0666);
	int rc = ::flock(pid_file, LOCK_EX | LOCK_NB);
	if(rc) {
		logger().information("uarc_colld进程已存在，请不要重复运行！");
		exit(-1);
	}

	addSubsystem(new CollDataSubsystem);
	addSubsystem(new CollTaskSubsystem);
	addSubsystem(new RecvDataSubsystem);
	addSubsystem(new SendCmdSubsystem);
	addSubsystem(new SendScheduleSubsystem);
	addSubsystem(new SendZhaoceSubsystem);
	addSubsystem(new VIEC104Subsystem);
	addSubsystem(new IEC104Subsystem);
	addSubsystem(new NxSubsystem);

}

UARCCollServer::~UARCCollServer() {

}

void UARCCollServer::initialize(Application & self)
{
	// load configuration files, if present
	loadConfiguration("/etc/uarc/uarc-cfg/uarc_colld.properties");
	ServerApplication::initialize(self);
	_VIEC104 = self.config().getBool("virtual_iec104", false);
	if (!_VIEC104)
	{
		_IEC104 = self.config().getBool("iec104", false);
	}
	_Nx = self.config().getBool("nx", false);
	logger().information("starting up",__FILE__,__LINE__);
}

void UARCCollServer::uninitialize()
{
	logger().information("shutting down",__FILE__,__LINE__);
	ServerApplication::uninitialize();
}

void UARCCollServer::handleOption(const string & name, const string & value)
{
	ServerApplication::handleOption(name, value);

	if (name == "help")
		_helpRequested = true;
}

void UARCCollServer::defineOptions(OptionSet & options)
{
	ServerApplication::defineOptions(options);

	options.addOption(
		Option("help", "h", "display help information on command line arguments")
			.required(false)
			.repeatable(false)
			.callback(OptionCallback<UARCCollServer>(this, &UARCCollServer::handleHelp)));
}

void UARCCollServer::handleHelp(const string& name, const string& value)
{
	_helpRequested = true;
	displayHelp();
	stopOptionsProcessing();
}

void UARCCollServer::displayHelp()
{
	HelpFormatter helpFormatter(options());
	helpFormatter.setCommand(commandName());
	helpFormatter.setUsage("OPTIONS");
	helpFormatter.setHeader("uarc data collection service.");
	helpFormatter.format(cout);
}

int UARCCollServer::main(const vector<string> & args)
{
	if ( _helpRequested ) {

		displayHelp();

		return Application::EXIT_OK;
	}

	try {

		// laungh subsystems
		if (_VIEC104)
		{
			getSubsystem<VIEC104Subsystem>().start();
		}else if (_IEC104){
			getSubsystem<IEC104Subsystem>().start();
		}
		if (_Nx)
		{
			getSubsystem<NxSubsystem>().start();
		}
		getSubsystem<CollTaskSubsystem>().Start();
		getSubsystem<CollDataSubsystem>().Start();
		getSubsystem<SendZhaoceSubsystem>().Start();
		getSubsystem<SendCmdSubsystem>().Start();
		getSubsystem<SendScheduleSubsystem>().Start();
		getSubsystem<RecvDataSubsystem>().Start();

	} catch ( Poco::Exception& e ) {

		logger().information(e.message(), __FILE__, __LINE__);
		return Application::EXIT_CANTCREAT;
	}

	// wait for CTRL-C or kill
	waitForTerminationRequest();

	getSubsystem<CollTaskSubsystem>().Stop();
	getSubsystem<CollDataSubsystem>().Stop();
	getSubsystem<SendCmdSubsystem>().Stop();
	getSubsystem<SendScheduleSubsystem>().Stop();
	getSubsystem<SendZhaoceSubsystem>().Stop();
	getSubsystem<RecvDataSubsystem>().Stop();
	if (_VIEC104)
	{
		getSubsystem<VIEC104Subsystem>().stop();
	}else if (_IEC104){
		getSubsystem<IEC104Subsystem>().stop();
	}
	if (_Nx)
	{
		getSubsystem<NxSubsystem>().stop();
	}

	return Application::EXIT_OK;
}

bool setReceiveCallBack(Receive_F receive)
{
	bool result = true;
	if (UARCCollServer::_VIEC104)
	{
		result = UARCCollServer::instance().getSubsystem<VIEC104Subsystem>().setReceiveCallBack(receive);
	}else if (UARCCollServer::_IEC104){
		result = UARCCollServer::instance().getSubsystem<IEC104Subsystem>().setReceiveCallBack(receive);
	}
	if (UARCCollServer::_Nx)
	{
		result = UARCCollServer::instance().getSubsystem<NxSubsystem>().setReceiveCallBack(receive);
	}

	return result;
}

bool send(int deviceId, int sendType, const vector<DataItem> &dataItems)
{
	bool result = false;
	if (UARCCollServer::_VIEC104)
	{
		result = UARCCollServer::instance().getSubsystem<VIEC104Subsystem>().send(deviceId, sendType, dataItems);
	}else if (UARCCollServer::_IEC104){
		result = UARCCollServer::instance().getSubsystem<IEC104Subsystem>().send(deviceId, sendType, dataItems);
	}
	if (UARCCollServer::_Nx)
	{
		result = UARCCollServer::instance().getSubsystem<NxSubsystem>().send(deviceId, sendType, dataItems);
	}

	return result;
}

POCO_SERVER_MAIN(UARCCollServer)
