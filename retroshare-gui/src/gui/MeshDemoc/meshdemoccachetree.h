#ifndef MESHDEMOCCACHETREE_H
#define MESHDEMOCCACHETREE_H

#include "retroshare/rsmeshdemoc.h"

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <QVariantMap>
#include <QVariantList>
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
	std::string topicName;
  TopicVoteCache(groupId parentIn, std::string nameIn){
    parentId = parentIn;
	topicName = nameIn;
  }
};

typedef map<groupId, TopicVoteCache*> TopicDict;

class VoteCache{
  TopicDict topicDict;
  std::set<gxsId> directVoted;
  std::set<gxsId> liquidVoted;
public:
  bool addTopic(groupId topicID, groupId parentID, std::string topicName);
  //bool addReps(groupId topicID,RepMap repMap);
  void convertAddReps(gxsIdReprMmap repMap);
  QVariantMap *getQMap(groupId vTopic, msgVoteMmap voteMap);
  int getLiquidQMap(gxsId voterID, groupId topicID, QVariantList &nodes, QVariantList &links, QHash<QString, int> &nodemap);


  int getLiquidVotes(groupId vTopic, msgVoteMmap voteMap);

  int getLiquidVote(gxsId voterID, groupId topicID);
  QString getQTopicName(std::string topicID);
  std::string getTopicParent(std::string topicID);
};


#endif // MESHDEMOCCACHETREE_H
