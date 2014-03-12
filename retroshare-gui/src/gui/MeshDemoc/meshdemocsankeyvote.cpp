#include "meshdemocsankeyvote.h"

#include <qwebframe.h>

MeshDemocSankeyVote::MeshDemocSankeyVote(QWidget *parent) :
    QWebView(parent)
{
	connect( this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),    this,   SLOT(onJavaScriptWindowObjectCleared()) );

	//this->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
	//connect( this->page(), SIGNAL(linkClicked(const QUrl & )),    this,   SLOT(onLinkClicked(const QUrl & )) );
	//settings()->enablePersistentStorage();

	//connect(NotifyQt::getInstance(), SIGNAL(downloadComplete(QString)), this, SLOT(onDownloadComplete(QString)));


}


void MeshDemocSankeyVote::setVoteData(QVariantMap& voteData){
	voteInfo = &voteData;
}
QVariantMap MeshDemocSankeyVote::getVoteData(){
	return (*voteInfo);
}

void MeshDemocSankeyVote::onJavaScriptWindowObjectCleared()
{
    QWebFrame *frame = this->page()->mainFrame();
    //frame->setZoomFactor(4);

	frame->addToJavaScriptWindowObject("webview", this);

	//frame->addToJavaScriptWindowObject("bridge", eBridge);
}
