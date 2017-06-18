/** TRECVidXtraction (Version 1.0) **************************
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
* @version 1.0 09/06/17
*
**/

#include "trecvidxtraction.hpp"
#include "mastershot.hpp"


trecvid::TRECVidXtraction::TRECVidXtraction()
	: mXtractor(nullptr), mVideo(nullptr), mFeatures(nullptr), mXtractionTimes(nullptr)
{
	mArgs = nullptr;
}

bool trecvid::TRECVidXtraction::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	bool areArgsValid = true;

	mVideo = new File(mArgs["infile"].as< std::string >());
	mFeatures = new File(mArgs["outfile"].as< std::string >());
	mXtractionTimes = new File(mArgs["General.measurements"].as< std::string >());

	defuse::DYSIGXtractor::FrameSelection frameSelection;
	int maxFrames;
	int initSeeds;
	int initialCentroids;
	int iterations;
	int minClusterSize;
	int grayscaleBits;
	int windowRadius;
	float minDistance;
	float dropThreshold;
	bool resetTracking;

	defuse::SamplePoints::Distribution distribution;
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
			areArgsValid = false;
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
			areArgsValid = false;
		}

		samplepointdir = mArgs["Cfg.ffs.samplepointdir"].as<std::string>();

		mXtractor = new defuse::DYSIGXtractor(maxFrames, initSeeds, initialCentroids, samplepointdir, distribution);

		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mFrameSelection = frameSelection;
		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mIterations = iterations;
		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mMinimalClusterSize = minClusterSize;
		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mGrayscaleBits = grayscaleBits;
		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mWindowRadius = windowRadius;
		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mMinimalDistance = minDistance;
		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mClusterDropThreshold = dropThreshold;
		static_cast<defuse::DYSIGXtractor *>(mXtractor)->mResetTracking = resetTracking;

		mXtractionTimes->extendFileName(static_cast<defuse::DYSIGXtractor *>(mXtractor)->getXtractorID());
		mFeatures->addDirectoryToPath(static_cast<defuse::DYSIGXtractor *>(mXtractor)->getXtractorID());

		LOG_INFO("**** " << "TRECVidXtraction Tool " << "**** ");
		LOG_INFO("**** " << "Settings");
		LOG_INFO("**** " << static_cast<defuse::DYSIGXtractor *>(mXtractor)->toString());
		LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

	}else
	{
		LOG_FATAL("Descriptor " << mArgs["General.descriptor"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	return areArgsValid;
}

void trecvid::TRECVidXtraction::run()
{
	MasterShot* shot = new MasterShot(mVideo);

	defuse::Features* features = mXtractor->xtract(shot);
	LOG_INFO("Write Binary");
	//Process extraction times
	std::ofstream of(mXtractionTimes->getFile(), std::ofstream::out | std::ofstream::app);
	of << shot->mVideoFileName << ", " << features->mExtractionTime << "\n";

	//Save features
	features->writeBinary(mFeatures->getFile());
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

	delete shot;
}

trecvid::TRECVidXtraction::~TRECVidXtraction()
{
	delete mXtractor;
	delete mVideo;
	delete mFeatures;
	delete mXtractionTimes;
}


