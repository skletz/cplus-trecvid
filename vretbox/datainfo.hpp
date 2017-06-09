#ifndef _DATAINFO_HPP_
#define  _DATAINFO_HPP_

#include "toolbase.hpp"

namespace trecvid {

	/**
	 * \brief 
	 */
	class DataInfo : public ToolBase
	{
	public:
		
		DataInfo();

		bool init(boost::program_options::variables_map _args) override;

		void run() override;
		
	};
}

#endif //_DATA_INFO_HPP_