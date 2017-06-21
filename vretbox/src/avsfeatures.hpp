#ifndef _AVSFEATURES_HPP_
#define _AVSFEATURES_HPP_

#include "defuse.hpp"

namespace trecvid {

	class AVSFeatures : public defuse::Features
	{
	public:

		int mQID;

		int mVID;

		int mSID;

		AVSFeatures() {};

		AVSFeatures(int _vid, int _sid, int _qid = 0);

		void setFilename(std::string _filename);

		void setExtractionTime(float _time);

		std::string getFilename() const;

		float getExtractionTime() const;

		void deserialize(std::string _file);
	};
}
#endif //_AVSFEATURES_HPP_
