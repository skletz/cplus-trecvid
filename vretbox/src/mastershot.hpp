#ifndef _TRECVID_MASTERSHOT_HPP_
#define _TRECVID_MASTERSHOT_HPP_

#include <cplusutil.hpp>
#include <defuse.hpp>

namespace trecvid {

	class MasterShot : public defuse::VideoBase
	{
	public:

		int mVid;
		int mSid;
		int mStart;
		int mEnd;
		float mFps;
		int mWidth;
		int mHeight;

		/**
		* \brief
		*/
		MasterShot(File* _file);

		/**
		* \brief
		*/
		~MasterShot() {};
	};
}
#endif //_TRECVID_MASTERSHOT_HPP_