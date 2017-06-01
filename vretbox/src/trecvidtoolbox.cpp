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

#include <cpluslogger.hpp>
#include <cplusutil.hpp>

#include <boost/program_options.hpp>

#include "toolbase.hpp"
#include "datainfo.hpp"
#include "dextraction.hpp"

using namespace boost;
using namespace trecvid;

static std::string prog = "TRECVID-Toolbox";

program_options::variables_map processProgramOptions(const int argc, const char *const argv[]);


int main(int argc, char const *argv[]) {

	program_options::variables_map args = processProgramOptions(argc, argv);

	ToolBase* tool = nullptr;

	if(args["tool"].as< std::string >() == "info")
	{
		tool = new DataInfo();

	}else if(args["tool"].as< std::string >() == "conversion")
	{
		LOG_FATAL("Not available: conversion");

	}
	else if (args["tool"].as< std::string >() == "extraction")
	{
		tool = new DeXtraction();
		
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

void showHelp(const program_options::options_description& desc)
{
	LOG_INFO(desc);
}

program_options::variables_map processProgramOptions(const int argc, const char *const argv[])
{
	program_options::options_description generic("Generic options");
	generic.add_options()
		("help,h", "Print options")
		("tool,t", program_options::value<std::string>()->default_value("extraction"), "The tool to be used")
		("config", program_options::value<std::string>()->default_value("default.ini"), "Configuration file for the settings to be used")
		("infile,i", program_options::value<std::string>(), "Input file")
		("outfile,o", program_options::value<std::vector<std::string> >(), "Output file")
		;

	//program_options::options_description hidden("Hidden options");
	//hidden.add_options()
	//	("outfile,o", program_options::value<std::vector<std::string> >(), "Output file")
	//	;


	//Declare all possible options that will be allowed in config file for the different tools
	program_options::options_description config("Configuration");
	config.add_options()
		("General.descriptor", program_options::value<std::string>(), "descriptor")
		("Settings.samplepoints", program_options::value<int>(), "samplepoints")
		("Settings.sampling", program_options::value<std::string>(), "sampling")
		("Settings.sample_files", program_options::value<std::string>(), "sample_files")
		("Settings.sample_rate", program_options::value<int>(), "sample_rate")
		("Settings.centroids", program_options::value<int>(), "centroids")
		("Settings.iterations", program_options::value<int>(), "centroids")
		("Settings.minimal_weight", program_options::value<int>(), "centroids")
		("Settings.minimal_distance", program_options::value<float>(), "centroids")
		;

	program_options::positional_options_description positional;
	positional.add("outfile", -1);

	program_options::options_description cmdlineOptions;
	cmdlineOptions.add(generic).add(config);// .add(hidden);

	program_options::options_description configfileOptions;
	configfileOptions.add(config);// .add(hidden);

	program_options::options_description visible("Allowed options");
	visible.add(generic).add(config);

	if (argc < 3)
	{
		LOG_ERROR("For the execution of " << prog << " there are too few command line arguments");
		showHelp(visible);
		exit(EXIT_SUCCESS);
	}

	program_options::variables_map args;
	program_options::variables_map configfileArgs;

	try
	{
		store(program_options::command_line_parser(argc, argv).options(cmdlineOptions).positional(positional).run(), args);
	}
	catch (program_options::error const& e)
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
		store(program_options::parse_config_file(ifs, config), args);
	}
	catch (program_options::error const& e)
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
