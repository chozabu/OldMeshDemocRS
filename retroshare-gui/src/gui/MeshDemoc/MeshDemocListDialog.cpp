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

#include "MeshDemocListDialog.h"

#include "MeshDemocGroupDialog.h"
#include "MeshDemocCreatePostDialog.h"
#include "MeshDemocDialog.h"
#include "MeshDemocItem.h"
#include "MeshDemocUserTypes.h"
#include "SelectRepresentitiveDialog.h"

#include <iostream>

#include <QMenu>
#include <QMessageBox>

/****************************************************************
 */

/* Images for context menu icons */
#define IMAGE_MESSAGE	":/images/folder-draft.png"
#define IMAGE_SUBSCRIBE      ":/images/edit_add24.png"
#define IMAGE_UNSUBSCRIBE    ":/images/cancel.png"
#define IMAGE_SHOW           ":/images/info16.png"
#define IMAGE_EDIT           ":/images/settings16.png"

/* Images for TreeWidget */
#define IMAGE_FOLDER	 ":/images/folder16.png"
#define IMAGE_FOLDERGREEN    ":/images/folder_green.png"
#define IMAGE_FOLDERRED      ":/images/folder_red.png"
#define IMAGE_FOLDERYELLOW   ":/images/folder_yellow.png"

// token types to deal with

#define MESHDEMOC_DEFAULT_LISTING_LENGTH 10
#define MESHDEMOC_MAX_INDEX	      10000

/** Constructor */
MeshDemocListDialog::MeshDemocListDialog(QWidget *parent)
: RsGxsUpdateBroadcastPage(rsMeshDemoc, parent), GxsServiceDialog(dynamic_cast<GxsCommentContainer *>(parent))
{
	/* Invoke the Qt Designer generated object setup routine */
	ui.setupUi(this);

	/* Setup Queue */
	mMeshDemocQueue = new TokenQueue(rsMeshDemoc->getTokenService(), this);

	connect( ui.groupTreeWidget, SIGNAL(treeCustomContextMenuRequested(QPoint)), this, SLOT(groupListCustomPopupMenu(QPoint)));
	connect( ui.groupTreeWidget, SIGNAL(treeCurrentItemChanged(QString)), this, SLOT(changedTopic(QString)));

	connect(ui.hotSortButton, SIGNAL(clicked()), this, SLOT(getRankings()));
	connect(ui.newSortButton, SIGNAL(clicked()), this, SLOT(getRankings()));
	connect(ui.topSortButton, SIGNAL(clicked()), this, SLOT(getRankings()));
	connect(ui.nextButton, SIGNAL(clicked()), this, SLOT(showNext()));
	connect(ui.prevButton, SIGNAL(clicked()), this, SLOT(showPrev()));
	
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(todo()));

	// default sort method.
	mSortMethod = RsMeshDemoc::HotRankType;
	mLastSortMethod = RsMeshDemoc::TopRankType; // to be different.
	mPostIndex = 0;
	mPostShow = MESHDEMOC_DEFAULT_LISTING_LENGTH;
	
	/* Initialize group tree */
	QToolButton *newTopicButton = new QToolButton(this);
	newTopicButton->setIcon(QIcon(":/images/posted_add_24.png"));
	newTopicButton->setToolTip(tr("Create New Topic"));
	connect(newTopicButton, SIGNAL(clicked()), this, SLOT(newTopic()));
	ui.groupTreeWidget->addToolButton(newTopicButton);

	/* create posted tree */
	yourTopics = ui.groupTreeWidget->addCategoryItem(tr("My Topics"), QIcon(IMAGE_FOLDER), true);
	subscribedTopics = ui.groupTreeWidget->addCategoryItem(tr("Subscribed Topics"), QIcon(IMAGE_FOLDERRED), true);
	allTopics = ui.groupTreeWidget->addCategoryItem(tr("All Topics"), QIcon(IMAGE_FOLDERYELLOW), false);

	ui.hotSortButton->setChecked(true);

	connect(ui.submitPostButton, SIGNAL(clicked()), this, SLOT(newPost()));
}

void MeshDemocListDialog::showNext()
{
	mPostIndex += mPostShow;
	if (mPostIndex > MESHDEMOC_MAX_INDEX)
		mPostIndex = MESHDEMOC_MAX_INDEX;
	applyRanking();
	updateShowText();
}

void MeshDemocListDialog::showPrev()
{
	mPostIndex -= mPostShow;
	if (mPostIndex < 0)
		mPostIndex = 0;
	applyRanking();
	updateShowText();
}

void MeshDemocListDialog::updateShowText()
{
	QString showText = tr("Showing");
	showText += " ";
	showText += QString::number(mPostIndex + 1);
	showText += "-";
	showText += QString::number(mPostIndex + mPostShow);
	ui.showLabel->setText(showText);
}

