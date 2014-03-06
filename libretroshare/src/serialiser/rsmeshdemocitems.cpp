/*
 * libretroshare/src/gxs: rsopsteditems.cc
 *
 * RetroShare Serialiser.
 *
 * Copyright 2012-2013 by Robert Fernie, Christopher Evi-Parker
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

#include "serialiser/rsmeshdemocitems.h"
#include "rsbaseserial.h"
#include "rstlvbase.h"


uint32_t RsGxsMeshDemocSerialiser::size(RsItem *item)
{
#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::size()" << std::endl;
#endif

	RsGxsMeshDemocGroupItem* pgItem = NULL;
	RsGxsMeshDemocPostItem* ppItem = NULL;
	RsGxsMeshDemocRepresentationItem* prItem = NULL;

	if ((pgItem = dynamic_cast<RsGxsMeshDemocGroupItem*>(item)) != NULL)
	{
		return sizeGxsMeshDemocGroupItem(pgItem);
	}
	else if ((ppItem = dynamic_cast<RsGxsMeshDemocPostItem*>(item)) != NULL)
	{
		return sizeGxsMeshDemocPostItem(ppItem);
	}
	else if ((prItem = dynamic_cast<RsGxsMeshDemocRepresentationItem*>(item)) != NULL)
	{
		return sizeGxsMeshDemocRepresentationItem(prItem);
	}
	else
	{
		return RsGxsCommentSerialiser::size(item);
	}

	return NULL;
}

bool RsGxsMeshDemocSerialiser::serialise(RsItem *item, void *data, uint32_t *size)
{
#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::serialise()" << std::endl;
#endif

	RsGxsMeshDemocPostItem* ppItem = NULL;
	RsGxsMeshDemocGroupItem* pgItem = NULL;
	RsGxsMeshDemocRepresentationItem* prItem = NULL;

	if ((pgItem = dynamic_cast<RsGxsMeshDemocGroupItem*>(item)) != NULL)
	{
		return serialiseGxsMeshDemocGroupItem(pgItem, data, size);
	}
	else if ((ppItem = dynamic_cast<RsGxsMeshDemocPostItem*>(item)) != NULL)
	{
		return serialiseGxsMeshDemocPostItem(ppItem, data, size);
	}
	else if ((prItem = dynamic_cast<RsGxsMeshDemocRepresentationItem*>(item)) != NULL)
	{
		return serialiseGxsMeshDemocRepresentationItem(prItem, data, size);
	}
	else
	{
		return RsGxsCommentSerialiser::serialise(item, data, size);
	}
	return false;
}

RsItem* RsGxsMeshDemocSerialiser::deserialise(void *data, uint32_t *size)
{
#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::deserialise()" << std::endl;
#endif
	/* get the type and size */
	uint32_t rstype = getRsItemId(data);

	if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) ||
			(RS_SERVICE_GXSV2_TYPE_MESH_DEMOC != getRsItemService(rstype)))
	{
			std::cerr << "RsGxsMeshDemocSerialiser::deserialise() ERROR Wrong Type";
			std::cerr << std::endl;
			return NULL; /* wrong type */
	}

	switch(getRsItemSubType(rstype))
	{

		case RS_PKT_SUBTYPE_MESH_DEMOC_GRP_ITEM:
			return deserialiseGxsMeshDemocGroupItem(data, size);
			break;
	case RS_PKT_SUBTYPE_MESH_DEMOC_POST_ITEM:
		return deserialiseGxsMeshDemocPostItem(data, size);
		break;
	case RS_PKT_SUBTYPE_MESH_DEMOC_REPR_ITEM:
		return deserialiseGxsMeshDemocRepresentationItem(data, size);
		break;
		default:
			return RsGxsCommentSerialiser::deserialise(data, size);
			break;
	}
	return NULL;
}

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

void RsGxsMeshDemocRepresentationItem::clear()
{
	mPost.mRepresenterId.clear();
}

std::ostream & RsGxsMeshDemocRepresentationItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RsGxsMeshDemocRepresentationItem", indent);
        uint16_t int_Indent = indent + 2;

        printIndent(out, int_Indent);
        out << "Representer: " << mPost.mRepresenterId << std::endl;

        printRsItemEnd(out ,"RsGxsMeshDemocRepresentationItem", indent);
        return out;
}

uint32_t RsGxsMeshDemocSerialiser::sizeGxsMeshDemocRepresentationItem(RsGxsMeshDemocRepresentationItem* item)
{
	RsMeshDemocRepr& p = item->mPost;

	uint32_t s = 8;

	s += GetTlvStringSize(p.mRepresenterId);

	return s;
}

