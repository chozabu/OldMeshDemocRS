#ifndef SELECTREPRESENTITIVEDIALOG_H
#define SELECTREPRESENTITIVEDIALOG_H

#include <QDialog>
#include "retroshare/rsmeshdemoc.h"

class TokenQueue;

namespace Ui {
class SelectRepresentitiveDialog;
}

class SelectRepresentitiveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SelectRepresentitiveDialog(TokenQueue* tokenQ, RsMeshDemoc* posted, const RsGxsGroupId& grpId, QWidget *parent = 0);
	~SelectRepresentitiveDialog();

private slots:
	void createRepr();
private:
	Ui::SelectRepresentitiveDialog *ui;
	TokenQueue* mTokenQueue;
	RsMeshDemoc* mMeshDemoc;
	RsGxsGroupId mGrpId;
};

#endif // SELECTREPRESENTITIVEDIALOG_H