void MeshDemocListDialog::getRankings()
{
	if(mCurrTopicId.empty())
		return;

	std::cerr << "MeshDemocListDialog::getRankings()";
	std::cerr << std::endl;

	int oldSortMethod = mSortMethod;

	QObject* button = sender();
	if(button == ui.hotSortButton)
	{
		mSortMethod = RsMeshDemoc::HotRankType;
	}
	else if(button == ui.topSortButton)
	{
		mSortMethod = RsMeshDemoc::TopRankType;
	}
	else if(button == ui.newSortButton)
	{
		mSortMethod = RsMeshDemoc::NewRankType;
	}
	else
	{
		return;
	}

	if (oldSortMethod != mSortMethod)
	{
		/* Reset Counter */
		mPostIndex = 0;
		updateShowText();
	}

	applyRanking();
}

void MeshDemocListDialog::groupListCustomPopupMenu(QPoint /*point*/)
{
	if (mCurrTopicId.empty())
	{
		return;
	}

	uint32_t subscribeFlags = ui.groupTreeWidget->subscribeFlags(QString::fromStdString(mCurrTopicId));

	QMenu contextMnu(this);

	//bool isAdmin = IS_GROUP_ADMIN(subscribeFlags);
	//bool isPublisher = IS_GROUP_PUBLISHER(subscribeFlags);
	bool isSubscribed = IS_GROUP_SUBSCRIBED(subscribeFlags);

	QAction *action = contextMnu.addAction(QIcon(IMAGE_MESSAGE), tr("Submit a new Post"), this, SLOT(newPost()));
	action->setEnabled(isSubscribed);
	action = contextMnu.addAction(QIcon(IMAGE_SUBSCRIBE), tr("Subscribe"), this, SLOT(subscribeTopic()));
	action->setEnabled(!isSubscribed);
	action = contextMnu.addAction(QIcon(IMAGE_UNSUBSCRIBE), tr("Unsubscribe"), this, SLOT(unsubscribeTopic()));
	action->setEnabled(isSubscribed);

	contextMnu.addSeparator();

	action = contextMnu.addAction(QIcon(IMAGE_SHOW), tr("Show Topic Details"), this, SLOT(showTopic()));
	action->setEnabled (!mCurrTopicId.empty());

	action = contextMnu.addAction(QIcon(IMAGE_EDIT), tr("Edit Topic Details"), this, SLOT(editTopic()));
	action->setEnabled (!mCurrTopicId.empty() && IS_GROUP_ADMIN(subscribeFlags));

	contextMnu.addSeparator();

	action = contextMnu.addAction(QIcon(IMAGE_FOLDERGREEN), tr("New Child-Group"), this, SLOT(newSubTopic()));
	action->setEnabled(isSubscribed);
	action = contextMnu.addAction(QIcon(IMAGE_FOLDER), tr("Select Representitive"), this, SLOT(newRepresentitive()));
	action->setEnabled(isSubscribed);
	action = contextMnu.addAction(QIcon(IMAGE_FOLDER), tr("Show Representitives"), this, SLOT(showCurrentReprs()));
	action->setEnabled(isSubscribed);

	contextMnu.exec(QCursor::pos());
}

void MeshDemocListDialog::newSubTopic()
{
    std::cerr << "mCurrTopicId: " << mCurrTopicId << std::endl;
	if (mCurrTopicId.empty()) {
        return;
    }
    MeshDemocGroupDialog cf (mMeshDemocQueue, this);
    cf.setParentLabel(mCurrTopicId.c_str());
    cf.exec ();

}
void MeshDemocListDialog::newRepresentitive()
{
    std::cerr << "mCurrTopicId: " << mCurrTopicId << std::endl;
	if (mCurrTopicId.empty()) {
        return;
    }

	uint32_t subscribeFlags = ui.groupTreeWidget->subscribeFlags(QString::fromStdString(mCurrTopicId));
	bool isSubscribed = IS_GROUP_SUBSCRIBED(subscribeFlags);
	if (isSubscribed)
	{
    SelectRepresentitiveDialog srd(mMeshDemocQueue, rsMeshDemoc, mCurrTopicId, this);
	srd.exec();
	}

}

void MeshDemocListDialog::showVotes(RsGxsGrpMsgIdPair msgID)
{

	RsTokReqOptions opts;
	opts.mReqType = GXS_REQUEST_TYPE_MSG_RELATED_DATA;
	opts.mOptions = RS_TOKREQOPT_MSG_PARENT | RS_TOKREQOPT_MSG_LATEST;
	std::vector<RsGxsGrpMsgIdPair> msgIds;
	msgIds.push_back(msgID);
	uint32_t token;
	mMeshDemocQueue->requestMsgRelatedInfo(token, RS_TOKREQ_ANSTYPE_DATA, opts, msgIds, TOKENREQ_MSGRELATEDINFO);
}

void MeshDemocListDialog::showCurrentReprs()
{
	showReprs(mCurrTopicId);
}

void MeshDemocListDialog::showReprs(RsGxsGroupId groupId)
{
	RsTokReqOptions opts;

	opts.mReqType = GXS_REQUEST_TYPE_MSG_DATA;
	opts.mOptions = RS_TOKREQOPT_MSG_LATEST;

	std::list<RsGxsGroupId> grpIds;
	grpIds.push_back(groupId);

	std::cerr << "MeshDemocListDialog::showReprs(" << groupId << ")";
	std::cerr << std::endl;

	uint32_t token;
	mMeshDemocQueue->requestMsgInfo(token, RS_TOKREQ_ANSTYPE_DATA, opts, grpIds, TOKEN_USER_TYPE_REPR);
}

