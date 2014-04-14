#ifndef MESHDEMOCWEBVIEW_H
#define MESHDEMOCWEBVIEW_H

#include <QWebView>
#include <QVariantMap>

class MeshDemocWebView : public QWebView
{
	Q_OBJECT
public:
	explicit MeshDemocWebView(QWidget *parent = 0);
	void setPayLoad(QVariantMap &payLoadIn);
	QVariantMap* payLoad;

public slots:
    void onJavaScriptWindowObjectCleared();
	QVariantMap getPayLoad();
};

#endif // MESHDEMOCWEBVIEW_H
