#ifndef _TRECVID_AVSQUERY_HPP_
#define _TRECVID_AVSQUERY_HPP_

#include <defuse.hpp>

namespace trecvid {

	class AVSQuery : public defuse::VideoBase
	{
		
	public:

		int mQid;

		int mVid;

		int mSid;

		defuse::Features* mFeatures;

		/**
		* \brief
		*/
		AVSQuery(int _qid, int _vid, int _sid);

		AVSQuery(int _vid, int _sid);

		AVSQuery(int _vid, int _sid, defuse::Features* _features);

		/**
		* \brief
		*/
		~AVSQuery() {};
	};
}
#endif //_TRECVID_AVSQUERY_HPP_