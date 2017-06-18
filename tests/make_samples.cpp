#include "stdafx.h"
#include "CppUnitTest.h"
#include <defuse.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string TRECVIDPATH = "../../../../testdata/unit-tests/trecvid-videos/";
std::string ENDOPATH = "../../../../testdata/unit-tests/trecvid-videos/";

const std::string SAMPLEPOINTDIR = "../../../../testdata/unit-tests/samplepoints/";
const int NRSAMPLEPOINTS = 8000;
const int NRCLUSTERS = 40;
const int NRFRAMES = 5;

const defuse::SamplePoints::Distribution DISTRIBUTION = defuse::SamplePoints::Distribution::RANDOM;
namespace vretbox
{
	TEST_CLASS(Xtractor)
	{
	public:

		TEST_METHOD(DrawDynamicSignature)
		{
			defuse::DYSIGXtractor* xtractor = new defuse::DYSIGXtractor(NRFRAMES, NRSAMPLEPOINTS, NRCLUSTERS, SAMPLEPOINTDIR, DISTRIBUTION);

			std::vector<std::string> trecvid =	cplusutil::FileIO::getFileListFromDirectory(TRECVIDPATH, ".mp4");
			std::vector<std::string> endo = cplusutil::FileIO::getFileListFromDirectory(ENDOPATH, ".mp4");

			std::vector<std::string> testdata;
			testdata.insert(testdata.end(), trecvid.begin(), trecvid.end());
			testdata.insert(testdata.end(), endo.begin(), endo.end());

			for(int iFile = 0; iFile < testdata.size(); iFile++)
			{
				File* videofile = new File(testdata.at(iFile));

				defuse::VideoBase* video = new defuse::VideoBase(videofile);

				defuse::Features* features = xtractor->xtract(video);

				//xtractor->
			}

		}


	};
}