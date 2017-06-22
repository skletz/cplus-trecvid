#ifndef _TRECVIDUPDATE_HPP_
#define  _TRECVIDUPDATE_HPP_

#include "toolbase.hpp"
#include <defuse.hpp>
#include "mastershot.hpp"
#include "avsquery.hpp"

namespace trecvid {

	/**
	* \brief
	*/
	class TRECVidUpdate : public vretbox::ToolBase
	{

		/**
		* \brief input file
		*/
		File* mGroundTruth;

		/**
		* \brief input file
		*/
		File* mMasterShots;

		/**
		 * \brief output file
		 */
		File* mUpdatedGroundTruth;

	public:

		/**
		* \brief
		*/
		TRECVidUpdate();

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
		~TRECVidUpdate() override;

		static void updateQueryList(std::list<trecvid::MasterShot*>* _shots, std::list<trecvid::AVSQuery*>* _queries);

		static bool writeQueryList(std::list<trecvid::AVSQuery*>* _queries, std::string _file);

		void showProgress(std::string _name, int _step, int _total) const;

	};
}

#endif //_TRECVIDUPDATE_HPP_
