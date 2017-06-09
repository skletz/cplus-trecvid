/** DataInfo (Version 1.0) *****************************
* ******************************************************
*       _    _      ()_()
*      | |  | |    |(o o)
*   ___| | _| | ooO--`o'--Ooo
*  / __| |/ / |/ _ \ __|_  /
*  \__ \   <| |  __/ |_ / /
*  |___/_|\_\_|\___|\__/___|
*
* ******************************************************
* Purpose: 
* Input/Output:

* @author skletz
* @version 1.0 24/05/17
*
**/

#include "datainfo.hpp"


trecvid::DataInfo::DataInfo()
{
	mArgs = nullptr;
}

bool trecvid::DataInfo::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	return false;
}

void trecvid::DataInfo::run()
{
}