void MeshDemocListDialog::showVotesFromToken(u_int32_t token)
{
	QString messageString;
	std::multimap<RsGxsMessageId, RsGxsVote *> voteMap;
	rsMeshDemoc->getRelatedVotes(token, voteMap);
	std::multimap<RsGxsMessageId, RsGxsVote *>::iterator it;
	messageString.append(QString("start\n"));
	messageString.append(QString::number(voteMap.size()));
	messageString.append(QString(" votes \n"));

	for (it = voteMap.begin(); it != voteMap.end(); it++){
		RsGxsVote* item = it->second;
		if (item->mVoteType == GXS_VOTE_UP)
		{
			messageString.append(QString(item->mMeta.mAuthorId.c_str()));
			messageString.append(QString(" votes up @ "));
			messageString.append(QString::number(item->mMeta.mPublishTs));
			messageString.append(QString("\n"));
		}
		else
		{
			messageString.append(QString(item->mMeta.mAuthorId.c_str()));
			messageString.append(QString(" votes down @ "));
			messageString.append(QString::number(item->mMeta.mPublishTs));
			messageString.append(QString("\n"));
		}
	}
	messageString.append(QString("end\n"));
	QMessageBox::information(NULL, "votes!", messageString);
}
void MeshDemocListDialog::showReprsFromToken(u_int32_t token)
{
	QString messageString;
	std::vector<RsMeshDemocRepr> representerList;
	rsMeshDemoc->getRelatedReprs(token, representerList);
	messageString.append(QString("start\n"));
	messageString.append(QString::number(representerList.size()));
	messageString.append(QString(" representors \n"));

	std::vector<RsMeshDemocRepr>::iterator it;
	for (it = representerList.begin(); it != representerList.end(); it++){
		RsMeshDemocRepr item = *it;
			messageString.append(QString(item.mMeta.mAuthorId.c_str()));
			messageString.append(QString("-->"));
			messageString.append(QString(item.mRepresenterId.c_str()));

			messageString.append(QString("\n"));
	}
	messageString.append(QString("end\n"));
	QMessageBox::information(NULL, "representations!", messageString);
}

void MeshDemocListDialog::newPost()
{
	if(mCurrTopicId.empty())
		return;

	uint32_t subscribeFlags = ui.groupTreeWidget->subscribeFlags(QString::fromStdString(mCurrTopicId));
	bool isSubscribed = IS_GROUP_SUBSCRIBED(subscribeFlags);

	if (isSubscribed)
	{
		MeshDemocCreatePostDialog cp(mMeshDemocQueue, rsMeshDemoc, mCurrTopicId, this);
		cp.exec();
	}
}

void MeshDemocListDialog::unsubscribeTopic()
{
	std::cerr << "MeshDemocListDialog::unsubscribeTopic()";
	std::cerr << std::endl;

	if(mCurrTopicId.empty())
		return;

	uint32_t token;
	rsMeshDemoc->subscribeToGroup(token, mCurrTopicId, false);
	mMeshDemocQueue->queueRequest(token, 0, RS_TOKREQ_ANSTYPE_ACK, TOKEN_USER_TYPE_SUBSCRIBE_CHANGE);
}

void MeshDemocListDialog::subscribeTopic()
{
	std::cerr << "MeshDemocListDialog::subscribeTopic()";
	std::cerr << std::endl;

	if(mCurrTopicId.empty())
		return;

	uint32_t token;
	rsMeshDemoc->subscribeToGroup(token, mCurrTopicId, true);
	mMeshDemocQueue->queueRequest(token, 0, RS_TOKREQ_ANSTYPE_ACK, TOKEN_USER_TYPE_SUBSCRIBE_CHANGE);
}

void MeshDemocListDialog::submitVote(const RsGxsGrpMsgIdPair &msgId, bool up)
{
	/* must grab AuthorId from Layout */
	RsGxsId authorId;
	if (!ui.idChooser->getChosenId(authorId))
	{
		std::cerr << "MeshDemocListDialog::createPost() ERROR GETTING AuthorId!, Vote Failed";
		std::cerr << std::endl;

		QMessageBox::warning(this, tr("RetroShare"),tr("Please create or choose a Signing Id before Voting"), QMessageBox::Ok, QMessageBox::Ok);

		return;
	}

	RsGxsVote vote;

	vote.mMeta.mGroupId = msgId.first;
	vote.mMeta.mThreadId = msgId.second;
	vote.mMeta.mParentId = msgId.second;
	vote.mMeta.mAuthorId = authorId;

	if (up)
	{
		vote.mVoteType = GXS_VOTE_UP;
	}
	else
	{
		vote.mVoteType = GXS_VOTE_DOWN;
	}

	std::cerr << "MeshDemocListDialog::submitVote()";
	std::cerr << std::endl;

	std::cerr << "GroupId : " << vote.mMeta.mGroupId << std::endl;
	std::cerr << "ThreadId : " << vote.mMeta.mThreadId << std::endl;
	std::cerr << "ParentId : " << vote.mMeta.mParentId << std::endl;
	std::cerr << "AuthorId : " << vote.mMeta.mAuthorId << std::endl;

	uint32_t token;
	rsMeshDemoc->createVote(token, vote);
	mMeshDemocQueue->queueRequest(token, TOKENREQ_MSGINFO, RS_TOKREQ_ANSTYPE_ACK, TOKEN_USER_TYPE_VOTE);
} 

