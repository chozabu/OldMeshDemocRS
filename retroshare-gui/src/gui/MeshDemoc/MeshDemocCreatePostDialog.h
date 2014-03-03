/*
 * Retroshare MeshDemoc
 *
 * Copyright 2012-2013 by Robert Fernie.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License Version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Please report all bugs and problems to "retroshare@lunamutt.com".
 *
 */

#ifndef MESHDEMOCCREATEPOSTDIALOG_H
#define MESHDEMOCCREATEPOSTDIALOG_H

#include <QDialog>
#include "retroshare/rsmeshdemoc.h"

class TokenQueue;

namespace Ui {
	class MeshDemocCreatePostDialog;
}

class MeshDemocCreatePostDialog : public QDialog
{
    Q_OBJECT

public:
	/*!
	 * @param tokenQ parent callee token
	 * @param posted
	 */
	explicit MeshDemocCreatePostDialog(TokenQueue* tokenQ, RsMeshDemoc* posted, const RsGxsGroupId& grpId, QWidget *parent = 0);
	~MeshDemocCreatePostDialog();

private slots:
	void createPost();

private:
	QString mLink;
	QString mNotes;
	TokenQueue* mTokenQueue;
	RsMeshDemoc* mMeshDemoc;
	RsGxsGroupId mGrpId;

	Ui::MeshDemocCreatePostDialog *ui;
};

#endif // MESHDEMOCCREATEPOSTDIALOG_H
