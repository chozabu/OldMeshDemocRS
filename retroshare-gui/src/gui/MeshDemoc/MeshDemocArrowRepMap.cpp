#include "MeshDemocArrowRepMap.h"

#include <qwebframe.h>

MeshDemocArrowRepMap::MeshDemocArrowRepMap(QWidget *parent) :
    QWebView(parent)
{
	connect( this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),    this,   SLOT(onJavaScriptWindowObjectCleared()) );
}


void MeshDemocArrowRepMap::setReprData(QVariantMap& repData){
	repInfo = &repData;
}
QVariantMap MeshDemocArrowRepMap::getRepData(){
	return (*repInfo);
}

void MeshDemocArrowRepMap::onJavaScriptWindowObjectCleared()
{
    QWebFrame *frame = this->page()->mainFrame();
	frame->addToJavaScriptWindowObject("webview", this);
}
