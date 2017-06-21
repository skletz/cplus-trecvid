#include "stdafx.h"
#include "CppUnitTest.h"
#include <defuse.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//const std::string CMDLINE = "--t=\"extraction\" --config=\".. / .. / vretbox / default.ini\" --i=\"../../../../shots/39104_70_1930-1932_30.08_320x240.mp4\" ../../../../features/39104_70_1930-1932_30.08_320x240.bin";

std::string FEATUREOUTPUT = "../../../../testdata/unit-tests";

std::string VIDEO1FRAME = "../../../../testdata/unit-tests/trecvid-videos/39238_118_820-821_25.00_320x240.mp4";
std::string VIDEO2FRAME = "../../../../testdata/unit-tests/trecvid-videos/39104_70_1930-1932_30.08_320x240.mp4";
std::string VIDEO17FRAME = "../../../../testdata/unit-tests/trecvid-videos/39104_59_1875-1892_30.08_320x240.mp4";
std::string VIDEO331FRAME = "../../../../testdata/unit-tests/trecvid-videos/36295_10_897-1228_30.00_320x240.mp4";
std::string VIDEO16978FRAME = "../../../../testdata/unit-tests/trecvid-videos/39869_1_0-16978_29.97_320x240.mp4";

std::string TRECVIDFEATURES = FEATUREOUTPUT + "/trecvid-videos/DySig_5_FramesPerVideo_true_8000_40_5_2_0.01_0_random_5_4";

std::string ENDO1 = "../../../../testdata/unit-tests/endo-videos/1.mp4_5660.mp4";
std::string ENDO2 = "../../../../testdata/unit-tests/endo-videos/119.mp4_10925.mp4";
std::string ENDO3 = "../../../../testdata/unit-tests/endo-videos/6089001.mp4_37525-0001.mp4";
std::string ENDO4 = "../../../../testdata/unit-tests/endo-videos/6584001.mp4_7500-0001.mp4";

std::string ENDOFEATURES = FEATUREOUTPUT + "/endo-videos/DFS2_SFS1_8000_random_40_5_2_0.01_0_0_5";

