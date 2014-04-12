/*
 * libretroshare/src/services p3meshdemoc.cc
 *
 * Posted interface for RetroShare.
 *
 * Copyright 2012-2012 by Robert Fernie.
 * Copyright 2014 Alex "Chozabu" P-B.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License Version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Please report all bugs and problems to "retroshare@lunamutt.com".
 *
 */

#include "p3meshdemoc.h"
#include "serialiser/rsmeshdemocitems.h"

#include <math.h>

/****
 * #define POSTED_DEBUG 1
 ****/

//TODO, remove unused defines - the ones in postedbase are mostly the ones that count
#define POSTBASE_BACKGROUND_PROCESSING	0x0002
#define PROCESSING_START_PERIOD		30
#define PROCESSING_INC_PERIOD		15

#define POSTBASE_ALL_GROUPS 		0x0011
#define POSTBASE_UNPROCESSED_MSGS	0x0012
#define POSTBASE_ALL_MSGS 		0x0013
#define POSTBASE_BG_POST_META		0x0014


RsMeshDemoc *rsMeshDemoc = NULL;

/********************************************************************************/
/******************* Startup / Tick    ******************************************/
/********************************************************************************/

p3MeshDemoc::p3MeshDemoc(RsGeneralDataService* gds, RsNetworkExchangeService* nes, RsGixs* gixs)
    :p3PostBase(gds, nes, gixs, new RsGxsMeshDemocSerialiser(), RS_SERVICE_GXSV2_TYPE_MESH_DEMOC)
    ,RsMeshDemoc(this)
{
	return;
}


bool p3MeshDemoc::getGroupData(const uint32_t &token, std::vector<RsMeshDemocGroup> &groups)
{
	std::vector<RsGxsGrpItem*> grpData;
	bool ok = RsGenExchange::getGroupData(token, grpData);

	if(ok)
	{
		std::vector<RsGxsGrpItem*>::iterator vit = grpData.begin();

		for(; vit != grpData.end(); vit++)
		{
			RsGxsMeshDemocGroupItem* item = dynamic_cast<RsGxsMeshDemocGroupItem*>(*vit);
			if (item)
			{
				RsMeshDemocGroup grp = item->mGroup;
				item->mGroup.mMeta = item->meta;
				grp.mMeta = item->mGroup.mMeta;
				delete item;
				groups.push_back(grp);
			}
			else
			{
				std::cerr << "Not a RsGxsPostedGroupItem, deleting!" << std::endl;
				delete *vit;
			}
		}
	}
	return ok;
}

bool p3MeshDemoc::getPostData(const uint32_t &token, std::vector<RsMeshDemocPost> &msgs)
{
	GxsMsgDataMap msgData;
	bool ok = RsGenExchange::getMsgData(token, msgData);
	time_t now = time(NULL);

	if(ok)
	{
		GxsMsgDataMap::iterator mit = msgData.begin();

		for(; mit != msgData.end();  mit++)
		{
			RsGxsGroupId grpId = mit->first;
			std::vector<RsGxsMsgItem*>& msgItems = mit->second;
			std::vector<RsGxsMsgItem*>::iterator vit = msgItems.begin();

			for(; vit != msgItems.end(); vit++)
			{
				RsGxsMeshDemocPostItem* item = dynamic_cast<RsGxsMeshDemocPostItem*>(*vit);

				if(item)
				{
					RsMeshDemocPost msg = item->mPost;
					msg.mMeta = item->meta;
					msg.calculateScores(now);

					msgs.push_back(msg);
					delete item;
				}
				else
				{
					std::cerr << "Not a PostedPostItem, deleting!" << std::endl;
					delete *vit;
				}
			}
		}
	}

	return ok;
}


bool p3MeshDemoc::getRelatedPosts(const uint32_t &token, std::vector<RsMeshDemocPost> &msgs)
{
	GxsMsgRelatedDataMap msgData;
	bool ok = RsGenExchange::getMsgRelatedData(token, msgData);
	time_t now = time(NULL);

	if(ok)
	{
		GxsMsgRelatedDataMap::iterator mit = msgData.begin();

		for(; mit != msgData.end();  mit++)
		{
			std::vector<RsGxsMsgItem*>& msgItems = mit->second;
			std::vector<RsGxsMsgItem*>::iterator vit = msgItems.begin();

			for(; vit != msgItems.end(); vit++)
			{
				RsGxsMeshDemocPostItem* item = dynamic_cast<RsGxsMeshDemocPostItem*>(*vit);

				if(item)
				{
					RsMeshDemocPost msg = item->mPost;
					msg.mMeta = item->meta;
					msg.calculateScores(now);

					msgs.push_back(msg);
					delete item;
				}
				else
				{
					std::cerr << "Not a PostedPostItem, deleting!" << std::endl;
					delete *vit;
				}
			}
		}
	}

	return ok;
}



