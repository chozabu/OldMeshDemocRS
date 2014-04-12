#include "meshdemoccachetree.h"

#include "gui/gxs/GxsIdDetails.h"


bool VoteCache::addTopic(groupId topicID, groupId parentID, std::string topicName){

	TopicDict::iterator ti = topicDict.find(topicID);
	if (ti == topicDict.end()){
		topicDict[topicID] = new TopicVoteCache(parentID, topicName);
	}
}

void VoteCache::convertAddReps(gxsIdReprMmap repMap){
	gxsIdReprMmap::iterator bmit = repMap.begin();
	for(; bmit != repMap.end(); bmit++)
	{
		RsMeshDemocRepr item = *(bmit->second);

		TopicDict::iterator ti = topicDict.find(item.mMeta.mGroupId);
		if (ti != topicDict.end()){
			//topicScore+=getLiquidVote(voterID, topicDict[topicID]->parentId);
			TopicVoteCache* tvc = ti->second;
			RepMap::iterator repi = tvc->representerMap.find(item.mRepresenterId);
			if (repi == tvc->representerMap.end()){
				tvc->representerMap[item.mRepresenterId] = representerTopicInfo();//(item.mRepresenterId);
			}
			tvc->representerMap[item.mRepresenterId].representees[item.mMeta.mAuthorId] = 1;//use timestamp, eh?
		}

	}
}


QVariantMap* VoteCache::getQMap(groupId vTopic, msgVoteMmap voteMap){
	QVariantMap* qm = new QVariantMap();
	QVariantList nodes;
	QHash<QString, int> nodemap;
	QVariantList links;

	QVariantMap noNode;
	noNode.insert(QString::fromStdString("name"),"No");
	nodes.append(noNode);
	QVariantMap yesNode;
	yesNode.insert(QString::fromStdString("name"),"Yes");
	nodes.append(yesNode);

	directVoted.clear();
	liquidVoted.clear();
	int directscore = 0;
	int liquidscore = 0;

	//count direct votes
	msgVoteMmap::iterator it;
	for (it = voteMap.begin(); it != voteMap.end(); it++){
		RsGxsVoteItem* item = it->second;
		int vval = item->mMsg.mVoteType == GXS_VOTE_UP ? 1 : -1;
		directscore+=vval;
		directVoted.insert(item->meta.mAuthorId);


		QString authorStr;
		std::list<QIcon> icons;
		GxsIdDetails::MakeIdDesc(item->meta.mAuthorId, false, authorStr, icons);

		QVariantMap node;
		node.insert("name",authorStr);
		nodes.append(node);

		nodemap.insert(QString::fromStdString(item->meta.mAuthorId),nodes.size()-1);
	}
	std::cerr << "directscore" << directscore << std::endl;


	//count representees recursivly
	for (it = voteMap.begin(); it != voteMap.end(); it++){
		std::cerr << "votemapITEM" << std::endl;
		RsGxsVoteItem* item = it->second;
		int vval = -1;if (item->mMsg.mVoteType == GXS_VOTE_UP) vval = 1;
		int thisscore = vval*getLiquidQMap(it->second->meta.mAuthorId, vTopic, nodes, links, nodemap);
		liquidscore+=thisscore;

		int ynIndex = item->mMsg.mVoteType == GXS_VOTE_UP ? 1 : 0;
		QVariantMap link;
		link.insert("source",nodemap[QString::fromStdString(item->meta.mAuthorId)]);
		link.insert("target",ynIndex);
		link.insert("value",thisscore+1);
		link.insert("topic",QString::fromStdString(topicDict[item->meta.mGroupId]->topicName));
		links.append(link);
	}

	qm->insert("nodes", nodes);
	qm->insert("links", links);
	return qm;
}

