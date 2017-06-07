#include "mastershot.hpp"

trecvid::MasterShot::MasterShot(File* _file): VideoBase(_file)
{
	mVideoFileName = _file->getFilename() + _file->getFileExtension();
}