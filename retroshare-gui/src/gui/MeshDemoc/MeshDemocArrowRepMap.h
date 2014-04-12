#ifndef MESHDEMOCARROWREPMAP_H
#define MESHDEMOCARROWREPMAP_H

#include <QWebView>
#include <QVariantMap>
#include <QVariantList>

class MeshDemocArrowRepMap : public QWebView
{
	Q_OBJECT
public:
	explicit MeshDemocArrowRepMap(QWidget *parent = 0);
	void setReprData(QVariantList &repData);
	//void showVoteData();
	QVariantList repInfo;

public slots:
    void onJavaScriptWindowObjectCleared();
	QVariantList getRepData();

};

#endif // MESHDEMOCARROWREPMAP_H
