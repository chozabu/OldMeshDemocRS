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

#include "MeshDemocGroupDialog.h"

#include <retroshare/rswiki.h>
#include <iostream>

const uint32_t MeshDemocCreateEnabledFlags = (
			  GXS_GROUP_FLAGS_NAME        |
			  // GXS_GROUP_FLAGS_ICON        |
                          GXS_GROUP_FLAGS_DESCRIPTION   |
                          GXS_GROUP_FLAGS_DISTRIBUTION  |
                          // GXS_GROUP_FLAGS_PUBLISHSIGN   |
                          GXS_GROUP_FLAGS_SHAREKEYS     |
                          // GXS_GROUP_FLAGS_PERSONALSIGN  |
                          // GXS_GROUP_FLAGS_COMMENTS      |
                          0);

uint32_t MeshDemocCreateDefaultsFlags = ( GXS_GROUP_DEFAULTS_DISTRIB_PUBLIC    |
                           //GXS_GROUP_DEFAULTS_DISTRIB_GROUP        |
                           //GXS_GROUP_DEFAULTS_DISTRIB_LOCAL        |

                           GXS_GROUP_DEFAULTS_PUBLISH_OPEN         |
                           //GXS_GROUP_DEFAULTS_PUBLISH_THREADS      |
                           //GXS_GROUP_DEFAULTS_PUBLISH_REQUIRED     |
                           //GXS_GROUP_DEFAULTS_PUBLISH_ENCRYPTED    |

                           //GXS_GROUP_DEFAULTS_PERSONAL_GPG         |
                           GXS_GROUP_DEFAULTS_PERSONAL_REQUIRED    |
                           //GXS_GROUP_DEFAULTS_PERSONAL_IFNOPUB     |

                           //GXS_GROUP_DEFAULTS_COMMENTS_YES         |
                           GXS_GROUP_DEFAULTS_COMMENTS_NO          |
                           0);

uint32_t MeshDemocEditEnabledFlags = MeshDemocCreateEnabledFlags;
uint32_t MeshDemocEditDefaultsFlags = MeshDemocCreateDefaultsFlags;

MeshDemocGroupDialog::MeshDemocGroupDialog(TokenQueue *tokenQueue, QWidget *parent)
	:GxsGroupDialog(tokenQueue, MeshDemocCreateEnabledFlags, MeshDemocCreateDefaultsFlags, parent)
{
}

MeshDemocGroupDialog::MeshDemocGroupDialog(TokenQueue *tokenExternalQueue, RsTokenService *tokenService, Mode mode, RsGxsGroupId groupId, QWidget *parent)
:GxsGroupDialog(tokenExternalQueue, tokenService, mode, groupId, MeshDemocEditEnabledFlags, MeshDemocEditDefaultsFlags, parent)
{
}

void MeshDemocGroupDialog::initUi()
{
	switch (mode())
	{
	case MODE_CREATE:
		setUiText(UITYPE_SERVICE_HEADER, tr("Create New Topic"));
		setUiText(UITYPE_BUTTONBOX_OK, tr("Create Topic"));
		break;
	case MODE_SHOW:
		setUiText(UITYPE_SERVICE_HEADER, tr("MeshDemoc Topic"));
		break;
	case MODE_EDIT:
		setUiText(UITYPE_SERVICE_HEADER, tr("Edit Topic"));
		setUiText(UITYPE_BUTTONBOX_OK, tr("Update Topic"));
		break;
	}

	setUiText(UITYPE_KEY_SHARE_CHECKBOX, tr("Add Topic Admins"));
	setUiText(UITYPE_CONTACTS_DOCK, tr("Select Topic Admins"));
}

QPixmap MeshDemocGroupDialog::serviceImage()
{
	return QPixmap(":/images/posted_add_64.png");
}

bool MeshDemocGroupDialog::service_CreateGroup(uint32_t &token, const RsGroupMetaData &meta)
{
	// Specific Function.
	RsMeshDemocGroup grp;
	grp.mMeta = meta;
	grp.mDescription = getDescription().toStdString();
	std::cerr << "MeshDemocGroupDialog::service_CreateGroup() storing to Queue";
	std::cerr << std::endl;

	rsMeshDemoc->createGroup(token, grp);

	return true;
}

bool MeshDemocGroupDialog::service_EditGroup(uint32_t &token,
			RsGroupMetaData &editedMeta)
{
	RsMeshDemocGroup grp;
	grp.mMeta = editedMeta;
	grp.mDescription = std::string(ui.groupDesc->toPlainText().toUtf8());

	std::cerr << "MeshDemocGroupDialog::service_EditGroup() submitting changes";
	std::cerr << std::endl;

	//rsMeshDemoc->updateGroup(token, grp);//TODO MD
	return true;
}


bool MeshDemocGroupDialog::service_loadGroup(uint32_t token, Mode mode, RsGroupMetaData& groupMetaData)
{
        std::cerr << "MeshDemocGroupDialog::service_loadGroup(" << token << ")";
        std::cerr << std::endl;

        std::vector<RsMeshDemocGroup> groups;
        if (!rsMeshDemoc->getGroupData(token, groups))
        {
                std::cerr << "MeshDemocGroupDialog::service_loadGroup() Error getting GroupData";
                std::cerr << std::endl;
                return false;
        }

        if (groups.size() != 1)
        {
                std::cerr << "MeshDemocGroupDialog::service_loadGroup() Error Group.size() != 1";
                std::cerr << std::endl;
                return false;
        }

        std::cerr << "MeshDemocGroupDialog::service_loadGroup() Unfinished Loading";
        std::cerr << std::endl;

	groupMetaData = groups[0].mMeta;
	return true;
}

