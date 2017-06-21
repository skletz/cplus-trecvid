#include "avsquery.hpp"

trecvid::AVSQuery::AVSQuery(int _qid, int _vid, int _sid)
	: VideoBase(nullptr)
{
	mQid = _qid;
	mVid = _vid;
	mSid = _sid;
}

trecvid::AVSQuery::AVSQuery(int _vid, int _sid)
	: VideoBase(nullptr)
{
	mQid = 0;
	mVid = _vid;
	mSid = _sid;
}

trecvid::AVSQuery::AVSQuery(int _vid, int _sid, defuse::Features* _features)
	: VideoBase(nullptr)
{
	mVid = _vid;
	mSid = _sid;
	mFeatures = _features;
}
