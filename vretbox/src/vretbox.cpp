/** TRECVid (Version 1.0) ******************************
* ******************************************************
*       _    _      ()_()
*      | |  | |    |(o o)
*   ___| | _| | ooO--`o'--Ooo
*  / __| |/ / |/ _ \ __|_  /
*  \__ \   <| |  __/ |_ / /
*  |___/_|\_\_|\___|\__/___|
*
* ******************************************************
* Purpose: 
* Input/Output:

* @author skletz
* @version 1.0 24/05/17
*
**/

#include "toolbase.hpp"
#include "trecvidxtraction.hpp"

#include <cpluslogger.hpp>
#include <cplusutil.hpp>

#include <boost/program_options.hpp>


static std::string PROGNAME = "VRETBOX";
static std::string PROGDESCRIPTION= "Video Retrieval Evaluation Toolbox";

boost::program_options::variables_map processProgramOptions(const int argc, const char *const argv[]);
void showDescription();
void showHelp(const boost::program_options::options_description& desc);

int main(int argc, char const *argv[]) {

	showDescription();
	boost::program_options::variables_map args = processProgramOptions(argc, argv);

	vretbox::ToolBase* tool = nullptr;

	if(args["tool"].as< std::string >() == "info")
	{
		LOG_FATAL("Not available: info");

	}else if(args["tool"].as< std::string >() == "conversion")
	{
		LOG_FATAL("Not available: conversion");

	}
	else if (args["tool"].as< std::string >() == "trecvid-xtraction")
	{
		tool = new trecvid::TRECVidXtraction();
		
	}else if (args["tool"].as< std::string >() == "evaluation")
	{
		LOG_FATAL("Not available: evaluation");
	}
	else
	{
		LOG_FATAL("Tool " << args["tool"].as< int >() << " is not defined");
	}

	if(tool->init(args)) tool->run();

	delete tool;

	return EXIT_SUCCESS;
}

void showDescription()
{
	LOG_INFO("**** " << PROGNAME << " PROGRAM " << "(" << PROGDESCRIPTION << ")" << " - " << "VERSION 1.0 -" <<  " 2017 " << "****" );
}

void showHelp(const boost::program_options::options_description& desc)
{
	LOG_INFO(desc);
}

boost::program_options::variables_map processProgramOptions(const int argc, const char *const argv[])
{
	boost::program_options::options_description generic("Generic options");
	generic.add_options()
		("help,h", "Print options")
		("tool,t", boost::program_options::value<std::string>()->default_value("extraction"), "The tool to be used")
		("config", boost::program_options::value<std::string>()->default_value("default.ini"), "Configuration file for the settings to be used")
		("infile,i", boost::program_options::value<std::string>(), "Input file")
		("outfile,o", boost::program_options::value<std::string >(), "Output file")
		;

	//All possible options that will be allowed in config file for the different tools
	boost::program_options::options_description config("Configuration");
	config.add_options()
		("General.descriptor", boost::program_options::value<std::string>()->default_value("ffs"), 
			"which features should be extracted")

		("General.measurements", boost::program_options::value<std::string>(),
			"in which file should times are stored")

		("Cfg.ffs.maxFrames", boost::program_options::value<int>()->default_value(5), 
			"how many frames should be used")
		("Cfg.ffs.frameSelection", boost::program_options::value<std::string>()->default_value("FramesPerVideo"), 
			"how should the frames selected: frames-per-video, frames-per-second")
		("Cfg.ffs.resetTracking", boost::program_options::value<bool>()->default_value(true), 
			"should the tracking samplepoints newly initialized for each frame")

		("Cfg.ffs.initSeeds", boost::program_options::value<int>()->default_value(100), 
			"how many samplepoints should be clustered")
		("Cfg.ffs.initialCentroids", boost::program_options::value<int>()->default_value(10), 
			"how many clusters should maximal created")

		("Cfg.ffs.iterations", boost::program_options::value<int>()->default_value(5), 
			"how many iteration should be clustered")
		("Cfg.ffs.minClusterSize", boost::program_options::value<int>()->default_value(2), 
			"what is the minimum number of samples per cluster")
		("Cfg.ffs.minDistance", boost::program_options::value<float>()->default_value(0.01), 
			"what is the minimal joining distance of clusters")
		("Cfg.ffs.dropThreshold", boost::program_options::value<float>()->default_value(0.0), 
			"what is the number of samples per cluster to drop it")
		
		("Cfg.ffs.samplepointdir", boost::program_options::value<std::string>(), 
			"directory where the samplepoints are stored")
		("Cfg.ffs.distribution", boost::program_options::value<std::string>()->default_value("regular"), 
			"what is the distribution: random, regular")

		("Cfg.ffs.grayscaleBits", boost::program_options::value<int>()->default_value(5),
			"grayscaleBits")
		("Cfg.ffs.windowRadius", boost::program_options::value<int>()->default_value(4),
			"windowRadius")
		;

	boost::program_options::positional_options_description positional;
	positional.add("outfile", -1);

	boost::program_options::options_description cmdlineOptions;
	cmdlineOptions.add(generic).add(config);

	boost::program_options::options_description configfileOptions;
	configfileOptions.add(config);

	boost::program_options::options_description visible("Allowed options");
	visible.add(generic).add(config);

	if (argc < 2)
	{
		LOG_ERROR("For the execution of " << PROGNAME << " there are too few command line arguments");
		showHelp(visible);
		exit(EXIT_SUCCESS);
	}

	boost::program_options::variables_map args;
	boost::program_options::variables_map configfileArgs;

	try
	{
		store(boost::program_options::command_line_parser(argc, argv).options(cmdlineOptions).positional(positional).run(), args);
	}
	catch (boost::program_options::error const& e)
	{
		LOG_ERROR(e.what());
		showHelp(visible);
		exit(EXIT_FAILURE);
	}

	std::ifstream ifs(args["config"].as< std::string >());
	if (!ifs.is_open())
	{
		LOG_ERROR("Configuration file  " << args["config"].as< std::string >() << " cannot be found");
		showHelp(visible);
		exit(EXIT_SUCCESS);
	}

	try
	{
		store(parse_config_file(ifs, visible), args);
	}
	catch (boost::program_options::error const& e)
	{
		LOG_ERROR(e.what());
		showHelp(visible);
		exit(EXIT_FAILURE);
	}

	notify(args);


	if (args.count("help")) {
		showHelp(visible);
	}

	return args;
}
