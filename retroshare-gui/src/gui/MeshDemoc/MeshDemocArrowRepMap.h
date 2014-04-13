#ifndef MESHDEMOCARROWREPMAP_H
#define MESHDEMOCARROWREPMAP_H

#include <QWebView>
#include <QVariantMap>

class MeshDemocArrowRepMap : public QWebView
{
	Q_OBJECT
public:
	explicit MeshDemocArrowRepMap(QWidget *parent = 0);
	void setReprData(QVariantMap &repData);
	//void showVoteData();
	QVariantMap* repInfo;

public slots:
    void onJavaScriptWindowObjectCleared();
	QVariantMap getRepData();

};

#endif // MESHDEMOCARROWREPMAP_H
