#include "trecvidupdate.hpp"
#include "avsquery.hpp"
#include "mastershot.hpp"

trecvid::TRECVidUpdate::TRECVidUpdate()
	: mGroundTruth(nullptr), mMasterShots(nullptr), mUpdatedGroundTruth(nullptr)
{
	mArgs = nullptr;
}

bool trecvid::TRECVidUpdate::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	bool argsValid = true;

	mGroundTruth = new File(mArgs["infile"].as<std::string>());
	mUpdatedGroundTruth = new File(mArgs["outfile"].as<std::string>());
	mMasterShots = new File(mArgs["Cfg.gtupdate.mastershots"].as<std::string>());

	std::ifstream infile(mGroundTruth->getFile(), std::ios::in);
	if(!infile.is_open()){
		LOG_ERROR("File cannot be opened");
		argsValid = false;
	}
	infile.close();

	infile.open(mMasterShots->getFile(), std::ios::in);
	if(!infile.is_open()){
		LOG_ERROR("File cannot be opened");
		argsValid = false;
	}
	infile.close();


	return argsValid;
}


void trecvid::TRECVidUpdate::run()
{
	int qid, vid, sid;
	std::list<trecvid::AVSQuery*> queries;
	std::list<trecvid::MasterShot*> shots;

	//faster as ifstream
	FILE *file = fopen(mGroundTruth->getFile().c_str(), "r");
	while(fscanf(file, "%d,%d,%d", &qid, &vid, &sid) == 3)
	{
		AVSQuery *query = new AVSQuery(qid, vid, sid);
		queries.push_back(query);
	}
	fclose(file);

	char srvid[256],filename[256], xtractorid[256];
	int startframe, endframe, widht, height;
	float fps, length;
	int filter;

	file = fopen(mMasterShots->getFile().c_str(), "r");

	//fscanf awaits a whitespace terminated string when it tries to read %s
	while (fscanf(file, "%[^,],%[^,],%[^,],%d,%d,%d,%d,%f,%d,%d,%f,%d", srvid, filename, xtractorid, &vid, &sid, &startframe, &endframe, &fps, &widht, &height, &length, &filter) == 12)
	{
		//remove all shots with less than x, which is indicated by filter=true
		if(filter == true)
		{
			trecvid::MasterShot *shot = new trecvid::MasterShot(vid, sid, startframe, endframe, fps, widht, height);
			shots.push_back(shot);
		}

	}
	fclose(file);

	LOG_INFO("Found " << queries.size() << " queries");
	LOG_INFO("Found " << shots.size() << " shots");

	updateQueryList(&shots,&queries);


	writeQueryList(&queries, mUpdatedGroundTruth->getFile());

	std::list<trecvid::AVSQuery*>::iterator itqeueries;
	for(itqeueries = queries.begin(); itqeueries != queries.end(); ++itqeueries)
	{
		AVSQuery* entry = *itqeueries;
		delete entry;
	}
	queries.clear();

	std::list<trecvid::MasterShot*>::iterator itshots;
	for (itshots = shots.begin(); itshots != shots.end(); ++itshots)
	{
		MasterShot* entry = *itshots;
		delete entry;
	}
	shots.clear();

}

void trecvid::TRECVidUpdate::updateQueryList(
	std::list<trecvid::MasterShot*>* _shots, std::list<trecvid::AVSQuery*>* _queries)
{
	std::list<trecvid::AVSQuery*> _updatedQueries;
	_queries->sort([](const AVSQuery* f, const AVSQuery* s) {return f->mVid < s->mVid; });
	_shots->sort([](const MasterShot* f, const MasterShot* s) {return f->mVid < s->mVid; });

	std::list<trecvid::MasterShot*>::iterator itshots;
	int progress = 1;
	int lenght = _shots->size();
	int counter = 0;
	for (itshots = _shots->begin(); itshots != _shots->end(); ++itshots)
	{
		cplusutil::Terminal::showProgress("Update QueryList: ", progress, lenght);
		progress++;

		AVSQuery* findelem = new AVSQuery((*itshots)->mVid, (*itshots)->mSid);

		std::list<AVSQuery*>::iterator matching = find_if(_queries->begin(), _queries->end(), [&findelem](const AVSQuery* q)
		{
			return (findelem->mVid == q->mVid) && (findelem->mSid == q->mSid);
		});

		if(matching != _queries->end())
		{
			auto index = std::distance(_queries->begin(), matching);
			counter++;
			_queries->erase(matching);
		}
		
	}

	LOG_INFO("found " << counter << " shots in query list");
}

bool trecvid::TRECVidUpdate::writeQueryList(std::list<trecvid::AVSQuery*>* _queries, std::string _file)
{
	bool successful = false;
	FILE *file = fopen(_file.c_str(), "w");

	if(file != nullptr)
	{
		std::list<trecvid::AVSQuery*>::iterator itqeueries;
		for (itqeueries = _queries->begin(); itqeueries != _queries->end(); ++itqeueries)
		{
			fprintf(file, "%d,%d,%d\n", (*itqeueries)->mQid, (*itqeueries)->mVid, (*itqeueries)->mSid);
		}

		fclose(file);
		successful = true;
	}
	
	return successful;
}


trecvid::TRECVidUpdate::~TRECVidUpdate()
{
	delete mGroundTruth;
	delete mUpdatedGroundTruth;
	delete mMasterShots;
}
