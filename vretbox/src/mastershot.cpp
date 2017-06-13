#include "mastershot.hpp"

trecvid::MasterShot::MasterShot(File* _file)
	: VideoBase(_file), mVid(0), mSid(0), mStart(0), mEnd(0), mFps(0), mWidth(0), mHeight(0)
{
	mVideoFileName = _file->getFilename() + _file->getFileExtension();
}

trecvid::MasterShot::MasterShot(int _vid, int _sid, int _start, int _end, float _fps, int _width, int _height)
	: VideoBase(nullptr)
{
	mVid = _vid;
	mSid = _sid;
	mStart = _start;
	mEnd = _end;
	mFps = _fps;
	mWidth = _width;
	mHeight = _height;
}
