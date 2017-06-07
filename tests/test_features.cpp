#include "stdafx.h"
#include "CppUnitTest.h"
#include <defuse.hpp>
#include <opencv2/opencv.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const std::string FEATUREFILETESTIMPL = "../../../../testdata/featurestestimpl.bin";
const std::string FEATUREFILE = "../../../../testdata/features.bin";
float FEATURES[10] = {	0.00603927851,	5.91500819,		0.00392041653,	0.0059509456, 0.00603927851,
						0.0098853266,	0.00140769446,	0.00837378025,	0.00130157009, 0.0098853266	};
const int COLS = 5;
const int ROWS = 2;

namespace features
{		
	TEST_CLASS(FeaturesIO)
	{
	public:

		TEST_METHOD(WriteFeaturesToBinary)
		{
			std::ofstream outfile(FEATUREFILE, std::ios::out);
			Assert::AreEqual(true, outfile.is_open(), L"Feature file could not be created", LINE_INFO());
			outfile.close();

			cv::Mat vectors = cv::Mat(ROWS, COLS, CV_32F, FEATURES);

			defuse::Features* features = new defuse::Features();
			features->mVectors = vectors;
			features->writeBinary(FEATUREFILE);
			outfile.close();
			delete features;
		}


		TEST_METHOD(ReadFeaturesFromBinary)
		{
			std::ifstream infile(FEATUREFILE, std::ios::in, std::ios::binary);
			Assert::AreEqual(true, infile.good(), L"Feature file could not be opened", LINE_INFO());
			infile.close();

			defuse::Features* features = new defuse::Features();
			
			features->readBinary(FEATUREFILE);
			
			infile.close();

			cv::Mat orig = cv::Mat(ROWS, COLS, CV_32F, FEATURES);
			cv::Mat temp;
			cv::bitwise_xor(features->mVectors, orig, temp);
			bool areEqual = !(cv::countNonZero(temp));

			Assert::AreEqual(areEqual, true, L"Features could not be recreated", LINE_INFO());

			delete features;
		}

		//TEST_METHOD(WriteBinaryTestImpl)
		//{
		//	std::ofstream outfile(FEATUREFILETESTIMPL, std::ios::out);
		//	Assert::AreEqual(true, outfile.is_open(), L"Feature file could not be created", LINE_INFO());
		//	outfile.close();

		//	outfile.open(FEATUREFILETESTIMPL, std::ios::out, std::ios::binary);
		//	
		//	cv::Mat vectors = cv::Mat(ROWS, COLS, CV_32F, FEATURES);

		//	boost::archive::binary_oarchive oa(outfile);

		//	int cols, rows, type;
		//	cols = vectors.cols; rows = vectors.rows; type = vectors.type();
		//	bool continuous = vectors.isContinuous();

		//	oa & cols & rows & type & continuous;

		//	if (continuous) {
		//		const unsigned int data_size = rows * cols * vectors.elemSize();
		//		oa & boost::serialization::make_array(vectors.ptr(), data_size);
		//	}
		//	else {
		//		const unsigned int row_size = cols*vectors.elemSize();
		//		for (int i = 0; i < rows; i++) {
		//			oa & boost::serialization::make_array(vectors.ptr(i), row_size);
		//		}
		//	}			
		//}

		//TEST_METHOD(ReadBinaryTestImpl)
		//{
		//	defuse::Features* features = new defuse::Features();

		//	std::ifstream infile(FEATUREFILETESTIMPL, std::ios::in, std::ios::binary);
		//	Assert::AreEqual(true, infile.is_open(), L"Feature file could not be opened", LINE_INFO());

		//	boost::archive::binary_iarchive ia(infile);

		//	cv::Mat vectors;
		//	int cols, rows, type;
		//	bool continuous;

		//	ia & cols & rows & type & continuous;

		//	vectors.create(rows, cols, type);

		//	const unsigned int row_size = cols*vectors.elemSize();
		//	for (int i = 0; i < rows; i++) {
		//		ia & boost::serialization::make_array(vectors.ptr(i), row_size);
		//	}

		//	infile.close();
		//	delete features;
		//	cv::Mat orig = cv::Mat(ROWS, COLS, CV_32F, FEATURES);
		//	cv::Mat temp;
		//	cv::bitwise_xor(vectors, orig, temp);
		//	bool areEqual = !(cv::countNonZero(temp));

		//	Assert::AreEqual(areEqual, true, L"Features could not be recreated", LINE_INFO());
		//}

	};
}