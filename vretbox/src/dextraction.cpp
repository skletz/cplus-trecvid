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

trecvid::DeXtraction::DeXtraction(): mXtractor(nullptr)
{
	mArgs = nullptr;
}

bool trecvid::DeXtraction::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	bool valid = true;

	mVideo = new File(mArgs["infile"].as< std::string >());
	mFeatures = new File(mArgs["outfile"].as< std::string >());
	mMeasurements = new File(mArgs["General.measurements"].as< std::string >());

	//Possible Settings
	int frameSelection, maxFrames;
	bool resetTracking;
	int initSeeds, initialCentroids, iterations, minClusterSize;
	float minDistance, dropThreshold;

	int grayscaleBits, windowRadius;

	defuse::SamplePoints::Distribution distribution;
	defuse::SamplePoints* samplepoints;
	std::string samplepointdir;


	if(mArgs["General.descriptor"].as< std::string >() == "ffs")
	{
		
		maxFrames = mArgs["Cfg.ffs.maxFrames"].as<int>();

		if (mArgs["Cfg.ffs.frameSelection"].as<std::string>() == "FramesPerVideo")
		{
			frameSelection = defuse::DYSIGXtractor::FrameSelection::FramesPerVideo;
		}
		else if(mArgs["Cfg.ffs.frameSelection"].as<std::string>() == "FramesPerSecond")
		{
			frameSelection = defuse::DYSIGXtractor::FrameSelection::FramesPerSecond;
		}
		else
		{
			frameSelection = defuse::DYSIGXtractor::FrameSelection::FramesPerVideo;
			LOG_FATAL("Cfg.ffs.frameSelection " << mArgs["Cfg.ffs.frameSelection"].as< std::string >() << " is not defined");
			valid = false;
		}

		resetTracking = mArgs["Cfg.ffs.resetTracking"].as<bool>();

		initSeeds = mArgs["Cfg.ffs.initSeeds"].as<int>();
		initialCentroids = mArgs["Cfg.ffs.initialCentroids"].as<int>();
		iterations = mArgs["Cfg.ffs.iterations"].as<int>();

		minClusterSize = mArgs["Cfg.ffs.minClusterSize"].as<int>();
		minDistance = mArgs["Cfg.ffs.minDistance"].as<float>();
		dropThreshold = mArgs["Cfg.ffs.dropThreshold"].as<float>();

		grayscaleBits = mArgs["Cfg.ffs.grayscaleBits"].as<int>();
		windowRadius = mArgs["Cfg.ffs.windowRadius"].as<int>();

	
		//Process sampling
		if (mArgs["Cfg.ffs.distribution"].as<std::string>() == "random")
		{
			distribution = defuse::SamplePoints::Distribution::RANDOM;
		}
		else if (mArgs["Cfg.ffs.distribution"].as<std::string>() == "regular")
		{
			distribution = defuse::SamplePoints::Distribution::REGULAR;
		}
		else
		{
			distribution = defuse::SamplePoints::Distribution::REGULAR;
			LOG_FATAL("Cfg.ffs.distribution " << mArgs["Cfg.ffs.distribution"].as< std::string >() << " is not defined");
			valid = false;
		}

		samplepointdir = mArgs["Cfg.ffs.samplepointdir"].as<std::string>();

		mXtractor = new defuse::DYSIGXtractor(maxFrames, initSeeds, initialCentroids, samplepointdir, distribution);
		mMeasurements->extendFileName(static_cast<defuse::DYSIGXtractor *>(mXtractor)->getFilename());
		mFeatures->addDirectoryToPath(static_cast<defuse::DYSIGXtractor *>(mXtractor)->getFilename());
		LOG_INFO("Settings: " << static_cast<defuse::DYSIGXtractor *>(mXtractor)->get());

	}else
	{
		LOG_FATAL("Descriptor " << mArgs["General.descriptor"].as< std::string >() << " is not defined");
		valid = false;
	}

	
	return valid;
}

void trecvid::DeXtraction::run()
{
	MasterShot* shot = new MasterShot(mVideo);
	defuse::Features* features = mXtractor->xtract(shot);

	std::ofstream of(mMeasurements->getFile(), std::ofstream::out | std::ofstream::app);
	of << shot->mVideoFileName << ", " << features->mExtractionTime << "\n";

	features->writeBinary(mFeatures->getFile());

	delete shot;
}


