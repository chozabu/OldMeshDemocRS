#include "SelectRepresentitiveDialog.h"
#include "ui_SelectRepresentitiveDialog.h"
#include "MeshDemocUserTypes.h"

#include <QMessageBox>

#include "gui/common/FriendSelectionWidget.h"

#include "retroshare/rsmeshdemoc.h"


#include "util/TokenQueue.h"

#include <iostream>

SelectRepresentitiveDialog::SelectRepresentitiveDialog(TokenQueue *tokenQ, RsMeshDemoc *posted, const RsGxsGroupId &grpId, QWidget *parent) :
    QDialog(parent),
    mTokenQueue(tokenQ), mMeshDemoc(posted), mGrpId(grpId),
    ui(new Ui::SelectRepresentitiveDialog)
{
	ui->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, false);

	QObject::connect(ui->select_repr_PB,SIGNAL(clicked()),this,SLOT(createRepr())) ;

	ui->friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_SINGLE) ;
	ui->friendSelectionWidget->setShowType(FriendSelectionWidget::SHOW_GPG) ;
	ui->friendSelectionWidget->setHeaderText(QObject::tr("Select your Representitive")) ;
	ui->friendSelectionWidget->start() ;


	/* fill in the available OwnIds for signing */
	ui->idChooser->loadIds(IDCHOOSER_ID_REQUIRED, "");
}


SelectRepresentitiveDialog::~SelectRepresentitiveDialog()
{
	delete ui;
}

void SelectRepresentitiveDialog::createRepr()
{

	FriendSelectionWidget::IdType type ;
	std::string current_pgp_id = ui->friendSelectionWidget->selectedId(type) ;

	RsGxsId authorId;
	if (!ui->idChooser->getChosenId(authorId))
	{
		std::cerr << "MeshDemocCreatePostDialog::createPost() ERROR GETTING AuthorId!, Post Failed";
		std::cerr << std::endl;

		QMessageBox::warning(this, tr("RetroShare"),tr("Please create or choose a Signing Id first"), QMessageBox::Ok, QMessageBox::Ok);

		return;
	}

	RsMeshDemocRepr rep;
	rep.mMeta.mGroupId = mGrpId;
	rep.mRepresenterId = current_pgp_id;
	rep.mMeta.mMsgName = std::string("repritem");
	rep.mMeta.mAuthorId = authorId;

	uint32_t token;
	//mTokenQueue->queueRequest(token, TOKENREQ_MSGINFO, RS_TOKREQ_ANSTYPE_ACK, TOKEN_USER_TYPE_POST);
	rsMeshDemoc->createRepr(token,rep);
	mTokenQueue->queueRequest(token, TOKENREQ_MSGINFO, RS_TOKREQ_ANSTYPE_ACK, TOKEN_USER_TYPE_REPR);
	QMessageBox::information(NULL,tr("Representitive chosen"),tr("Your new representitive has been chosen.")) ;

}
