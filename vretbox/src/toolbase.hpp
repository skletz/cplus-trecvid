#ifndef _TOOLBASE_HPP_
#define  _TOOLBASE_HPP_

#include <boost/program_options.hpp>
#include <cpluslogger.hpp>
#include <cplusutil.hpp>
#include <defuse.hpp>

using namespace boost;

namespace trecvid {

	/**
	* \brief
	*/
	class ToolBase
	{
		
	protected:
		boost::program_options::variables_map mArgs;
	
	public:
		virtual bool init(boost::program_options::variables_map _args) = 0;
		virtual void run() = 0;
		virtual ~ToolBase() = 0 { };

	};
}

#endif //_TOOLBASE_HPP_