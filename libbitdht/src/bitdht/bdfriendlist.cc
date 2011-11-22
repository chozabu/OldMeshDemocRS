
/*
 * bitdht/bdfriendlist.cc
 *
 * BitDHT: An Flexible DHT library.
 *
 * Copyright 2011 by Robert Fernie
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License Version 3 as published by the Free Software Foundation.
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
 * Please report all bugs and problems to "bitdht@lunamutt.com".
 *
 */

#include "bitdht/bdfriendlist.h"
#include "bitdht/bdstddht.h"
#include "bitdht/bdpeer.h"

#include <iostream>


bdFriendEntry::bdFriendEntry()
{
	mFlags = 0;
	mLastSeen = 0;
}

bool bdFriendEntry::addrKnown(struct sockaddr_in *addr)
{
	if (mFlags & BD_FRIEND_ENTRY_ADDR_OK)
	{
		if (mFlags & BD_FRIEND_ENTRY_ONLINE)
		{
			*addr = mPeerId.addr;
			return true;
		}

		
		if (time(NULL) - mLastSeen < BD_FRIEND_ENTRY_TIMEOUT)
		{
			*addr = mPeerId.addr;
			return true;
		}
	}
	return false;
}

uint32_t bdFriendEntry::getPeerFlags()
{
	return mFlags & BD_FRIEND_ENTRY_MASK_KNOWN;
}


bdFriendList::bdFriendList(const bdNodeId *ownId)
{
	bdId tmpId;
	tmpId.id = *ownId;
	updatePeer(&tmpId, BD_FRIEND_ENTRY_SELF);
}

/******
 * Simple logic: timestamp is set with address.
 * if ONLINE, then address will be dropped as soon as OFFLINE.
 * if ADDR_OK, then address will be dropped after XX seconds.
 *
 * ONLINE - will be used with friends.
 * ADDR_OK - will potentially be used for friends of friends (but not for now).
 *****/

	/* catch-all interface function */
bool	bdFriendList::updatePeer(const bdId *id, uint32_t flags)
{
	std::cerr << "bdFriendList::updatePeer() Peer(";
	bdStdPrintId(std::cerr, id);
	std::cerr << ") Flags: " << flags;
	std::cerr << std::endl;

	std::map<bdNodeId, bdFriendEntry>::iterator it;
	it = mPeers.find(id->id);
	if (it == mPeers.end())
	{
		bdFriendEntry entry;
		entry.mPeerId.id = id->id;
		entry.mFlags = 0;
		entry.mLastSeen = 0;

		mPeers[id->id] = entry;
		it = mPeers.find(id->id);
	}

	/* update all */
	it->second.mFlags = flags;
	if (it->second.mFlags & (BD_FRIEND_ENTRY_ADDR_OK | BD_FRIEND_ENTRY_ONLINE))
	{
		it->second.mFlags |= BD_FRIEND_ENTRY_ADDR_OK;

		it->second.mPeerId.addr = id->addr;
		it->second.mLastSeen = time(NULL);
	}
	return true;
}

bool	bdFriendList::removePeer(const bdNodeId *id)
{
	/* see if it exists... */
	std::map<bdNodeId, bdFriendEntry>::iterator it;
	it = mPeers.find(*id);
	if (it == mPeers.end())
	{
		std::cerr << "bdFriendList::removeFriend() Peer(";
		bdStdPrintNodeId(std::cerr, id);
		std::cerr << ") is unknown!";
		std::cerr << std::endl;

		return false;
	}

	mPeers.erase(*id);

	return true;
}

bool	bdFriendList::findPeerEntry(const bdNodeId *id, bdFriendEntry &entry)
{
	/* see if it exists... */
	std::map<bdNodeId, bdFriendEntry>::iterator it;
	it = mPeers.find(*id);
	if (it == mPeers.end())
	{
		std::cerr << "bdFriendList::getPeerEntry() Peer(";
		bdStdPrintNodeId(std::cerr, id);
		std::cerr << ") is unknown!";
		std::cerr << std::endl;

		return false;
	}

	entry = it->second;


	return true;
}


bool	bdFriendList::print(std::ostream &out)
{
	time_t now = time(NULL);

	out << "bdFriendList::print()";
	out << std::endl;

	std::map<bdNodeId, bdFriendEntry>::iterator it;
	for(it = mPeers.begin(); it != mPeers.end(); it++)
	{
		bdStdPrintId(out, &(it->second.mPeerId));
		out << " Flags: " << it->second.mFlags;
		out << " Seen: " << now - it->second.mLastSeen;
		out << std::endl;
	}

	return true;
}



