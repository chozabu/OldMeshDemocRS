#ifndef SELECTREPRESENTITIVEDIALOG_H
#define SELECTREPRESENTITIVEDIALOG_H

#include <QDialog>

class TokenQueue;

namespace Ui {
class SelectRepresentitiveDialog;
}

class SelectRepresentitiveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SelectRepresentitiveDialog(QWidget *parent = 0);
	~SelectRepresentitiveDialog();

private slots:
	void createRepr();
private:
	Ui::SelectRepresentitiveDialog *ui;
	TokenQueue* mTokenQueue;
};

#endif // SELECTREPRESENTITIVEDIALOG_H