bool p3MeshDemoc::getRelatedReprs(const uint32_t &token, gxsIdReprMmap &reprMap)
{
	GxsMsgDataMap msgData;
	bool ok = RsGenExchange::getMsgData(token, msgData);
	std::map<RsGxsId, RsMeshDemocRepr *> backwardsReprMap;

	if(ok)
	{
		GxsMsgDataMap::iterator mit = msgData.begin();

		//extract representation - remove multi-representers
		for(; mit != msgData.end();  mit++)
		{
			std::vector<RsGxsMsgItem*>& msgItems = mit->second;
			std::vector<RsGxsMsgItem*>::iterator vit = msgItems.begin();

			for(; vit != msgItems.end(); vit++)
			{
				RsGxsMeshDemocRepresentationItem* item = dynamic_cast<RsGxsMeshDemocRepresentationItem*>(*vit);

				if(item)
				{
					RsMeshDemocRepr* msg = &(item->mPost);
					msg->mMeta = item->meta;
					//insert if not found, overwrite if found
					if(backwardsReprMap.find(msg->mMeta.mAuthorId) == backwardsReprMap.end())
					{
						//backwardsReprMap.[msg->mMeta.mAuthorId] = &msg;
						//backwardsReprMap.insert(std::make_pair(msg->mRepresenterId, msg));
						backwardsReprMap.insert(std::make_pair(msg->mMeta.mAuthorId, msg));
					} else {
						if (msg->mMeta.mPublishTs > backwardsReprMap[msg->mMeta.mAuthorId]->mMeta.mPublishTs)
							backwardsReprMap[msg->mMeta.mAuthorId] = msg;//delete old item first?
						else
							delete item;
					}
				}
				else
				{
					std::cerr << "Not a RsGxsMeshDemocRepresentationItem, deleting!" << std::endl;
					delete *vit;
				}
			}
		}

		//put into map based on representee
		std::map<RsGxsId, RsMeshDemocRepr *>::iterator bmit = backwardsReprMap.begin();
		for(; bmit != backwardsReprMap.end(); bmit++)
		{
			reprMap.insert(std::make_pair(bmit->second->mRepresenterId, bmit->second));
		}

	}

	return ok;
}

bool p3MeshDemoc::getGxsRelatedVotes(const uint32_t &token, msgVoteMmap &voteMap)
{
	std::cerr << "\n\n---\n\np3GxsCommentService::getGxsRelatedVotes()";
	std::cerr << std::endl;
	std::cerr << "Token: " << token;
	std::cerr << std::endl;

	GxsMsgRelatedDataMap msgData;
	bool ok = RsGenExchange::getMsgRelatedData(token, msgData);


	std::cerr << "msgData.size(): " << msgData.size();
	std::cerr << std::endl;

	if(ok)
	{
		GxsMsgRelatedDataMap::iterator mit = msgData.begin();

		for(; mit != msgData.end();  mit++)
		{
			std::cerr << "mit";
			std::cerr << std::endl;
			std::vector<RsGxsMsgItem*>& msgItems = mit->second;
			std::vector<RsGxsMsgItem*>::iterator vit = msgItems.begin();

			for(; vit != msgItems.end(); vit++)
			{
				std::cerr << "vit";
				std::cerr << std::endl;

				RsGxsVoteItem* vote = dynamic_cast<RsGxsVoteItem*>(*vit);
				if (vote)
				{
					voteMap.insert(std::make_pair(vote->meta.mParentId, vote));
					std::cerr << "!!" << std::endl;
					std::cerr << "GOT A VOTE!" << std::endl;
					std::cerr << "!!" << std::endl;
				}
				else
				{
					std::cerr << "Not a Vote, deleting!" << std::endl;
					delete *vit;
				}
			}
		}
	} else {
		std::cerr << "nope failed on:";
		std::cerr << std::endl;
		std::cerr << "mExchange->getGxsRelatedVotes(token, msgData);\n\n\n\n\n";
	}

	return ok;
}

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

/* Switched from having explicit Ranking calculations to calculating the set of scores
 * on each RsPostedPost item.
 *
 * TODO: move this function to be part of RsPostedPost - then the GUI 
 * can reuse is as necessary.
 *
 */

