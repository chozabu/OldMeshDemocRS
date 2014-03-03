#include "SelectRepresentitiveDialog.h"
#include "ui_SelectRepresentitiveDialog.h"

#include <QMessageBox>

#include "gui/common/FriendSelectionWidget.h"

#include "retroshare/rsmeshdemoc.h"


#include "util/TokenQueue.h"

SelectRepresentitiveDialog::SelectRepresentitiveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectRepresentitiveDialog)
{
	ui->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, false);

	QObject::connect(ui->select_repr_PB,SIGNAL(clicked()),this,SLOT(createRepr())) ;

	ui->friendSelectionWidget->setModus(FriendSelectionWidget::MODUS_SINGLE) ;
	ui->friendSelectionWidget->setShowType(FriendSelectionWidget::SHOW_GPG) ;
	ui->friendSelectionWidget->setHeaderText(QObject::tr("Select your Representitive")) ;
	ui->friendSelectionWidget->start() ;
}


SelectRepresentitiveDialog::~SelectRepresentitiveDialog()
{
	delete ui;
}

void SelectRepresentitiveDialog::createRepr()
{

	FriendSelectionWidget::IdType type ;
	std::string current_pgp_id = ui->friendSelectionWidget->selectedId(type) ;

	RsMeshDemocRepr rep;
	rep.mRepresenterId = current_pgp_id;

	uint32_t token;
	//mTokenQueue->queueRequest(token, TOKENREQ_MSGINFO, RS_TOKREQ_ANSTYPE_ACK, TOKEN_USER_TYPE_POST);
	rsMeshDemoc->createRepr(token,rep);
	//mTokenQueue->queueRequest(token, TOKENREQ_MSGINFO, RS_TOKREQ_ANSTYPE_ACK, TOKEN);
	QMessageBox::information(NULL,tr("Representitive chosen"),tr("Your new representitive has been chosen.")) ;

}