bool RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocRepresentationItem(RsGxsMeshDemocRepresentationItem* item, void* data, uint32_t *size)
{

#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem()" << std::endl;
#endif

	uint32_t tlvsize = sizeGxsMeshDemocRepresentationItem(item);
	uint32_t offset = 0;

	if(*size < tlvsize){
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem() Size too small" << std::endl;
		return false;
	}

	*size = tlvsize;

	bool ok = true;

	ok &= setRsItemHeader(data, tlvsize, item->PacketId(), tlvsize);

	/* skip the header */
	offset += 8;

	/* RsPostedPost */

	ok &= SetTlvString(data, tlvsize, &offset, 1, item->mPost.mRepresenterId);

	if(offset != tlvsize)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem() FAIL Size Error! " << std::endl;
		ok = false;
	}

#ifdef POSTED_DEBUG
	if (!ok)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem() NOK" << std::endl;
	}
#endif

	return ok;
}




RsGxsMeshDemocRepresentationItem* RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocRepresentationItem(void *data, uint32_t *size)
{

#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocRepresentationItem()" << std::endl;
#endif
	/* get the type and size */
	uint32_t rstype = getRsItemId(data);
	uint32_t rssize = getRsItemSize(data);

	uint32_t offset = 0;

	if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) ||
			(RS_SERVICE_GXSV2_TYPE_MESH_DEMOC != getRsItemService(rstype)) ||
			(RS_PKT_SUBTYPE_MESH_DEMOC_REPR_ITEM != getRsItemSubType(rstype)))
	{
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocRepresentationItem() FAIL wrong type" << std::endl;
			return NULL; /* wrong type */
	}

	if (*size < rssize)	/* check size */
	{
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocRepresentationItem() FAIL wrong size" << std::endl;
			return NULL; /* not enough data */
	}

	/* set the packet length */
	*size = rssize;

	bool ok = true;

	RsGxsMeshDemocRepresentationItem* item = new RsGxsMeshDemocRepresentationItem();
	/* skip the header */
	offset += 8;

	ok &= GetTlvString(data, rssize, &offset, 1, item->mPost.mRepresenterId);

	if (offset != rssize)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocRepresentationItem() FAIL size mismatch" << std::endl;
		/* error */
		delete item;
		return NULL;
	}

	if (!ok)
	{
#ifdef POSTED_DEBUG
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocRepresentationItem() NOK" << std::endl;
#endif
		delete item;
		return NULL;
	}

	return item;
}

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

void RsGxsMeshDemocPostItem::clear()
{
	mPost.mLink.clear();
	mPost.mNotes.clear();
}

std::ostream & RsGxsMeshDemocPostItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RsGxsMeshDemocPostItem", indent);
        uint16_t int_Indent = indent + 2;

        printIndent(out, int_Indent);
        out << "Link: " << mPost.mLink << std::endl;
        printIndent(out, int_Indent);
        out << "Notes: " << mPost.mNotes << std::endl;

        printRsItemEnd(out ,"RsGxsMeshDemocPostItem", indent);
        return out;
}

uint32_t RsGxsMeshDemocSerialiser::sizeGxsMeshDemocPostItem(RsGxsMeshDemocPostItem* item)
{
	RsMeshDemocPost& p = item->mPost;

	uint32_t s = 8;

	s += GetTlvStringSize(p.mLink);
	s += GetTlvStringSize(p.mNotes);

	return s;
}

bool RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem(RsGxsMeshDemocPostItem* item, void* data, uint32_t *size)
{

#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem()" << std::endl;
#endif

	uint32_t tlvsize = sizeGxsMeshDemocPostItem(item);
	uint32_t offset = 0;

	if(*size < tlvsize){
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem() Size too small" << std::endl;
		return false;
	}

	*size = tlvsize;

	bool ok = true;

	ok &= setRsItemHeader(data, tlvsize, item->PacketId(), tlvsize);

	/* skip the header */
	offset += 8;

	/* RsPostedPost */

	ok &= SetTlvString(data, tlvsize, &offset, 1, item->mPost.mLink);
	ok &= SetTlvString(data, tlvsize, &offset, 1, item->mPost.mNotes);

	if(offset != tlvsize)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem() FAIL Size Error! " << std::endl;
		ok = false;
	}

#ifdef POSTED_DEBUG
	if (!ok)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocPostItem() NOK" << std::endl;
	}
#endif

	return ok;
}




