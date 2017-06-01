#ifndef _DEXTRACTION_HPP_
#define  _DEXTRACTION_HPP_

#include "toolbase.hpp"
#include "datastructures.hpp"
#include "../../../defuse/src/xtractor.hpp"
#include "../../../defuse/src/parameter.hpp"

namespace trecvid {

	/**
	* \brief
	*/
	class DeXtraction : public ToolBase
	{
		defuse::Xtractor* mXtractor;
		defuse::Parameter* mParamter;

		File* mVideo;

	public:

		DeXtraction();

		bool init(boost::program_options::variables_map _args) override;

		void run() override;

		defuse::Features* extractVideo(Directory* dir, File* videofile);

		defuse::Features* xtract(MasterShot* _videobase);
		
	};
}

#endif //_DEXTRACTION_HPP_