bool RsMeshDemocPost::calculateScores(time_t ref_time)
{
	/* so we want to calculate all the scores for this Post. */

	PostStats stats;
	extractPostCache(mMeta.mServiceString, stats);

	mUpVotes = stats.up_votes;
	mDownVotes = stats.down_votes;
	mComments = stats.comments;
	mHaveVoted = (mMeta.mMsgStatus & GXS_SERV::GXS_MSG_STATUS_VOTE_MASK);

	time_t age_secs = ref_time - mMeta.mPublishTs;
#define POSTED_AGESHIFT (2.0)
#define POSTED_AGEFACTOR (3600.0)

	mTopScore = ((int) mUpVotes - (int) mDownVotes);
	if (mTopScore > 0)
	{
		// score drops with time.
		mHotScore =  mTopScore / pow(POSTED_AGESHIFT + age_secs / POSTED_AGEFACTOR, 1.5);
	}
	else
	{
		// gets more negative with time.
		mHotScore =  mTopScore * pow(POSTED_AGESHIFT + age_secs / POSTED_AGEFACTOR, 1.5);
	}
	mNewScore = -age_secs;

	return true;
}

/********************************************************************************************/
/********************************************************************************************/

bool p3MeshDemoc::createGroup(uint32_t &token, RsMeshDemocGroup &group)
{
	std::cerr << "p3MeshDemoc::createGroup()" << std::endl;

	RsGxsMeshDemocGroupItem* grpItem = new RsGxsMeshDemocGroupItem();
	grpItem->mGroup = group;
	grpItem->meta = group.mMeta;

	RsGenExchange::publishGroup(token, grpItem);
	return true;
}


bool p3MeshDemoc::createPost(uint32_t &token, RsMeshDemocPost &msg)
{
	std::cerr << "p3MeshDemoc::createPost() GroupId: " << msg.mMeta.mGroupId;
	std::cerr << std::endl;

	RsGxsMeshDemocPostItem* msgItem = new RsGxsMeshDemocPostItem();
	msgItem->mPost = msg;
	msgItem->meta = msg.mMeta;

	RsGenExchange::publishMsg(token, msgItem);
	return true;
}

bool p3MeshDemoc::createRepr(uint32_t &token, RsMeshDemocRepr &msg)
{
	std::cerr << "p3MeshDemoc::createRepr() GroupId: " << msg.mMeta.mGroupId;
	std::cerr << std::endl;

	RsGxsMeshDemocRepresentationItem* msgItem = new RsGxsMeshDemocRepresentationItem();
	msgItem->mPost = msg;
	msgItem->meta = msg.mMeta;

	RsGenExchange::publishMsg(token, msgItem);
	return true;
}

/********************************************************************************************/

/* This function is generalised to support any collection of messages, across multiple groups */

