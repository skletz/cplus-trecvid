/** TRECVidXtraction (Version 1.0) **************************
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
* @version 1.0 15/06/17
*
**/

#include "trecvidvaluation.hpp"
#include "avsquery.hpp"
#include "avsfeatures.hpp"
#include <unordered_map>
#include <boost/thread/thread.hpp>


/**
 * \brief Hash function for pairs
 * \tparam T struct
 * \param seed 
 * \param v value
 */
template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
	/**
	 * \brief Operator for unordered maps to use pairs as keys
	 * \tparam S first struct
	 * \tparam T second struct
	 */
	template<typename S, typename T> struct hash<pair<S, T>>
	{
		inline size_t operator()(const pair<S, T> & v) const
		{
			size_t seed = 0;
			::hash_combine(seed, v.first);
			::hash_combine(seed, v.second);
			return seed;
		}
	};
}

trecvid::TRECVidValuation::TRECVidValuation()
	: mDistance(nullptr), mXtractor(nullptr), mFeatures(nullptr), mGroundTruth(nullptr), mMAPValues(nullptr)
{
	mArgs = nullptr;
}

bool trecvid::TRECVidValuation::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	bool areArgsValid = true;

	mGroundTruth = new File(mArgs["infile"].as< std::string >());
	mFeatures = new Directory(mArgs["indir"].as< std::string >());
	mMAPValues = new File(mArgs["outfile"].as< std::string >());

	defuse::Parameter* paramter = nullptr;
	int grounddistance;
	int matchingstrategy;
	int direction;
	int costfunction;
	float lambda;


	if (mArgs["General.distance"].as< std::string >() == "smd")
	{
		paramter = new defuse::SMDParamter();

		if (mArgs["Cfg.smd.grounddistance"].as<std::string>() == "L1")
		{
			grounddistance = 1;
		}
		else if (mArgs["Cfg.smd.grounddistance"].as<std::string>() == "L2")
		{
			grounddistance = 2;
		}else
		{
			grounddistance = 2;
			LOG_FATAL("Cfg.smd.grounddistance " << mArgs["Cfg.smd.grounddistance"].as< std::string >() << " is not defined");
			areArgsValid = false;
		}

		if (mArgs["Cfg.smd.matchingstrategy"].as<std::string>() == "nearest-neighbor")
		{
			matchingstrategy = 0;
		}
		else
		{
			matchingstrategy = 0;
			LOG_FATAL("Cfg.smd.matchingstrategy " << mArgs["Cfg.smd.matchingstrategy"].as< std::string >() << " is not defined");
			areArgsValid = false;
		}

		if (mArgs["Cfg.smd.direction"].as<std::string>() == "bidirectional")
		{
			direction = 0;
		}
		else if (mArgs["Cfg.smd.direction"].as<std::string>() == "asymmetric-query")
		{
			direction = 1;
		}
		else if (mArgs["Cfg.smd.direction"].as<std::string>() == "asymmetric-database")
		{
			direction = 2;
		}
		else
		{
			direction = 1;
			LOG_FATAL("Cfg.smd.direction " << mArgs["Cfg.smd.direction"].as< std::string >() << " is not defined");
			areArgsValid = false;
		}

		if (mArgs["Cfg.smd.costfunction"].as<std::string>() == "weighted-distance")
		{
			costfunction = 0;
		}
		else
		{
			costfunction = 0;
			LOG_FATAL("Cfg.smd.costfunction " << mArgs["Cfg.smd.costfunction"].as< std::string >() << " is not defined");
			areArgsValid = false;
		}

		lambda = mArgs["Cfg.smd.lambda"].as<float>();

		static_cast<defuse::SMDParamter *>(paramter)->grounddistance.distance = grounddistance;
		static_cast<defuse::SMDParamter *>(paramter)->matching = matchingstrategy;
		static_cast<defuse::SMDParamter *>(paramter)->direction = direction;
		static_cast<defuse::SMDParamter *>(paramter)->cost = costfunction;
		static_cast<defuse::SMDParamter *>(paramter)->lambda = lambda;

	}else 
	{
		LOG_FATAL("Distance " << mArgs["General.distance"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	time_t now = time(nullptr);
	static char name[20]; //is also the model name
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", localtime(&now));

	std::stringstream logfile;
	logfile << paramter->getFilename() << "_" << mFeatures->mDirName << "_Evaluation" << ".log";
	Directory workingdir(".");
	File log(workingdir.getPath(), logfile.str());
	log.addDirectoryToPath("logs");

	cpluslogger::Logger::get()->logfile(log.getFile());
	cpluslogger::Logger::get()->filelogging(true);
	cpluslogger::Logger::get()->perfmonitoring(true);

	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
	LOG_INFO("**** " << "Timestamp " << name);
	LOG_INFO("**** " << "TRECVidValuation Tool " << "**** ");
	LOG_INFO("**** " << "Settings");
	LOG_INFO("**** " << paramter->get());
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");


	mDistance = new defuse::SMD(paramter, defuse::DYSIGXtractor::as_integer(defuse::DYSIGXtractor::IDX::WEIGHT), -1);

	return areArgsValid;
}

void trecvid::TRECVidValuation::run()
{
	int qid, vid, sid;

	//Key is videoid,shotid; value is qid
	std::unordered_map<std::pair<int,int>, int> queryindex;

	//fetch all queries from ground truth
	FILE *gtfile = fopen(mGroundTruth->getFile().c_str(), "r");
	while (fscanf(gtfile, "%d,%d,%d", &qid, &vid, &sid) == 3)
	{
		AVSQuery *query = new AVSQuery(qid, vid, sid);

		std::pair<int, int> key;
		key.first = vid;
		key.second = sid;
		queryindex[key] = qid;
	}
	fclose(gtfile);


	//fetch all features
	std::vector<std::string> files = cplusutil::FileIO::getFileListFromDirectory(mFeatures->getPath());

	std::unordered_map<int, std::vector<AVSFeatures*>> queries;

	int fileSize = files.size();
	for (int iFile = 0; iFile < fileSize; iFile++)
	{
		showProgress("Load features", iFile, fileSize);

		std::string file = files.at(iFile);

		AVSFeatures* features = new AVSFeatures();
		features->deserialize(file);

		if (features->mVectors.empty())
		{
			LOG_ERROR("Fatal Error: Feature file " << file << "cannot be deserialized.");
			exit(EXIT_FAILURE);
		}

		//add qid if available, otherwise zero id
		std::pair<int, int> queryid = std::make_pair(features->mVID, features->mSID);
		features->mQID = queryindex[queryid];
		queries[features->mQID].push_back(features);
		mModel.push_back(features);
	}

	float avgMeanAveragePrecision = 0.0;
	float avgMeanAverageComputationTime = 0.0;

	int querySize = queries.size();
	int queryCounter = 0;
	std::vector<std::pair<int, float>> mapvalues;
	//evaluate mean average precision for all elements in each query group
	for (auto iQueryGroup = queries.begin(); iQueryGroup != queries.end(); ++iQueryGroup)
	{
		LOG_INFO("Group " << (*iQueryGroup).first << " size " << ((*iQueryGroup).second).size());
		std::string groupnr = std::to_string((*iQueryGroup).first);
		showProgress(groupnr, queryCounter, querySize);
		queryCounter++;

		if((*iQueryGroup).first == 0)
		{
			LOG_INFO("Group 0 is not in the ground truth, evaluation step is skipped.");
			continue;
		}

		std::tuple<std::vector<std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>>>, float, float> interim;
		
		interim = evaluate((*iQueryGroup).first, (*iQueryGroup).second);
		avgMeanAveragePrecision += std::get<1>(interim);
		avgMeanAverageComputationTime += std::get<2>(interim);

		LOG_INFO("Mean Average Precision for group " << (*iQueryGroup).first << " is " << std::get<1>(interim));
		LOG_INFO("Average computation time for group " << (*iQueryGroup).first << " is " << std::get<2>(interim));

		mapvalues.push_back(std::make_pair((*iQueryGroup).first, std::get<1>(interim)));


	}

	appendValuesToCSVTemplate("MAP", mapvalues);
	avgMeanAveragePrecision /= float(queries.size());
	avgMeanAverageComputationTime /= float(queries.size());

	LOG_INFO("Total Mean Average Precision for " << queries.size() << " queries is " << avgMeanAveragePrecision);
	LOG_INFO("Total Average Computation for " << queries.size() << " queries is " << avgMeanAverageComputationTime << "s");

	//std::vector<boost::thread*> threads;
	//for (auto iQueryGroup = queries.begin(); iQueryGroup != queries.end(); ++iQueryGroup)
	//{
	//	threads.push_back(new boost::thread(boost::bind(&TRECVidValuation::evaluate, (*iQueryGroup).first, (*iQueryGroup).second, this)));
	//}
	

	//for(int iThread = 0; iThread < threads.size(); iThread++)
	//{
	//	threads[iThread]->join();
	//	delete threads[iThread];
	//}
}


std::tuple<std::vector<std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>>>, float, float> trecvid::TRECVidValuation::evaluate(int _queryid, std::vector<AVSFeatures*> _queries)
{
	std::tuple<std::vector<std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>>>, float, float>  results;
	int querySize = _queries.size();

	float meanAveragePrecision = 0.0;
	float meansAverageComputationTime = 0.0;

	for(int iQuery = 0; iQuery < querySize; iQuery++)
	{
		showProgress("Queries", iQuery, querySize);

		std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>> interim = evaluate(_queries.at(iQuery));
		std::get<0>(results).push_back(interim);

		meanAveragePrecision += interim.first->mAPValue;
		meansAverageComputationTime += interim.first->mAvgSearchtime;
	}

	meansAverageComputationTime /= float(querySize);
	meanAveragePrecision = meanAveragePrecision / float(querySize);

	std::get<1>(results) = meanAveragePrecision;
	std::get<2>(results) = meansAverageComputationTime;
	return results;
}

std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>> trecvid::TRECVidValuation::evaluate(AVSFeatures* _query)
{
	int modelSize = mModel.size();
	std::vector<defuse::ResultBase*> results;
	results.reserve(modelSize);

	float avgSearchTime = 0.0;
	float distance = 0.0;

	for (int iElem = 0; iElem < modelSize; iElem++)
	{
		showProgress("Model", iElem, modelSize);

		AVSFeatures* element = mModel.at(iElem);

		size_t e1_start, e1_end;
	
		e1_start = double(cv::getTickCount());
		distance = mDistance->compute(*(_query), *element);
		e1_end = cv::getTickCount();

		if (distance < 0)
		{
			LOG_ERROR(" Error: Distance is smaller than zero.");
		}
	
		double tickFrequency = double(cv::getTickFrequency());
		double searchTime = (e1_end - e1_start) / tickFrequency;
		avgSearchTime += searchTime;

		defuse::ResultBase* result = new defuse::ResultBase();
		result->mVideoFileName = element->mVideoFileName;
		result->mVideoID = element->mVID;
		result->mShotID = element->mSID;
		result->mQueryID = element->mQID;
		result->mDistance = distance;
		result->mSearchTime = searchTime;
	
		results.push_back(result);
	}
	
	avgSearchTime = avgSearchTime / float(modelSize);
	
	std::sort(results.begin(), results.end(), [](const defuse::ResultBase* s1, const defuse::ResultBase* s2)
	{
		return (s1->mDistance < s2->mDistance);
	});
	
	
	defuse::EvaluatedQuery* evalQuery = evaluate(_query, results, avgSearchTime);

	std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>> evaluation = std::make_pair(evalQuery, results);

	return evaluation;
}

defuse::EvaluatedQuery* trecvid::TRECVidValuation::evaluate(AVSFeatures* _query, std::vector<defuse::ResultBase*> _results, float _avgSearchTime)
{
	int matches = 0;

	float precisionAsSum = 0;

	float precision;

	int resultSize = _results.size();

	for (int iResult = 1; iResult < resultSize + 1; iResult++)
	{
		defuse::ResultBase* rankedresult = _results.at(iResult - 1);

		if (rankedresult->mQueryID == _query->mQID)
		{
			matches++;
			precisionAsSum += (static_cast<float>(matches) / static_cast<float>(iResult));
		}

		precision = ((static_cast<float>(matches) / static_cast<float>(iResult)));

		rankedresult->mPrecision = precision;
	}

	float ap = (precisionAsSum / static_cast<float>(matches));

	defuse::EvaluatedQuery* evalQuery = new defuse::EvaluatedQuery();
	evalQuery->mVideoFileName = _query->mVideoFileName;
	evalQuery->mQueryID = _query->mQID;
	evalQuery->mVideoID = _query->mVID;
	evalQuery->mShotID = _query->mSID;
	evalQuery->mAPValue = ap;
	evalQuery->mAvgSearchtime = _avgSearchTime;

	return evalQuery;
}

bool trecvid::TRECVidValuation::appendValuesToCSVTemplate(std::string type, std::vector<std::pair<int, float>> values) const
{
	std::ifstream csvfileIn;
	csvfileIn.open(mMAPValues->getFile().c_str(), std::ios_base::in);

	if (!csvfileIn.is_open())
	{
		LOG_ERROR("Error: Cannot open CSV File to append Values: " << mMAPValues->getFile());
	}

	//read first line (header of the table)
	std::string line;
	csvfileIn >> line;
	csvfileIn.close();

	//append the following lines
	std::ofstream csvfileOut;
	csvfileOut.open(mMAPValues->getFile().c_str(), std::ios_base::app);

	std::vector<std::string> elems = cplusutil::String::split(line, ',');

	//copy first line to add the values in the right column
	std::vector<std::string> newLine(elems);

	//first column is the model name
	newLine.at(0) = mFeatures->mDirName + ",";
	//second is the type of values
	newLine.at(0) += type + ",";

	for (int i = 2; i < elems.size(); i++)
	{
		int elem = std::atoi(elems.at(i).c_str());
		//LOG_DEBUG("Element " << elem);

		//foreach entry in the header, find its value
		for (int j = 0; j < values.size(); j++)
		{
			//LOG_DEBUG("Value " << values.at(j).first);

			if (values.at(j).first == elem)
			{
				std::string value = std::to_string(values.at(j).second) + ",";
				//LOG_DEBUG("Newline at " << i << " is " << value);
				newLine.at(i) = value;
				break;
			}

		}

	}
	//newLine.push_back(std::to_string(0));

	for (int i = 0; i < newLine.size(); i++)
	{
		//LOG_DEBUG("Write Line " << newLine.at(i));
		csvfileOut << newLine.at(i);
	}

	csvfileOut << std::endl;
	csvfileOut.close();
	return true;
}

void trecvid::TRECVidValuation::showProgress(std::string _name, int _step, int _total) const
{
	cplusutil::Terminal::showProgress(_name + " ", _step + 1, _total);
}


trecvid::TRECVidValuation::~TRECVidValuation()
{

}
