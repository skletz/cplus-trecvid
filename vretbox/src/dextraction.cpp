/** DataInfo (Version 1.0) *****************************
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

#include "dextraction.hpp"

trecvid::DeXtraction::DeXtraction(): mXtractor(nullptr), mParamter(nullptr)
{
	mArgs = nullptr;
}

bool trecvid::DeXtraction::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	bool valid = true;

	mVideo = new File(mArgs["infile"].as< std::string >());

	if(mArgs["descriptor"].as< std::string >() == "dfs")
	{
		mParamter = new defuse::DFS2Parameter();
		
		static_cast<defuse::DFS2Parameter *>(mParamter)->staticparamter.samplepoints = mArgs["Settings.samplepoints"].as<int>();

		int distribution = 0;
		if(mArgs["Settings.sampling"].as<std::string>() == "random")
		{
			distribution = 0;
		}else
		{
			LOG_FATAL("Settings.sampling " << mArgs["Settings.sampling"].as< std::string >() << " is not defined");
			valid = false;
		}

		static_cast<defuse::DFS2Parameter *>(mParamter)->staticparamter.distribution = distribution;
		static_cast<defuse::DFS2Parameter *>(mParamter)->staticparamter.initialCentroids = mArgs["Settings.centroids"].as<int>();
		static_cast<defuse::DFS2Parameter *>(mParamter)->staticparamter.samplepointsfile = mArgs["Settings.sample_files"].as<std::string>();
		static_cast<defuse::DFS2Parameter *>(mParamter)->staticparamter.iterations = mArgs["Settings.iterations"].as<int>();
		static_cast<defuse::DFS2Parameter *>(mParamter)->staticparamter.minimalWeight = mArgs["Settings.minimal_weight"].as<int>();
		static_cast<defuse::DFS2Parameter *>(mParamter)->staticparamter.minimalDistance = mArgs["Settings.minimal_distance"].as<float>();

		int frameselection = 0;
		if (mArgs["Settings.frame_selection"].as<std::string>() == "fnfpseg1")
		{
			frameselection = 0;
		}else
		{
			LOG_FATAL("Settings.frame_selection " << mArgs["Settings.frame_selection"].as< std::string >() << " is not defined");
			valid = false;
		}

		static_cast<defuse::DFS2Parameter *>(mParamter)->frameSelection = frameselection;
		static_cast<defuse::DFS2Parameter *>(mParamter)->frames = mArgs["Settings.sample_rate"].as<int>();
		
		mXtractor = new defuse::DFS2Xtractor(mParamter);


	}else
	{
		LOG_FATAL("Descriptor " << mArgs["descriptor"].as< std::string >() << " is not defined");
		valid = false;
	}


	return valid;
}

void trecvid::DeXtraction::run()
{

	MasterShot* videoclip = new MasterShot(file, videoID, startFrameNr, clazz);

	defuse::Features* feature = xtract(videoclip);

	File output(outputdir->getPath(), stream.str(), ".yml");
	output.addDirectoryToPath(descriptorshort);
	output.addDirectoryToPath(extractionsettings);

	serialize(features, output);

	//delete output;
	delete features;
}



defuse::Features* trecvid::DeXtraction::xtract(MasterShot* _videobase)
{
	size_t e1_start = double(cv::getTickCount());

	defuse::Features* features = mXtractor->xtract(_videobase);

	size_t e1_end = double(cv::getTickCount());

	double elapsed_secs = (e1_end - e1_start) / double(cv::getTickFrequency());
	features->mExtractionTime = elapsed_secs;
	//std::unique_lock<std::mutex> guard(f());
	//LOG_PERFMON(PTIME, "Computation-Time (secs): Extaction \t" << extractionsettings << "\t" << elapsed_secs);
	//guard.unlock();

	return features;
}
