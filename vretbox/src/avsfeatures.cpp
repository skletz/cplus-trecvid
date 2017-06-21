#include "avsfeatures.hpp"

trecvid::AVSFeatures::AVSFeatures(int _vid, int _sid, int _qid)
{
	mQID = _qid;
	mVID = _vid;
	mSID = _sid;
}

void trecvid::AVSFeatures::setFilename(std::string _filename)
{
	mVideoFileName = _filename;
}

void trecvid::AVSFeatures::setExtractionTime(float _time)
{
	mExtractionTime = _time;
}

std::string trecvid::AVSFeatures::getFilename() const
{
	return mVideoFileName;
}

float trecvid::AVSFeatures::getExtractionTime() const
{
	return mExtractionTime;
}

void trecvid::AVSFeatures::deserialize(std::string _file)
{
	File* file = new File(_file);

	readBinary(_file);

	mVideoFileName = _file;
	mExtractionTime = 0;

	std::vector<std::string> strings = cplusutil::String::split(file->getFilename().c_str(), '_');

	mVID = cplusutil::String::extractIntFromString(strings.at(0));
	mSID = cplusutil::String::extractIntFromString(strings.at(1));
	mQID = -1;
}