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
	mQid = -1;
	mVid = _vid;
	mSid = _sid;
}