/*****************************************************************************************/
// Overloaded from FeedHolder.
QScrollArea *MeshDemocListDialog::getScrollArea()
{
	return ui.scrollArea;
}

void MeshDemocListDialog::deleteFeedItem(QWidget */*item*/, uint32_t /*type*/)
{
	std::cerr << "MeshDemocListDialog::deleteFeedItem() Nah";
	std::cerr << std::endl;
	return;
}

void MeshDemocListDialog::openChat(std::string /*peerId*/)
{
	std::cerr << "MeshDemocListDialog::openChat() Nah";
	std::cerr << std::endl;
	return;
}

void MeshDemocListDialog::openComments(uint32_t /*feed_type*/, const RsGxsGroupId &groupId, const RsGxsMessageId &msgId, const QString &title)
{
	commentLoad(groupId, msgId, title);
}

/*****************************************************************************************/

void MeshDemocListDialog::updateDisplay(bool /*complete*/)
{
	std::cerr << "rsMeshDemoc->updateDisplay()";
	std::cerr << std::endl;

	/* update List */
	insertGroups();

	std::map<RsGxsGroupId, std::vector<RsGxsMessageId> > &msgs = getMsgIds();
	if(!msgs.empty())
	{
		std::cerr << "rsMeshDemoc->msgsChanged():";
		std::cerr << std::endl;

		std::map<RsGxsGroupId, std::vector<RsGxsMessageId> >::iterator mit;
		mit = msgs.find(mCurrTopicId);
		if(mit != msgs.end())
		{
			std::cerr << "current Group -> updating Displayed Items";
			std::cerr << std::endl;
			updateDisplayedItems(mit->second);
		}
	}
}

void MeshDemocListDialog::updateDisplayedItems(const std::vector<RsGxsMessageId> &msgIds)
{
	RsTokReqOptions opts;

	opts.mReqType = GXS_REQUEST_TYPE_MSG_DATA;
	opts.mOptions = RS_TOKREQOPT_MSG_LATEST;

	GxsMsgReq msgs;
	msgs[mCurrTopicId] = msgIds;

	std::cerr << "MeshDemocListDialog::updateDisplayedItems(" << mCurrTopicId << ")";
	std::cerr << std::endl;

	std::vector<RsGxsMessageId>::const_iterator it;
	for(it = msgIds.begin(); it != msgIds.end(); it++)
	{
		std::cerr << "\t\tMsgId: " << *it;
		std::cerr << std::endl;
	}

	uint32_t token;
	mMeshDemocQueue->requestMsgInfo(token, RS_TOKREQ_ANSTYPE_DATA, opts, msgs, TOKEN_USER_TYPE_POST_MOD);
}

void MeshDemocListDialog::changedTopic(const QString &id)
{
	mCurrTopicId = id.toStdString();
	insertThreads();
}

/*********************** **** **** **** ***********************/
/** New / Edit Groups		  ********************************/
/*********************** **** **** **** ***********************/

void MeshDemocListDialog::newTopic()
{
	MeshDemocGroupDialog cf (mMeshDemocQueue, this);
	cf.exec ();
}
	
void MeshDemocListDialog::showTopic()
{
	if (mCurrTopicId.empty()) {
		return;
	}

        MeshDemocGroupDialog cf(mMeshDemocQueue, rsMeshDemoc->getTokenService(), GxsGroupDialog::MODE_SHOW, mCurrTopicId, this);
        cf.exec ();
}
	
void MeshDemocListDialog::editTopic()
{
	if (mCurrTopicId.empty()) {
		return;
	}

        MeshDemocGroupDialog cf(mMeshDemocQueue, rsMeshDemoc->getTokenService(), GxsGroupDialog::MODE_EDIT, mCurrTopicId, this);
        cf.exec ();
}
	
void MeshDemocListDialog::showGroupDetails()
{
	if (mCurrTopicId.empty()) 
	{
		return;
	}

	MeshDemocGroupDialog cf(mMeshDemocQueue, rsMeshDemoc->getTokenService(), GxsGroupDialog::MODE_SHOW, mCurrTopicId, this);
	cf.exec ();
}

void MeshDemocListDialog::insertGroups()
{
	requestGroupSummary();
}

void MeshDemocListDialog::requestGroupSummary()
{
	std::cerr << "MeshDemocListDialog::requestGroupSummary()";
	std::cerr << std::endl;

	RsTokReqOptions opts;
	opts.mReqType = GXS_REQUEST_TYPE_GROUP_META;
	uint32_t token;
	mMeshDemocQueue->requestGroupInfo(token,  RS_TOKREQ_ANSTYPE_SUMMARY, opts, TOKEN_USER_TYPE_TOPIC);

	/* refresh Id Chooser Too */
	RsGxsId currentId = "";
	ui.idChooser->getChosenId(currentId);
	ui.idChooser->loadIds(IDCHOOSER_ID_REQUIRED, currentId);
}