//std::string VIDEOS[5] = { VIDEO1FRAME, VIDEO2FRAME, VIDEO17FRAME, VIDEO331FRAME, VIDEO16978FRAME };
std::string TRECVID_VIDEOS[4] = { VIDEO2FRAME, VIDEO17FRAME, VIDEO331FRAME, VIDEO16978FRAME };
std::string ENDO_VIDEOS[4] = { ENDO1, ENDO2, ENDO3, ENDO4 };

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
			Assert::AreEqual(samples->getSampleCnt(), NRSAMPLEPOINTS, L"Wrong sample count of samplepoints", LINE_INFO());
			Assert::IsTrue(samples->getDistribution() == DISTRIBUTION, L"Wrong distribution of samplepoints", LINE_INFO());
		}

		TEST_METHOD(TRECVIDVideosWithFixFramesPerVideo)
		{
			defuse::DYSIGXtractor* xtractor = new defuse::DYSIGXtractor(NRFRAMES, NRSAMPLEPOINTS, NRCLUSTERS, SAMPLEPOINTDIR, DISTRIBUTION);

			for(int iVideo=0; iVideo < sizeof(TRECVID_VIDEOS)/sizeof(TRECVID_VIDEOS[0]); iVideo++)
			{
				File* videofile = new File(TRECVID_VIDEOS[iVideo]);

				defuse::VideoBase* video = new defuse::VideoBase(videofile);

				defuse::Features* features = xtractor->xtract(video);

				Assert::IsTrue(features != NULL, L"DYSIGXtractor returns no features", LINE_INFO());

				defuse::FeatureSignatures* featuresignatures = static_cast<defuse::FeatureSignatures*>(features);

				Assert::IsTrue(featuresignatures != NULL, L"Wrong type of features is returned", LINE_INFO());

				std::string file = TRECVIDFEATURES + "/" + videofile->getFilename() + ".yml";

				File* fileToCompare = new File(file);

				defuse::Features* compareFeatures = nullptr;

				cv::FileStorage fileStorage(fileToCompare->getFile(), cv::FileStorage::READ);

				cv::FileNode node = fileStorage["Data"];
				cv::FileNodeIterator it = node.begin(), it_end = node.end();

				for (; it != it_end; ++it)
				{
					compareFeatures = new defuse::FeatureSignatures();
					(*compareFeatures).read(*it);
				}

				fileStorage.release();

				cv::Mat temp;
				cv::bitwise_xor(compareFeatures->mVectors, featuresignatures->mVectors, temp);
				bool areEqual = !(cv::countNonZero(temp));

				Assert::AreEqual(areEqual, true, L"Features could not be recreated", LINE_INFO());

				//File* output = new File(videofile->getFile());
				//output->setPath(FEATUREOUTPUT);
				//output->addDirectoryToPath(xtractor->getXtractorID());
				//output->setFileExtension(".yml");

				//cv::FileStorage filestorage(output->getFile(), cv::FileStorage::WRITE);

				//filestorage << "Data" << "[";
				//features->write(filestorage);
				//filestorage << "]";
				//filestorage.release();
			}


		}

		TEST_METHOD(CreateTRECVIDFeaturesWithFixFramesBinary)
		{
			defuse::DYSIGXtractor* xtractor = new defuse::DYSIGXtractor(NRFRAMES, NRSAMPLEPOINTS, NRCLUSTERS, SAMPLEPOINTDIR, DISTRIBUTION);

			for (int iVideo = 0; iVideo < sizeof(TRECVID_VIDEOS) / sizeof(TRECVID_VIDEOS[0]); iVideo++)
			{
				File* videofile = new File(TRECVID_VIDEOS[iVideo]);

				defuse::VideoBase* video = new defuse::VideoBase(videofile);

				defuse::Features* features = xtractor->xtract(video);

				Assert::IsTrue(features != NULL, L"DYSIGXtractor returns no features", LINE_INFO());

				defuse::FeatureSignatures* featuresignatures = static_cast<defuse::FeatureSignatures*>(features);

				Assert::IsTrue(featuresignatures != NULL, L"Wrong type of features is returned", LINE_INFO());

				std::string file = TRECVIDFEATURES + "/" + videofile->getFilename() + ".bin";

				File* output = new File(file);

				features->writeBinary(output->getFile());

			}


		}

		TEST_METHOD(ENDOVideosWithFixFramesPerVideo)
		{
			defuse::DYSIGXtractor* xtractor = new defuse::DYSIGXtractor(NRFRAMES, NRSAMPLEPOINTS, NRCLUSTERS, SAMPLEPOINTDIR, DISTRIBUTION);

			for (int iVideo = 0; iVideo < sizeof(ENDO_VIDEOS) / sizeof(ENDO_VIDEOS[0]); iVideo++)
			{
				File* videofile = new File(ENDO_VIDEOS[iVideo]);

				defuse::VideoBase* video = new defuse::VideoBase(videofile);

				defuse::Features* features = xtractor->xtract(video);

				Assert::IsTrue(features != NULL, L"DYSIGXtractor returns no features", LINE_INFO());

				defuse::FeatureSignatures* featuresignatures = static_cast<defuse::FeatureSignatures*>(features);

				Assert::IsTrue(featuresignatures != NULL, L"Wrong type of features is returned", LINE_INFO());

				std::string file = ENDOFEATURES + "/" + videofile->getFilename() + ".yml";

				File* fileToCompare = new File(file);

				defuse::Features* compareFeatures = nullptr;

				cv::FileStorage fileStorage(fileToCompare->getFile(), cv::FileStorage::READ);

				cv::FileNode node = fileStorage["Data"];
				cv::FileNodeIterator it = node.begin(), it_end = node.end();

				for (; it != it_end; ++it)
				{
					compareFeatures = new defuse::FeatureSignatures();
					(*compareFeatures).read(*it);
				}

				fileStorage.release();

				cv::Mat temp;
				cv::bitwise_xor(compareFeatures->mVectors, featuresignatures->mVectors, temp);
				bool areEqual = !(cv::countNonZero(temp));

				Assert::AreEqual(areEqual, true, L"Features could not be recreated", LINE_INFO());

			}


		}
	};
}