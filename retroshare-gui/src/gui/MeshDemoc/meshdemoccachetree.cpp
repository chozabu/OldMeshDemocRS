#include "meshdemoccachetree.h"

bool VoteCache::addTopic(groupId topicID, groupId parentID){

	TopicDict::iterator ti = topicDict.find(topicID);
	if (ti == topicDict.end()){
		topicDict[topicID] = new TopicVoteCache(parentID);
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


		}
	}else{
		std::cout << "TOPIC NOT FOUND \n";
	}
	return topicScore;
}