RsGxsMeshDemocPostItem* RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocPostItem(void *data, uint32_t *size)
{

#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocPostItem()" << std::endl;
#endif
	/* get the type and size */
	uint32_t rstype = getRsItemId(data);
	uint32_t rssize = getRsItemSize(data);

	uint32_t offset = 0;

	if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) ||
			(RS_SERVICE_GXSV2_TYPE_MESH_DEMOC != getRsItemService(rstype)) ||
			(RS_PKT_SUBTYPE_MESH_DEMOC_POST_ITEM != getRsItemSubType(rstype)))
	{
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocPostItem() FAIL wrong type" << std::endl;
			return NULL; /* wrong type */
	}

	if (*size < rssize)	/* check size */
	{
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocPostItem() FAIL wrong size" << std::endl;
			return NULL; /* not enough data */
	}

	/* set the packet length */
	*size = rssize;

	bool ok = true;

	RsGxsMeshDemocPostItem* item = new RsGxsMeshDemocPostItem();
	/* skip the header */
	offset += 8;

	ok &= GetTlvString(data, rssize, &offset, 1, item->mPost.mLink);
	ok &= GetTlvString(data, rssize, &offset, 1, item->mPost.mNotes);

	if (offset != rssize)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocPostItem() FAIL size mismatch" << std::endl;
		/* error */
		delete item;
		return NULL;
	}

	if (!ok)
	{
#ifdef POSTED_DEBUG
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocPostItem() NOK" << std::endl;
#endif
		delete item;
		return NULL;
	}

	return item;
}


/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

void RsGxsMeshDemocGroupItem::clear()
{
	mGroup.mDescription.clear();
	return;
}

std::ostream & RsGxsMeshDemocGroupItem::print(std::ostream &out, uint16_t indent)
{
        printRsItemBase(out, "RsGxsMeshDemocGroupItem", indent);
        uint16_t int_Indent = indent + 2;

        printIndent(out, int_Indent);
        out << "Description: " << mGroup.mDescription << std::endl;

        printRsItemEnd(out ,"RsGxsMeshDemocGroupItem", indent);
        return out;
}


uint32_t RsGxsMeshDemocSerialiser::sizeGxsMeshDemocGroupItem(RsGxsMeshDemocGroupItem* item)
{
	RsMeshDemocGroup& g = item->mGroup;
	uint32_t s = 8; // header

	s += GetTlvStringSize(g.mDescription);

	return s;
}

bool RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocGroupItem(RsGxsMeshDemocGroupItem* item, void* data, uint32_t *size)
{

#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocGroupItem()" << std::endl;
#endif

	uint32_t tlvsize = sizeGxsMeshDemocGroupItem(item);
	uint32_t offset = 0;

	if(*size < tlvsize){
#ifdef POSTED_DEBUG
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocGroupItem()" << std::endl;
#endif
		return false;
	}

	*size = tlvsize;

	bool ok = true;

	ok &= setRsItemHeader(data, tlvsize, item->PacketId(), tlvsize);

	/* skip the header */
	offset += 8;

	/* PostedGroupItem */
	ok &= SetTlvString(data, tlvsize, &offset, 1, item->mGroup.mDescription);


	if(offset != tlvsize)
	{
#ifdef POSTED_DEBUG
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocGroupItem() FAIL Size Error! " << std::endl;
#endif
		ok = false;
	}

#ifdef POSTED_DEBUG
	if (!ok)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::serialiseGxsMeshDemocGroupItem() NOK" << std::endl;
	}
#endif

	return ok;
}
RsGxsMeshDemocGroupItem* RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocGroupItem(void *data, uint32_t *size)
{

#ifdef POSTED_DEBUG
	std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocGroupItem()" << std::endl;
#endif
	/* get the type and size */
	uint32_t rstype = getRsItemId(data);
	uint32_t rssize = getRsItemSize(data);

	uint32_t offset = 0;


	if ((RS_PKT_VERSION_SERVICE != getRsItemVersion(rstype)) ||
			(RS_SERVICE_GXSV2_TYPE_MESH_DEMOC != getRsItemService(rstype)) ||
			(RS_PKT_SUBTYPE_MESH_DEMOC_GRP_ITEM != getRsItemSubType(rstype)))
	{
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocGroupItem() FAIL wrong type" << std::endl;
			return NULL; /* wrong type */
	}

	if (*size < rssize)	/* check size */
	{
			std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocGroupItem() FAIL wrong size" << std::endl;
			return NULL; /* not enough data */
	}

	/* set the packet length */
	*size = rssize;

	bool ok = true;

	RsGxsMeshDemocGroupItem* item = new RsGxsMeshDemocGroupItem();
	/* skip the header */
	offset += 8;

	ok &= GetTlvString(data, rssize, &offset, 1, item->mGroup.mDescription);

	if (offset != rssize)
	{
		std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocGroupItem() FAIL size mismatch" << std::endl;
		/* error */
		delete item;
		return NULL;
	}

	if (!ok)
	{
#ifdef POSTED_DEBUG
		std::cerr << "RsGxsMeshDemocSerialiser::deserialiseGxsMeshDemocGroupItem() NOK" << std::endl;
#endif
		delete item;
		return NULL;
	}

	return item;
}

