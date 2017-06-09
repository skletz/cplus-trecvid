#ifndef _DEXTRACTION_HPP_
#define  _DEXTRACTION_HPP_

#include "toolbase.hpp"
#include <defuse.hpp>

namespace trecvid {

	/**
	* \brief
	*/
	class TRECVidXtraction : public vretbox::ToolBase
	{

		defuse::Xtractor* mXtractor;

		File* mVideo;

		File* mFeatures;

		File* mXtractionTimes;

	public:

		/**
		 * \brief 
		 */
		TRECVidXtraction();

		/**
		 * \brief 
		 * \param _args 
		 * \return 
		 */
		bool init(boost::program_options::variables_map _args) override;

		/**
		 * \brief 
		 */
		void run() override;

		/**
		 * \brief 
		 */
		~TRECVidXtraction() override;
	};
}

#endif //_DEXTRACTION_HPP_