int VoteCache::getLiquidQMap(gxsId voterID, groupId topicID, QVariantList& nodes, QVariantList& links, QHash<QString, int>& nodemap){
	int topicScore=0;
	bool addedNode = false;
	TopicDict::iterator tv = topicDict.find(topicID);
	TopicVoteCache *tvc = tv->second;
	if (tv != topicDict.end()){
		TopicDict::iterator tp = topicDict.find(tvc->parentId);
		if (tp != topicDict.end()){
			topicScore+=getLiquidQMap(voterID, topicDict[topicID]->parentId, nodes, links, nodemap);
		}

		if(tvc->representerMap.find(voterID) == tvc->representerMap.end()){
			std::cerr << "\n\nREPRESENTER NOT found \n\n";
		} else {
			std::cerr << "\n representer FOUND\n";

		}
		RepTeeMap representees = tvc->representerMap[voterID].representees;
		std::cerr << "\n representees count:\n" << representees.size()<< "\n";
		RepTeeMap::iterator ri = representees.begin();
		for (;ri != representees.end(); ri++){//for(representee in tvc->representerMap.find(voterID)){
			gxsId representee = ri->first;
			if (directVoted.count(representee) > 0) continue;
			if (liquidVoted.count(representee) > 0){
				//std::cerr << "Representee: "+representee+" already counted, skipping (check timestamps?)" << std::endl;
				continue;
			}
			topicScore+=1;
			liquidVoted.insert(representee);
			if (tp != topicDict.end())
				topicScore+=getLiquidQMap(representee, tvc->parentId, nodes, links, nodemap);

			QString authorStr;
			std::list<QIcon> icons;
			GxsIdDetails::MakeIdDesc(representee, false, authorStr, icons);

			QVariantMap node;
			node.insert("name",authorStr);

			nodes.append(node);
			nodemap.insert(QString::fromStdString(representee),nodes.size()-1);
			topicScore+=getLiquidQMap(representee, topicID, nodes, links, nodemap);
			//should investigate and improve recursive vote counting.

			QVariantMap link;
			link.insert(QString::fromStdString("source"),nodemap[QString::fromStdString(representee)]);
			link.insert(QString::fromStdString("target"),nodemap[QString::fromStdString(voterID)]);
			link.insert(QString::fromStdString("value"),topicScore);
			links.append(link);


		}
	}else{
		std::cout << "TOPIC NOT FOUND \n";
	}
	return topicScore;
}

int VoteCache::getLiquidVotes(groupId vTopic,msgVoteMmap voteMap){
	std::cerr << "" << std::endl;
	std::cerr << "Counting votes in " << vTopic << std::endl;
	//reset for recount
	directVoted.clear();
	liquidVoted.clear();
	int directscore = 0;
	int liquidscore = 0;

	//count direct votes
	msgVoteMmap::iterator it;
	for (it = voteMap.begin(); it != voteMap.end(); it++){
		RsGxsVoteItem* item = it->second;
		int vval = -1;if (item->mMsg.mVoteType == GXS_VOTE_UP) vval = 1;
		directscore+=vval;
		directVoted.insert(item->meta.mAuthorId);
	}
	std::cerr << "directscore" << directscore << std::endl;


	//count representees recursivly
	for (it = voteMap.begin(); it != voteMap.end(); it++){
		std::cerr << "votemapITEM" << std::endl;
		RsGxsVoteItem* item = it->second;
		int vval = -1;if (item->mMsg.mVoteType == GXS_VOTE_UP) vval = 1;
		liquidscore+=vval*getLiquidVote(it->second->meta.mAuthorId, vTopic);
	}

	std::cerr << "liquidscore "<<liquidscore << std::endl;

	int finalscore = directscore+liquidscore;
	std::cerr << "FINALSCORE "<<finalscore << std::endl;
	return finalscore;
}

int VoteCache::getLiquidVote(gxsId voterID, groupId topicID){
	int topicScore=0;
	TopicDict::iterator tv = topicDict.find(topicID);
	TopicVoteCache *tvc = tv->second;
	if (tv != topicDict.end()){
		TopicDict::iterator tp = topicDict.find(tvc->parentId);
		if (tp != topicDict.end()){
			topicScore+=getLiquidVote(voterID, topicDict[topicID]->parentId);
		}

		if(tvc->representerMap.find(voterID) == tvc->representerMap.end()){
			std::cerr << "\n\nREPRESENTER NOT found \n\n";
		} else {
			std::cerr << "\n representer FOUND\n";

		}
		RepTeeMap representees = tvc->representerMap[voterID].representees;
		std::cerr << "\n representees count:\n" << representees.size()<< "\n";
		RepTeeMap::iterator ri = representees.begin();
		for (;ri != representees.end(); ri++){//for(representee in tvc->representerMap.find(voterID)){
			gxsId representee = ri->first;
			if (directVoted.count(representee) > 0) continue;
			if (liquidVoted.count(representee) > 0){
				//std::cerr << "Representee: "+representee+" already counted, skipping (check timestamps?)" << std::endl;
				continue;
			}
			topicScore+=1;
			liquidVoted.insert(representee);
			if (tp != topicDict.end())
				topicScore+=getLiquidVote(representee, tvc->parentId);

			topicScore+=getLiquidVote(representee, topicID);

		}
	}else{
		std::cout << "TOPIC NOT FOUND \n";
	}
	return topicScore;
}





