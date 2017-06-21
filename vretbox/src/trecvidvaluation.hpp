#ifndef _TRECVIDVALUATION_HPP_
#define  _TRECVIDVALUATION_HPP_

#include "toolbase.hpp"
#include "avsquery.hpp"
#include <defuse.hpp>
#include "avsfeatures.hpp"
#include <unordered_map>


namespace trecvid {

	/**
	* \brief
	*/
	class TRECVidValuation : public vretbox::ToolBase
	{

		defuse::Distance* mDistance;

		defuse::Xtractor* mXtractor;

		std::vector<AVSFeatures*> mModel;

		Directory* mFeatures;

		File* mGroundTruth;

		File* mMAPValues;

	public:

		/**
		* \brief
		*/
		TRECVidValuation();

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
		~TRECVidValuation() override;



		/**
		* \brief Evaluates the mean average precision for the set of _queries
		* \param _queryid unique number
		* \param _queries set of queries related to the unique number
		* \return a triple of the evaluated interim results, its mean average precision value and its average compution time
		*/
		std::tuple<std::vector<std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>>>, float, float> evaluate(int _queryid, std::vector<AVSFeatures*> _queries);

		std::pair<defuse::EvaluatedQuery*, std::vector<defuse::ResultBase*>> evaluate(AVSFeatures* _query);

		defuse::EvaluatedQuery* evaluate(AVSFeatures* _query, std::vector<defuse::ResultBase*> _results, int _avgSearchTime);

		/**
		 * \brief Append evaluation values to csv template. Examples can be found in trecvid-maps.csv
		 * \param type the evaluation type (MAP, P at k, ...)
		 * \param values set of evaluation results with query id and its value
		 * \return true if the addition was successful, otherwise false
		 */
		bool appendValuesToCSVTemplate(std::string type, std::vector<std::pair<int, float>> values) const;
	};

}

#endif //_TRECVIDVALUATION_HPP_