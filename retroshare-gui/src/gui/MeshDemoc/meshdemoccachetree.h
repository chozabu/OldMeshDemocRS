#ifndef MESHDEMOCCACHETREE_H
#define MESHDEMOCCACHETREE_H

#include "retroshare/rsmeshdemoc.h"

#include <iostream>
#include <string>
#include <map>
#include <set>
using namespace std;

typedef std::string gxsId;
typedef std::string postId;
typedef std::string groupId;


typedef std::map<gxsId, int> RepTeeMap;

class representerTopicInfo{
public:
  gxsId representor;
  RepTeeMap representees;
};

typedef std::map<gxsId, representerTopicInfo> RepMap;


class TopicVoteCache{
public:
	RepMap representerMap;
    groupId parentId;
  TopicVoteCache(groupId parentIn){
    parentId = parentIn;
  }
};

typedef map<groupId, TopicVoteCache*> TopicDict;

class VoteCache{
  TopicDict topicDict;
  std::set<gxsId> directVoted;
  std::set<gxsId> liquidVoted;
public:
  bool addTopic(groupId topicID, groupId parentID);
  //bool addReps(groupId topicID,RepMap repMap);
  void convertAddReps(gxsIdReprMmap repMap);


  int getLiquidVotes(groupId vTopic,msgVoteMmap voteMap);

  int getLiquidVote(gxsId voterID, groupId topicID);
};


#endif // MESHDEMOCCACHETREE_H