void MeshDemocListDialog::acknowledgeGroup(const uint32_t &token)
{
	RsGxsGroupId grpId;
	rsMeshDemoc->acknowledgeGrp(token, grpId);

	if(!grpId.empty())
	{
		RsTokReqOptions opts;
		opts.mReqType = GXS_REQUEST_TYPE_GROUP_META;
		uint32_t reqToken;
		mMeshDemocQueue->requestGroupInfo(reqToken, RS_TOKREQ_ANSTYPE_SUMMARY, opts, TOKEN_USER_TYPE_TOPIC);
	}
}

void MeshDemocListDialog::acknowledgePostMsg(const uint32_t &token)
{
	RsGxsGrpMsgIdPair msgId;

	// just acknowledge, don't load anything
	rsMeshDemoc->acknowledgeMsg(token, msgId);
}

void MeshDemocListDialog::loadGroupSummary(const uint32_t &token)
{
	std::cerr << "MeshDemocListDialog::loadGroupSummary()";
	std::cerr << std::endl;

	std::list<RsGroupMetaData> groupInfo;
	rsMeshDemoc->getGroupSummary(token, groupInfo);

	if (groupInfo.size() > 0)
	{
		insertGroupData(groupInfo);
	}
	else
	{
		std::cerr << "MeshDemocListDialog::loadGroupSummary() ERROR No Groups...";
		std::cerr << std::endl;
	}
}

void MeshDemocListDialog::loadPostData(const uint32_t &token)
{
	loadGroupThreadData_InsertThreads(token);
}

void MeshDemocListDialog::acknowledgeVoteMsg(const uint32_t &token)
{
	RsGxsGrpMsgIdPair msgId;

	rsMeshDemoc->acknowledgeVote(token, msgId);
}

void MeshDemocListDialog::loadVoteData(const uint32_t &/*token*/)
{
	return;
}

/*********************** **** **** **** ***********************/
/*********************** **** **** **** ***********************/

void MeshDemocListDialog::requestGroupSummary_CurrentForum(const std::string &forumId)
{
	RsTokReqOptions opts;
	
	std::list<std::string> grpIds;
	grpIds.push_back(forumId);

	std::cerr << "MeshDemocListDialog::requestGroupSummary_CurrentForum(" << forumId << ")";
	std::cerr << std::endl;

	uint32_t token;	
	mMeshDemocQueue->requestGroupInfo(token, RS_TOKREQ_ANSTYPE_SUMMARY, opts, grpIds, MESHDEMOCDIALOG_CURRENTFORUM);
}

void MeshDemocListDialog::loadGroupSummary_CurrentForum(const uint32_t &token)
{
	std::cerr << "MeshDemocListDialog::loadGroupSummary_CurrentForum()";
	std::cerr << std::endl;

	std::list<RsGroupMetaData> groupInfo;
	rsMeshDemoc->getGroupSummary(token, groupInfo);

	if (groupInfo.size() == 1)
	{
		RsGroupMetaData fi = groupInfo.front();
		//insertForumThreads(fi);
	}
	else
	{
		std::cerr << "MeshDemocListDialog::loadGroupSummary_CurrentForum() ERROR Invalid Number of Groups...";
		std::cerr << std::endl;
	}
}

/*********************** **** **** **** ***********************/
/*********************** **** **** **** ***********************/
/*********************** **** **** **** ***********************/
/*********************** **** **** **** ***********************/

void MeshDemocListDialog::insertThreads()
{
	loadCurrentTopicThreads(mCurrTopicId);
}

void MeshDemocListDialog::loadCurrentTopicThreads(const std::string &topicId)
{
	std::cerr << "MeshDemocListDialog::loadCurrentForumThreads(" << topicId << ")";
	std::cerr << std::endl;

	if (topicId.empty())
	{
		std::cerr << "MeshDemocListDialog::loadCurrentForumThreads() Empty GroupId .. ignoring Req";
		std::cerr << std::endl;
		return;
	}

	clearPosts();

	/* initiate loading */
	requestGroupThreadData_InsertThreads(topicId);
}

void MeshDemocListDialog::requestGroupThreadData_InsertThreads(const std::string &groupId)
{
	RsTokReqOptions opts;

	opts.mReqType = GXS_REQUEST_TYPE_MSG_DATA;
	opts.mOptions = RS_TOKREQOPT_MSG_LATEST;
	
	std::list<RsGxsGroupId> grpIds;
	grpIds.push_back(groupId);

	std::cerr << "MeshDemocListDialog::requestGroupThreadData_InsertThreads(" << groupId << ")";
	std::cerr << std::endl;

	uint32_t token;	
	mMeshDemocQueue->requestMsgInfo(token, RS_TOKREQ_ANSTYPE_DATA, opts, grpIds, TOKEN_USER_TYPE_POST);
}

