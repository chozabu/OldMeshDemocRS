#include "MeshDemocWebView.h"

#include <qwebframe.h>

MeshDemocWebView::MeshDemocWebView(QWidget *parent):
    QWebView(parent)
{
	connect( this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),    this,   SLOT(onJavaScriptWindowObjectCleared()) );
}

void MeshDemocWebView::setPayLoad(QVariantMap &payLoadIn){
	payLoad = &payLoadIn;
}
QVariantMap MeshDemocWebView::getPayLoad(){
	return (*payLoad);
}

void MeshDemocWebView::onJavaScriptWindowObjectCleared()
{
    QWebFrame *frame = this->page()->mainFrame();
	frame->addToJavaScriptWindowObject("webview", this);
}
