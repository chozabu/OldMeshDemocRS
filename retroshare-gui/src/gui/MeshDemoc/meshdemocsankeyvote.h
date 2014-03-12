#ifndef MESHDEMOCSANKEYVOTE_H
#define MESHDEMOCSANKEYVOTE_H

#include <QWebView>
#include <QVariantMap>

class MeshDemocSankeyVote : public QWebView
{
	Q_OBJECT
public:
	explicit MeshDemocSankeyVote(QWidget *parent = 0);
	void setVoteData(QVariantMap &voteData);
	//void showVoteData();
	QVariantMap* voteInfo;

public slots:
    void onJavaScriptWindowObjectCleared();
	QVariantMap getVoteData();

};

#endif // MESHDEMOCSANKEYVOTE_H