void MeshDemocListDialog::loadGroupThreadData_InsertThreads(const uint32_t &token)
{
	std::cerr << "MeshDemocListDialog::loadGroupThreadData_InsertThreads()";
	std::cerr << std::endl;

	clearPosts();

	std::vector<RsMeshDemocPost> posts;
	rsMeshDemoc->getPostData(token, posts);
	std::vector<RsMeshDemocPost>::iterator vit;

	for(vit = posts.begin(); vit != posts.end(); vit++)
	{
		RsMeshDemocPost& p = *vit;
		loadPost(p);
	}

	applyRanking();
}

void MeshDemocListDialog::loadPost(const RsMeshDemocPost &post)
{
	MeshDemocItem *item = new MeshDemocItem(this, 0, post, true);
	connect(item, SIGNAL(vote(RsGxsGrpMsgIdPair,bool)), this, SLOT(submitVote(RsGxsGrpMsgIdPair,bool)));
	connect(item, SIGNAL(votesReq(RsGxsGrpMsgIdPair)), this, SLOT(showVotes(RsGxsGrpMsgIdPair)));
	mPosts.insert(post.mMeta.mMsgId, item);
	//QLayout *alayout = ui.scrollAreaWidgetContents->layout();
	//alayout->addWidget(item);
	mPostList.push_back(item);
}

bool CmpPIHot(const MeshDemocItem *a, const MeshDemocItem *b)
{
	const RsMeshDemocPost &postA = a->getPost();
	const RsMeshDemocPost &postB = b->getPost();

	if (postA.mHotScore == postB.mHotScore)
	{
		return (postA.mNewScore > postB.mNewScore);
	}

	return (postA.mHotScore > postB.mHotScore);
}

bool CmpPITop(const MeshDemocItem *a, const MeshDemocItem *b)
{
	const RsMeshDemocPost &postA = a->getPost();
	const RsMeshDemocPost &postB = b->getPost();

	if (postA.mTopScore == postB.mTopScore)
	{
		return (postA.mNewScore > postB.mNewScore);
	}

	return (postA.mTopScore > postB.mTopScore);
}

bool CmpPINew(const MeshDemocItem *a, const MeshDemocItem *b)
{
	return (a->getPost().mNewScore > b->getPost().mNewScore);
}

void MeshDemocListDialog::applyRanking()
{
	/* uses current settings to sort posts, then add to layout */
	std::cerr << "MeshDemocListDialog::applyRanking()";
	std::cerr << std::endl;

	shallowClearPosts();

	/* sort */
	switch(mSortMethod)
	{
		default:
		case RsMeshDemoc::HotRankType:
			std::cerr << "MeshDemocListDialog::applyRanking() HOT";
			std::cerr << std::endl;
			mPostList.sort(CmpPIHot);
			break;
		case RsMeshDemoc::NewRankType:
			std::cerr << "MeshDemocListDialog::applyRanking() NEW";
			std::cerr << std::endl;
			mPostList.sort(CmpPINew);
			break;
		case RsMeshDemoc::TopRankType:
			std::cerr << "MeshDemocListDialog::applyRanking() TOP";
			std::cerr << std::endl;
			mPostList.sort(CmpPITop);
			break;
	}
	mLastSortMethod = mSortMethod;

	std::cerr << "MeshDemocListDialog::applyRanking() Sorted mPostList";
	std::cerr << std::endl;

	/* go through list (skipping out-of-date items) to get */
	QLayout *alayout = ui.scrollAreaWidgetContents->layout();
	int counter = 0;
	time_t min_ts = 0;
	std::list<MeshDemocItem *>::iterator it;
	for(it = mPostList.begin(); it != mPostList.end(); it++)
	{
		MeshDemocItem *item = (*it);
		std::cerr << "MeshDemocListDialog::applyRanking() Item: " << item;
		std::cerr << std::endl;
		
		if (item->getPost().mMeta.mPublishTs < min_ts)
		{
			std::cerr << "\t Skipping OLD";
			std::cerr << std::endl;
			item->hide();
			continue;
		}

		if (counter >= mPostIndex + mPostShow)
		{
			std::cerr << "\t END - Counter too high";
			std::cerr << std::endl;
			item->hide();
		}
		else if (counter >= mPostIndex)
		{
			std::cerr << "\t Adding to Layout";
			std::cerr << std::endl;
			/* add it in! */
			alayout->addWidget(item);
			item->show();
		}
		else
		{
			std::cerr << "\t Skipping to Low";
			std::cerr << std::endl;
			item->hide();
		}
		counter++;
	}

	std::cerr << "MeshDemocListDialog::applyRanking() Loaded New Order";
	std::cerr << std::endl;

	// trigger a redraw.
	ui.scrollAreaWidgetContents->update();
}