void p3MeshDemoc::background_loadMsgs(const uint32_t &token, bool unprocessed)
{
	/* get messages */
	std::cerr << "p3PostBase::background_loadMsgs()";
	std::cerr << std::endl;

	std::map<RsGxsGroupId, std::vector<RsGxsMsgItem*> > msgData;
	bool ok = RsGenExchange::getMsgData(token, msgData);

	if (!ok)
	{
		std::cerr << "p3PostBase::background_loadMsgs() Failed to getMsgData()";
		std::cerr << std::endl;

		/* cleanup */
		background_cleanup();
		return;

	}

	{
		RsStackMutex stack(mPostBaseMtx); /********** STACK LOCKED MTX ******/
		mBgStatsMap.clear();
		mBgIncremental = unprocessed;
	}

	std::map<RsGxsGroupId, std::vector<RsGxsMessageId> > postMap;

	// generate vector of changes to push to the GUI.
	std::vector<RsGxsNotify *> changes;
	RsGxsMsgChange *msgChanges = new RsGxsMsgChange(RsGxsNotify::TYPE_PROCESSED);


	RsGxsGroupId groupId;
	std::map<RsGxsGroupId, std::vector<RsGxsMsgItem*> >::iterator mit;
	std::vector<RsGxsMsgItem*>::iterator vit;
	for (mit = msgData.begin(); mit != msgData.end(); mit++)
	{
		  groupId = mit->first;
		  for (vit = mit->second.begin(); vit != mit->second.end(); vit++)
		  {
			RsGxsMessageId parentId = (*vit)->meta.mParentId;
			RsGxsMessageId threadId = (*vit)->meta.mThreadId;


			bool inc_counters = false;
			uint32_t vote_inc = 0;
			uint32_t vote_up_inc = 0;
			uint32_t vote_down_inc = 0;
			uint32_t comment_inc = 0;

			bool add_voter = false;
			RsGxsId voterId;
			RsGxsCommentItem *commentItem;
			RsGxsVoteItem    *voteItem;

			/* THIS Should be handled by UNPROCESSED Filter - but isn't */
			if (!IS_MSG_UNPROCESSED((*vit)->meta.mMsgStatus))
			{
				RsStackMutex stack(mPostBaseMtx); /********** STACK LOCKED MTX ******/
				if (mBgIncremental)
				{
					std::cerr << "p3PostBase::background_loadMsgs() Msg already Processed - Skipping";
					std::cerr << std::endl;
					std::cerr << "p3PostBase::background_loadMsgs() ERROR This should not happen";
					std::cerr << std::endl;
					delete(*vit);
					continue;
				}
			}

			/* 3 types expected: PostedPost, Comment and Vote */
			if (parentId.empty())
			{
				/* we don't care about top-level (Posts) */
				std::cerr << "\tIgnoring TopLevel Item";
				std::cerr << std::endl;

				/* but we need to notify GUI about them */
				msgChanges->msgChangeMap[mit->first].push_back((*vit)->meta.mMsgId);
			}
			else if (NULL != (commentItem = dynamic_cast<RsGxsCommentItem *>(*vit)))
			{
				/* comment - want all */
				/* Comments are counted by Thread Id */
				std::cerr << "\tProcessing Comment: " << commentItem;
				std::cerr << std::endl;

				inc_counters = true;
				comment_inc = 1;
			}
			else if (NULL != (voteItem = dynamic_cast<RsGxsVoteItem *>(*vit)))
			{
				/* vote - only care about direct children */
				if (parentId == threadId)
				{
					/* Votes are organised by Parent Id,
					 * ie. you can vote for both Posts and Comments
					 */
					std::cerr << "\tProcessing Vote: " << voteItem;
					std::cerr << std::endl;

					inc_counters = true;
					add_voter = true;
					voterId = voteItem->meta.mAuthorId;

					if (voteItem->mMsg.mVoteType == GXS_VOTE_UP)
					{
						vote_up_inc = 1;
						vote_inc = 1;
					}
					else
					{
						vote_down_inc = 1;
						vote_inc = -1;
					}
				}
			}
			else
			{
				/* unknown! */
				std::cerr << "p3PostBase::background_processNewMessages() ERROR Strange NEW Message:";
				std::cerr << std::endl;
				std::cerr << "\t" << (*vit)->meta;
				std::cerr << std::endl;

			}

			if (inc_counters)
			{
				RsStackMutex stack(mPostBaseMtx); /********** STACK LOCKED MTX ******/

				std::map<RsGxsMessageId, PostStats>::iterator sit = mBgStatsMap.find(threadId);
				if (sit == mBgStatsMap.end())
				{
					// add to map of ones to update.
					postMap[groupId].push_back(threadId);

					mBgStatsMap[threadId] = PostStats(0,0,0);
					sit = mBgStatsMap.find(threadId);
				}

				/*std::map<RsGxsId, int>::iterator voteit = sit->second.votes.find(voterId);
				if (add_voter && voteit != sit->second.votes.end())
				{//undo last vote by this ID before accepting new one
					int lastvote = sit->second.votes[voterId];
					sit->second.up_votes -= lastvote;
					sit->second.down_votes += lastvote;
				}*/

				sit->second.comments += comment_inc;
				sit->second.up_votes += vote_up_inc;
				sit->second.down_votes += vote_down_inc;

				if (add_voter)
				{
					sit->second.voters.push_back(voterId);
					//sit->second.votes[voterId] = vote_inc;//map stores each IDs vote - no anonymity by this method.
				}

				std::cerr << "\tThreadId: " << threadId;
				std::cerr << " Comment Total: " << sit->second.comments;
				std::cerr << " UpVote Total: " << sit->second.up_votes;
				std::cerr << " DownVote Total: " << sit->second.down_votes;
				std::cerr << std::endl;
			}

			/* flag all messages as processed */
			if ((*vit)->meta.mMsgStatus & GXS_SERV::GXS_MSG_STATUS_UNPROCESSED)
			{
				uint32_t token_a;
				RsGxsGrpMsgIdPair msgId = std::make_pair(groupId, (*vit)->meta.mMsgId);
				RsGenExchange::setMsgStatusFlags(token_a, msgId, 0, GXS_SERV::GXS_MSG_STATUS_UNPROCESSED);
			}
			delete(*vit);
		}
	}

	/* push updates of new Posts */
	if (msgChanges->msgChangeMap.size() > 0)
	{
		std::cerr << "p3PostBase::background_processNewMessages() -> receiveChanges()";
		std::cerr << std::endl;

		changes.push_back(msgChanges);
	 	receiveHelperChanges(changes);
	}

	/* request the summary info from the parents */
	uint32_t token_b;
	uint32_t anstype = RS_TOKREQ_ANSTYPE_SUMMARY;
	RsTokReqOptions opts;
	opts.mReqType = GXS_REQUEST_TYPE_MSG_META;
	RsGenExchange::getTokenService()->requestMsgInfo(token_b, anstype, opts, postMap);

	GxsTokenQueue::queueRequest(token_b, POSTBASE_BG_POST_META);
	return;
}
