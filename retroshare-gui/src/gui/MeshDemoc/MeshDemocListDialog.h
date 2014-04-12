/*
 * Retroshare MeshDemoc List
 *
 * Copyright 2012-2012 by Robert Fernie.
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

#ifndef MRK_MESHDEMOC_LIST_DIALOG_H
#define MRK_MESHDEMOC_LIST_DIALOG_H

#include "retroshare-gui/mainpage.h"
#include "ui_MeshDemocListDialog.h"

#include <retroshare/rsmeshdemoc.h>

#include "util/TokenQueue.h"
#include "gui/gxs/RsGxsUpdateBroadcastPage.h"

#include "gui/gxs/GxsCommentContainer.h"
#include "gui/feeds/FeedHolder.h"

#include "meshdemoccachetree.h"

class MeshDemocItem;

/*********************** **** **** **** ***********************/
/** Request / Response of Data ********************************/
/*********************** **** **** **** ***********************/

#define MESHDEMOCDIALOG_LISTING        1
#define MESHDEMOCDIALOG_CURRENTFORUM   2
#define MESHDEMOCDIALOG_INSERTTHREADS  3
#define MESHDEMOCDIALOG_INSERTCHILD    4
#define MESHDEMOCDIALOG_INSERT_POST    5
#define MESHDEMOCDIALOG_REPLY_MESSAGE  6

class MeshDemocListDialog : public RsGxsUpdateBroadcastPage, public FeedHolder, public TokenResponse, public GxsServiceDialog
{
	Q_OBJECT

public:
	MeshDemocListDialog(QWidget *parent = 0);

	// Overloaded from FeedHolder.
	virtual QScrollArea *getScrollArea();
	virtual void deleteFeedItem(QWidget *item, uint32_t type);
	virtual void openChat(std::string peerId);
	virtual void openComments(uint32_t type, const RsGxsGroupId &groupId, const RsGxsMessageId &msgId, const QString &title);

private slots:
	void groupListCustomPopupMenu(QPoint point);
	void changedTopic(const QString &id);

	void newTopic();
	void showTopic();
	void editTopic();

	void showGroupDetails();
	void newPost();

	void submitVote(const RsGxsGrpMsgIdPair& msgId, bool up);

	void getRankings();

	void subscribeTopic();
	void unsubscribeTopic();

	void showNext();
	void showPrev();
	
	void todo();

	void showCurrentReprs();
	void showTopicRepChart();
	void showReprs(RsGxsGroupId groupId);
	void showVotes(RsGxsGrpMsgIdPair msgID);
	void showVoteChart(RsGxsGrpMsgIdPair msgID, std::string postTitle);
	void newSubTopic();
	void newRepresentitive();
	void selectRepr(std::string reprId);
protected:
	virtual void updateDisplay(bool complete);

private:
	void updateShowText();

	void clearPosts();

	/*!
	 * Only removes it from layout
	 */
	void shallowClearPosts();

	void loadPost(const RsMeshDemocPost &post);

	void insertGroups();
	void requestGroupSummary();
	void acknowledgeGroup(const uint32_t &token);
	void loadGroupSummary(const uint32_t &token);

	void requestGroupSummary_CurrentForum(const std::string &forumId);
	void loadGroupSummary_CurrentForum(const uint32_t &token);

	// subscribe/unsubscribe ack.
	void acknowledgeSubscribeChange(const uint32_t &token);

	// posts
	void acknowledgePostMsg(const uint32_t &token);
	void loadPostData(const uint32_t &token);
	void insertThreads();
	void loadCurrentTopicThreads(const std::string &forumId);
	void requestGroupThreadData_InsertThreads(const std::string &forumId);
	void loadGroupThreadData_InsertThreads(const uint32_t &token);

	// votes
	void acknowledgeVoteMsg(const uint32_t& token);
	void loadVoteData(const uint32_t &token);

	// ranking
	//void loadRankings(const uint32_t& token);
	//void applyRanking(const MeshDemocRanking& ranks);
	void applyRanking();

	// update displayed item
	void updateDisplayedItems(const std::vector<RsGxsMessageId>& msgIds);
	void updateCurrentDisplayComplete(const uint32_t& token);

	void insertGroupData(const std::list<RsGroupMetaData> &groupList);
	void groupInfoToGroupItemInfo(const RsGroupMetaData &groupInfo, GroupRecurItemInfo &groupItemInfo);

	void loadRequest(const TokenQueue *queue, const TokenRequest &req);


	

private:
	QTreeWidgetItem *yourTopics;
	QTreeWidgetItem *subscribedTopics;
	QTreeWidgetItem *allTopics;

	int	mSortMethod;
	int	mLastSortMethod;
	int	mPostIndex;
	int	mPostShow;

	bool mThreadLoading;
	RsGxsGroupId mCurrTopicId;
	gxsIdReprMmap mCurrTopicReprs;

	VoteCache liquidCache;

	QMap<RsGxsGroupId, RsMeshDemocGroup> mGroups;
	QMap<RsGxsMessageId, MeshDemocItem*> mPosts;
	std::list<MeshDemocItem *> mPostList;

	TokenQueue *mMeshDemocQueue;

	//CommentHolder* mCommentHolder;

	/* UI - from Designer */
	Ui::MeshDemocListDialog ui;
	void showVotesFromToken(u_int32_t token);
	void showReprsFromToken(u_int32_t token);
	void showVoteChartFromToken(u_int32_t token);
};

#endif