void MeshDemocListDialog::clearPosts()
{
	std::cerr << "MeshDemocListDialog::clearPosts()" << std::endl;

	std::list<MeshDemocItem *> postedItems;
	std::list<MeshDemocItem *>::iterator pit;

	QLayout *alayout = ui.scrollAreaWidgetContents->layout();
	int count = alayout->count();
	for(int i = 0; i < count; i++)
	{
		QLayoutItem *litem = alayout->itemAt(i);
		if (!litem)
		{
			std::cerr << "MeshDemocListDialog::clearPosts() missing litem";
			std::cerr << std::endl;
			continue;
		}

		MeshDemocItem *item = dynamic_cast<MeshDemocItem *>(litem->widget());
		if (item)
		{
			std::cerr << "MeshDemocListDialog::clearPosts() item: " << item;
			std::cerr << std::endl;

			postedItems.push_back(item);
		}
		else
		{
			std::cerr << "MeshDemocListDialog::clearPosts() Found Child, which is not a MeshDemocItem???";
			std::cerr << std::endl;
		}
	}

	for(pit = postedItems.begin(); pit != postedItems.end(); pit++)
	{
		MeshDemocItem *item = *pit;
		alayout->removeWidget(item);
		delete item;
	}

	mPosts.clear();
	mPostList.clear();
}

void MeshDemocListDialog::shallowClearPosts()
{
	std::cerr << "MeshDemocListDialog::shallowClearPosts()" << std::endl;

	std::list<MeshDemocItem *> postedItems;
	std::list<MeshDemocItem *>::iterator pit;

	QLayout *alayout = ui.scrollAreaWidgetContents->layout();
	int count = alayout->count();
	for(int i = 0; i < count; i++)
	{
		QLayoutItem *litem = alayout->itemAt(i);
		if (!litem)
		{
			std::cerr << "MeshDemocListDialog::shallowClearPosts() missing litem";
			std::cerr << std::endl;
			continue;
		}

		MeshDemocItem *item = dynamic_cast<MeshDemocItem *>(litem->widget());
		if (item)
		{
			std::cerr << "MeshDemocListDialog::shallowClearPosts() item: " << item;
			std::cerr << std::endl;

			postedItems.push_back(item);
		}
		else
		{
			std::cerr << "MeshDemocListDialog::shallowClearPosts() Found Child, which is not a MeshDemocItem???";
			std::cerr << std::endl;
		}
	}

	for(pit = postedItems.begin(); pit != postedItems.end(); pit++)
	{
		MeshDemocItem *item = *pit;
		alayout->removeWidget(item);
	}
}

void MeshDemocListDialog::updateCurrentDisplayComplete(const uint32_t &token)
{
	std::cerr << "MeshDemocListDialog::updateCurrentDisplayComplete()";
	std::cerr << std::endl;

	std::vector<RsMeshDemocPost> posts;
	rsMeshDemoc->getPostData(token, posts);

	std::vector<RsMeshDemocPost>::iterator vit;
	for(vit = posts.begin(); vit != posts.end(); vit++)
	{
		RsMeshDemocPost& p = *vit;

		// modify post content
		if(mPosts.find(p.mMeta.mMsgId) != mPosts.end())
		{
			std::cerr << "MeshDemocListDialog::updateCurrentDisplayComplete() updating MsgId: " << p.mMeta.mMsgId;
			std::cerr << std::endl;

			mPosts[p.mMeta.mMsgId]->setContent(p);
		}
		else
		{
			std::cerr << "MeshDemocListDialog::updateCurrentDisplayComplete() loading New MsgId: " << p.mMeta.mMsgId;
			std::cerr << std::endl;
			/* insert new entry */
			loadPost(p);
		}
	}

	time_t now = time(NULL);
	QMap<RsGxsMessageId, MeshDemocItem*>::iterator pit;
	for(pit = mPosts.begin(); pit != mPosts.end(); pit++)
	{
		(*pit)->post().calculateScores(now);
	}

	applyRanking();
}

void MeshDemocListDialog::acknowledgeSubscribeChange(const uint32_t &token)
{
	std::cerr << "MeshDemocListDialog::acknowledgeSubscribeChange()";
	std::cerr << std::endl;

	std::vector<RsMeshDemocPost> posts;
	RsGxsGroupId groupId;
	rsMeshDemoc->acknowledgeGrp(token, groupId);

	insertGroups();
}

/*********************** **** **** **** ***********************/
/*********************** **** **** **** ***********************/
/*********************** **** **** **** ***********************/
	
