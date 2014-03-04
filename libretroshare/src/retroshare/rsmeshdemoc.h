#ifndef RSMESHDEMOC_H
#define RSMESHDEMOC_H


#include <inttypes.h>
#include <string>
#include <list>

#include "retroshare/rstokenservice.h"
#include "retroshare/rsgxsifacehelper.h"
#include "retroshare/rsgxscommon.h"

/* The Main Interface Class - for information about your Posted */
class RsMeshDemoc;
extern RsMeshDemoc *rsMeshDemoc;

class RsMeshDemocRepr;
class RsMeshDemocPost;
class RsMeshDemocGroup
{
	public:
	RsMeshDemocGroup() { return; }

	RsGroupMetaData mMeta;
	std::string mDescription;
};


//#define RSPOSTED_MSGTYPE_POST		0x0001
//#define RSPOSTED_MSGTYPE_VOTE		0x0002
//#define RSPOSTED_MSGTYPE_COMMENT	0x0004

#define RSPOSTED_PERIOD_YEAR		1
#define RSPOSTED_PERIOD_MONTH		2
#define RSPOSTED_PERIOD_WEEK		3
#define RSPOSTED_PERIOD_DAY		4
#define RSPOSTED_PERIOD_HOUR		5

#define RSPOSTED_VIEWMODE_LATEST	1
#define RSPOSTED_VIEWMODE_TOP		2
#define RSPOSTED_VIEWMODE_HOT		3
#define RSPOSTED_VIEWMODE_COMMENTS	4


std::ostream &operator<<(std::ostream &out, const RsMeshDemocGroup &group);
std::ostream &operator<<(std::ostream &out, const RsMeshDemocPost &post);


class RsMeshDemoc : public RsGxsIfaceHelper, public RsGxsCommentService
{
	    public:

	enum RankType {TopRankType, HotRankType, NewRankType };

	//static const uint32_t FLAG_MSGTYPE_POST;
	//static const uint32_t FLAG_MSGTYPE_MASK;

	RsMeshDemoc(RsGxsIface* gxs) : RsGxsIfaceHelper(gxs) { return; }
virtual ~RsMeshDemoc() { return; }

	    /* Specific Service Data */
virtual bool getGroupData(const uint32_t &token, std::vector<RsMeshDemocGroup> &groups) = 0;
virtual bool getPostData(const uint32_t &token, std::vector<RsMeshDemocPost> &posts) = 0;
	virtual bool getRelatedPosts(const uint32_t &token, std::vector<RsMeshDemocPost> &posts) = 0;
	virtual bool getRelatedReprs(const uint32_t &token, std::vector<RsMeshDemocRepr> &posts) = 0;
	virtual bool getRelatedVotes(const uint32_t &token, std::multimap<RsGxsMessageId, RsGxsVote *> &voteMap) = 0;

	    /* From RsGxsCommentService */
//virtual bool getCommentData(const uint32_t &token, std::vector<RsGxsComment> &comments) = 0;
//virtual bool getRelatedComments(const uint32_t &token, std::vector<RsGxsComment> &comments) = 0;
//virtual bool createComment(uint32_t &token, RsGxsComment &comment) = 0;
//virtual bool createVote(uint32_t &token, RsGxsVote &vote) = 0;

        //////////////////////////////////////////////////////////////////////////////
virtual void setMessageReadStatus(uint32_t& token, const RsGxsGrpMsgIdPair& msgId, bool read) = 0;

virtual bool createGroup(uint32_t &token, RsMeshDemocGroup &group) = 0;
virtual bool createPost(uint32_t &token, RsMeshDemocPost &post) = 0;
virtual bool createRepr(uint32_t &token, RsMeshDemocRepr &repr) = 0;


};



class RsMeshDemocPost
{
	public:
	RsMeshDemocPost()
	{
		//mMeta.mMsgFlags = RsMeshDemoc::FLAG_MSGTYPE_POST;
		mUpVotes = 0;
		mDownVotes = 0;
		mComments = 0;
		mHaveVoted = false;
		return;
	}

	bool calculateScores(time_t ref_time);

	RsMsgMetaData mMeta;
	std::string mLink;
	std::string mNotes;

	bool     mHaveVoted;

	// Calculated.
	uint32_t mUpVotes;
	uint32_t mDownVotes;
	uint32_t mComments;


	// and Calculated Scores:???
	double  mHotScore;
	double  mTopScore;
	double  mNewScore;
};



class RsMeshDemocRepr
{
	public:

	std::string mRepresenterId;


	RsMsgMetaData mMeta;
};

#endif // RSMESHDEMOC_H
