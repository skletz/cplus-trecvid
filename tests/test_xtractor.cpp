#include "stdafx.h"
#include "CppUnitTest.h"
#include <defuse.hpp>
//#include <vretbox.hpp>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//const std::string CMDLINE = "--t=\"extraction\" --config=\".. / .. / vretbox / default.ini\" --i=\"../../../../shots/39104_70_1930-1932_30.08_320x240.mp4\" ../../../../features/39104_70_1930-1932_30.08_320x240.bin";
const std::string SAMPLEPOINTDIR = "../../../../testdata/samplepoints/";
const int NRSAMPLEPOINTS = 500;
const int NRCLUSTERS = 50;
const int NRFRAMES = 5;

const defuse::SamplePoints::Distribution DISTRIBUTION = defuse::SamplePoints::Distribution::RANDOM;

namespace vretbox
{
	TEST_CLASS(Xtractor)
	{
	public:

		TEST_METHOD(DySigDefaultConstructor)
		{
			defuse::Xtractor* xtractor = new defuse::DYSIGXtractor();
		}

		TEST_METHOD(DySigConstructor)
		{
			defuse::Xtractor* xtractor = new defuse::DYSIGXtractor(NRFRAMES, NRSAMPLEPOINTS, NRCLUSTERS, SAMPLEPOINTDIR, DISTRIBUTION);

			Assert::AreEqual(static_cast<defuse::DYSIGXtractor *>(xtractor)->mInitSeeds, NRSAMPLEPOINTS, L"Wrong number of samplepoints", LINE_INFO());
			Assert::AreEqual(static_cast<defuse::DYSIGXtractor *>(xtractor)->mMaxClusters, NRCLUSTERS, L"Wrong number of clusters", LINE_INFO());
			Assert::AreEqual(static_cast<defuse::DYSIGXtractor *>(xtractor)->mMaxFrames, NRFRAMES, L"Wrong number of frames", LINE_INFO());
			Assert::IsTrue(static_cast<defuse::DYSIGXtractor *>(xtractor)->mDistribution == DISTRIBUTION, L"Wrong distribution", LINE_INFO());

			defuse::SamplePoints* samples = static_cast<defuse::DYSIGXtractor *>(xtractor)->mSamplepoints;
			Assert::AreEqual(samples->getSampleCnt(), NRSAMPLEPOINTS, L"Wrong samplepoints", LINE_INFO());
			Assert::IsTrue(samples->getDistribution() == DISTRIBUTION, L"Wrong samplepoints", LINE_INFO());
		}

	};
}