void MeshDemocListDialog::loadRequest(const TokenQueue *queue, const TokenRequest &req)
{
	std::cerr << "MeshDemocListDialog::loadRequest() UserType: " << req.mUserType;
	std::cerr << std::endl;
				
	if (queue == mMeshDemocQueue)
	{
		/* now switch on req */
		switch(req.mUserType)
		{
			case TOKEN_USER_TYPE_TOPIC:
				switch(req.mAnsType)
				{
					case RS_TOKREQ_ANSTYPE_SUMMARY:
						loadGroupSummary(req.mToken);
						break;
					default:
						std::cerr << "Error, unexpected anstype:" << req.mAnsType << std::endl;
						break;
				}
				break;
			case TOKEN_USER_TYPE_POST:
				switch(req.mAnsType)
				{
					case RS_TOKREQ_ANSTYPE_ACK:
						acknowledgePostMsg(req.mToken);
						break;
					case RS_TOKREQ_ANSTYPE_DATA:
						loadPostData(req.mToken);
						break;
					default:
						std::cerr << "Error, unexpected anstype:" << req.mAnsType << std::endl;
						break;
				}
				break;
		case TOKEN_USER_TYPE_REPR:
			switch(req.mAnsType)
			{
				case RS_TOKREQ_ANSTYPE_ACK:
					acknowledgePostMsg(req.mToken);
					break;
				case RS_TOKREQ_ANSTYPE_DATA:
					showReprsFromToken(req.mToken);
					break;
				default:
					std::cerr << "Error, unexpected anstype:" << req.mAnsType << std::endl;
					break;
			}
			break;
			case TOKEN_USER_TYPE_VOTE:
				switch(req.mAnsType)
				{
					case RS_TOKREQ_ANSTYPE_ACK:
						acknowledgeVoteMsg(req.mToken);
						break;
					default:
						std::cerr << "Error, unexpected anstype:" << req.mAnsType << std::endl;
						break;
				}
				break;
			case TOKENREQ_MSGRELATEDINFO:
				switch(req.mAnsType)
				{
					case RS_TOKREQ_ANSTYPE_DATA:
						showVotesFromToken(req.mToken);
						break;
					default:
						std::cerr << "Error, unexpected anstype:" << req.mAnsType << std::endl;
						break;
				}
			case TOKEN_USER_TYPE_POST_MOD:
				switch(req.mAnsType)
				{
					case RS_TOKREQ_ANSTYPE_DATA:
						updateCurrentDisplayComplete(req.mToken);
						break;
					default:
						std::cerr << "Error, unexpected anstype:" << req.mAnsType << std::endl;
						break;
				}
				break;
			case TOKEN_USER_TYPE_POST_RANKINGS:
				switch(req.mAnsType)
				{
					case RS_TOKREQ_ANSTYPE_DATA:
						//loadRankings(req.mToken);
						break;
					default:
						std::cerr << "Error, unexpected anstype:" << req.mAnsType << std::endl;
						break;
				}
			case TOKEN_USER_TYPE_SUBSCRIBE_CHANGE:
				acknowledgeSubscribeChange(req.mToken);
				break;
			default:
				std::cerr << "MeshDemocListDialog::loadRequest() ERROR: INVALID TYPE";
				std::cerr << std::endl;
				break;
		}
	}

	/* now switch on req */
	switch(req.mType)
	{
		case TOKENREQ_GROUPINFO:
			switch(req.mAnsType)
			{
			case RS_TOKREQ_ANSTYPE_ACK:
				acknowledgeGroup(req.mToken);
				break;
			}
			break;
	}
}

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
/**************************** Groups **********************/

void MeshDemocListDialog::groupInfoToGroupItemInfo(const RsGroupMetaData &groupInfo, GroupRecurItemInfo &groupItemInfo)
{
	groupItemInfo.id = QString::fromStdString(groupInfo.mGroupId);
	groupItemInfo.name = QString::fromUtf8(groupInfo.mGroupName.c_str());
	//groupItemInfo.description = QString::fromUtf8(groupInfo.forumDesc);
	groupItemInfo.popularity = groupInfo.mPop;
	groupItemInfo.lastpost = QDateTime::fromTime_t(groupInfo.mLastPost);
	groupItemInfo.subscribeFlags = groupInfo.mSubscribeFlags;
	groupItemInfo.parentId = QString::fromStdString(groupInfo.mParentGrpId);
}

void MeshDemocListDialog::insertGroupData(const std::list<RsGroupMetaData> &groupList)
{
	std::list<RsGroupMetaData>::const_iterator it;

	QList<GroupRecurItemInfo> adminList;
	QList<GroupRecurItemInfo> subList;
	QList<GroupRecurItemInfo> allList;
	std::multimap<uint32_t, GroupRecurItemInfo> popMap;

	for (it = groupList.begin(); it != groupList.end(); it++) 
	{
		/* sort it into Publish (Own), Subscribed, Popular and Other */
		uint32_t flags = it->mSubscribeFlags;

		GroupRecurItemInfo groupItemInfo;
		groupInfoToGroupItemInfo(*it, groupItemInfo);

		if (IS_GROUP_SUBSCRIBED(flags))
		{
			if (IS_GROUP_ADMIN(flags) || IS_GROUP_PUBLISHER(flags))
			{
				adminList.push_back(groupItemInfo);
			}
			else
			{
				subList.push_back(groupItemInfo);
			}
		}
		allList.push_back(groupItemInfo);
	}


	/* now we can add them in as a tree! */
	ui.groupTreeWidget->fillGroupItems(yourTopics, adminList);
	ui.groupTreeWidget->fillGroupItems(subscribedTopics, subList);
	ui.groupTreeWidget->fillGroupItems(allTopics, allList);
}

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/

void MeshDemocListDialog::todo()
{
	QMessageBox::information(this, "Todo",
							 "<b>Open points:</b><ul>"
							 "<li>Subreddits/tag to posts support"
							 "<li>Edit Topics"
							 "<li>Picture Support"
							 "</ul>");
}
