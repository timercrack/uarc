//============================================================================
// Name        : uarc_rmemd.cpp
// Author      :
// Version     :
// Copyright   : 
// Description :
//============================================================================


#include "uarc_rmemd.h"

#include "GatherSubsystem.h"
#include "CalcSubsystem.h"
#include "DataProcess.h"
#include "StoreExeSubsystem.h"
#include "WaitAllPerArchSubsystem.h"
#include "ClearHistory.h"

#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Thread.h>
#include <Poco/ThreadPool.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/Process.h>
#include <iostream>
#include <sys/file.h>
#include <iostream>
#include <sys/file.h>


using Poco::Util::Application;
using Poco::Util::ServerApplication;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using Poco::Event;

using namespace std;
Poco::SharedPtr<Poco::Event> g_pevtWakeup;


UarcRmemdServer::UarcRmemdServer() : _helpRequested(false) {
//地址需要改变
	int pid_file = ::open("/var/lock/uarc_rmemd.lock", O_CREAT | O_RDWR, 0666);
	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
	if(rc) {
		std::cout << "UarcRmemdServer进程已存在，请不要重复运行！" << std::endl;
		exit(-1);
	}

	addSubsystem(new ClearHistory);
	addSubsystem(new GatherSubsystem);
	addSubsystem(new StoreExeSubsystem);
	addSubsystem(new CalcSubsystem);
	addSubsystem(new DataProcess);
	addSubsystem(new WaitAllPerArchSubsystem);



	g_pevtWakeup = new Poco::Event(false);
	g_pevtWakeup->reset();
}
UarcRmemdServer::~UarcRmemdServer() {
}

void UarcRmemdServer::initialize(Application & self)
{


	// load configuration files, if present
	loadConfiguration("/etc/uarc/uarc-cfg/uarc_rmemd.properties");
	ServerApplication::initialize(self);
	logger().information("-----能源网运行调控平台数据归集与计算-----",__FILE__,__LINE__);
	logger().information("starting up\n", __FILE__, __LINE__);

}

void UarcRmemdServer::uninitialize()
{
	logger().information("shutting down\n",__FILE__,__LINE__);
	ServerApplication::uninitialize();
}

void UarcRmemdServer::handleOption(const string & name, const string & value)
{
	ServerApplication::handleOption(name, value);

	if (name == "help")
		_helpRequested = true;
}
void UarcRmemdServer::defineOptions(OptionSet & options)
{
	ServerApplication::defineOptions(options);

	options.addOption(Option("help", "h", "display help information on command line arguments")
			.required(false)
			.repeatable(false)
			.callback(OptionCallback<UarcRmemdServer>(this, &UarcRmemdServer::handleHelp)));
}

void UarcRmemdServer::handleHelp(const string& name, const string& value)
{
	_helpRequested = true;
	displayHelp();
	stopOptionsProcessing();
}

void UarcRmemdServer::displayHelp()
{
	HelpFormatter helpFormatter(options());
	helpFormatter.setCommand(commandName());
	helpFormatter.setUsage("OPTIONS");
	helpFormatter.setHeader("UarcRmemdServer");
	helpFormatter.format(cout);
}
int UarcRmemdServer::main(const std::vector<std::string>& args)
{

	if ( _helpRequested ) {

			displayHelp();

			return Application::EXIT_OK;
		}

	try {
		// laungh subsystems
		getSubsystem<ClearHistory> ().Start();
		getSubsystem<GatherSubsystem> ().Start();
		getSubsystem<CalcSubsystem> ().Start();
		getSubsystem<DataProcess> ().Start();
		getSubsystem<StoreExeSubsystem> ().Start();
		getSubsystem<WaitAllPerArchSubsystem> ().Start();




	} catch ( Poco::Exception& e ) {

			UarcRmemdServer::GetLogger().information(e.message(), __FILE__, __LINE__);
			return Application::EXIT_CANTCREAT;
		}

		// wait for CTRL-C or kill
		waitForTerminationRequest();
		g_pevtWakeup->set();

    getSubsystem<StoreExeSubsystem> ().Stop();
	printf("已经关闭归集指标处理线程！\n");
	getSubsystem<GatherSubsystem> ().Stop();
	printf("已经关闭归集回调任务线程！\n");
	getSubsystem<CalcSubsystem> ().Stop();
	printf("已经关闭计算任务线程！\n");
	getSubsystem<DataProcess> ().Stop();
	printf("已经关闭数据处理任务线程！\n");
	getSubsystem<WaitAllPerArchSubsystem> ().Stop();
	printf("已经关闭初始归集指标获取线程！\n");
	getSubsystem<ClearHistory> ().Stop();
	printf("已经关闭定时清除历史数据线程！\n");



		UarcRmemdServer::GetLogger().information("UarcRmemdServer quit.",	__FILE__, __LINE__);

		return Application::EXIT_OK;
}
POCO_SERVER_MAIN(UarcRmemdServer)
