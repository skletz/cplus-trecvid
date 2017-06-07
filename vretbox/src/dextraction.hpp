#ifndef _DEXTRACTION_HPP_
#define  _DEXTRACTION_HPP_

#include "toolbase.hpp"
#include <defuse.hpp>
#include "mastershot.hpp"

namespace trecvid {

	/**
	* \brief
	*/
	class DeXtraction : public ToolBase
	{
		defuse::Xtractor* mXtractor;

		File* mVideo;
		File* mFeatures;
		File* mMeasurements;

	public:

		DeXtraction();

		bool init(boost::program_options::variables_map _args) override;

		void run() override;
		
	};
}

#endif //_DEXTRACTION_HPP_