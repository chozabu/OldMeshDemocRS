/***************************************************************************
 *   Copyright (C) 2009                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <QStringList>
#include <QRegExp>
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include <QDesktopServices>
#include <QMessageBox>
#include <QIcon>
#include <QObject>

#include "RetroShareLink.h"
#include "MainWindow.h"
#include "ForumsDialog.h"
#include "ChannelFeed.h"
#include "util/misc.h"
#include "common/PeerDefs.h"

#include <retroshare/rsfiles.h>
#include <retroshare/rspeers.h>
#include <retroshare/rsforums.h>
#include <retroshare/rschannels.h>

//#define DEBUG_RSLINK 1

#define HOST_FILE       "file"
#define HOST_PERSON     "person"
#define HOST_FORUM      "forum"
#define HOST_CHANNEL    "channel"
#define HOST_REGEXP     "file|person|forum|channel"

#define FILE_NAME       "name"
#define FILE_SIZE       "size"
#define FILE_HASH       "hash"

#define PERSON_NAME     "name"
#define PERSON_HASH     "hash"

#define FORUM_NAME      "name"
#define FORUM_ID        "id"
#define FORUM_MSGID     "msgid"

#define CHANNEL_NAME    "name"
#define CHANNEL_ID      "id"
#define CHANNEL_MSGID   "msgid"

RetroShareLink::RetroShareLink(const QUrl& url)
{
    fromUrl(url);
}

RetroShareLink::RetroShareLink(const QString& url)
{
    fromString(url);
}

void RetroShareLink::fromString(const QString& url)
{
    clear();

    // parse
#ifdef DEBUG_RSLINK
    std::cerr << "got new RS link \"" << url.toStdString() << "\"" << std::endl ;
#endif

    if ((url.startsWith(QString(RSLINK_SCHEME) + "://" + QString(HOST_FILE)) && url.count("|") == 3) ||
        (url.startsWith(QString(RSLINK_SCHEME) + "://" + QString(HOST_PERSON)) && url.count("|") == 2)) {
        /* Old link, we try it */
        QStringList list = url.split ("|");

        if (list.size() >= 1) {
            if (list.size() == 4 && list[0] == QString(RSLINK_SCHEME) + "://" + QString(HOST_FILE)) {
                bool ok ;

                _type = TYPE_FILE;
                _name = list[1] ;
                _size = list[2].toULongLong(&ok) ;
                _hash = list[3].left(40) ;	// normally not necessary, but it's a security.

                if (ok) {
#ifdef DEBUG_RSLINK
                    std::cerr << "New RetroShareLink forged:" << std::endl ;
                    std::cerr << "  name = \"" << _name.toStdString() << "\"" << std::endl ;
                    std::cerr << "  hash = \"" << _hash.toStdString() << "\"" << std::endl ;
                    std::cerr << "  size = " << _size << std::endl ;
#endif
                    check();
                    return;
                }
            } else if (list.size() == 3 && list[0] == QString(RSLINK_SCHEME) + "://" + QString(HOST_PERSON)) {
                _type = TYPE_PERSON;
                _name = list[1] ;
                _hash = list[2].left(40) ;	// normally not necessary, but it's a security.
                _size = 0;
                check();
                return;
            }

            // bad link
        }
    }

    /* Now try QUrl */
    fromUrl(QUrl::fromEncoded(url.toAscii()));
}

void RetroShareLink::fromUrl(const QUrl& url)
{
    clear();

    // parse
#ifdef DEBUG_RSLINK
    std::cerr << "got new RS link \"" << url.toString().toStdString() << "\"" << std::endl ;
#endif

    if (url.scheme() != RSLINK_SCHEME) {
        /* No RetroShare-Link */
        return;
    }

    if (url.host() == HOST_FILE) {
        bool ok ;

        _type = TYPE_FILE;
        _name = url.queryItemValue(FILE_NAME);
        _size = url.queryItemValue(FILE_SIZE).toULongLong(&ok);
        _hash = url.queryItemValue(FILE_HASH).left(40);	// normally not necessary, but it's a security.

        if (ok) {
#ifdef DEBUG_RSLINK
            std::cerr << "New RetroShareLink forged:" << std::endl ;
            std::cerr << "  name = \"" << _name.toStdString() << "\"" << std::endl ;
            std::cerr << "  hash = \"" << _hash.toStdString() << "\"" << std::endl ;
            std::cerr << "  size = " << _size << std::endl ;
#endif
            check();
            return;
        }
    }

    if (url.host() == HOST_PERSON) {
        _type = TYPE_PERSON;
        _name = url.queryItemValue(PERSON_NAME);
        _hash = url.queryItemValue(PERSON_HASH).left(40);	// normally not necessary, but it's a security.
        _size = 0;
        check();
        return;
    }

    if (url.host() == HOST_FORUM) {
        _type = TYPE_FORUM;
        _name = url.queryItemValue(FORUM_NAME);
        _hash = url.queryItemValue(FORUM_ID);
        _msgId = url.queryItemValue(FORUM_MSGID);
        _size = 0;
        check();
        return;
    }

    if (url.host() == HOST_CHANNEL) {
        _type = TYPE_CHANNEL;
        _name = url.queryItemValue(CHANNEL_NAME);
        _hash = url.queryItemValue(CHANNEL_ID);
        _msgId = url.queryItemValue(CHANNEL_MSGID);
        _size = 0;
        check();
        return;
    }

    // bad link

#ifdef DEBUG_RSLINK
    std::cerr << "Wrongly formed RS link. Can't process." << std::endl ;
#endif
    clear();
}

// file
RetroShareLink::RetroShareLink(const QString & name, uint64_t size, const QString & hash)
    : _name(name),_size(size),_hash(hash)
{
    _valid = false;
    _type = TYPE_FILE;
    check() ;
}

// person
RetroShareLink::RetroShareLink(const QString & name, const QString & hash)
    : _name(name),_size(0),_hash(hash)
{
    _valid = false;
    _type = TYPE_PERSON;
    check() ;
}

// forum, channel
RetroShareLink::RetroShareLink(enumType type, const QString& name, const QString& id, const QString& msgId)
    : _name(name),_size(0),_hash(id),_msgId(msgId)
{
    _valid = false;
    _type = TYPE_UNKNOWN;

    switch (type) {
    case TYPE_UNKNOWN:
    case TYPE_FILE:
    case TYPE_PERSON:
        // wrong type
        break;
    case TYPE_FORUM:
    case TYPE_CHANNEL:
        _type = type;
        break;
    }

    check() ;
}

void RetroShareLink::clear()
{
    _valid = false;
    _type = TYPE_UNKNOWN;
    _hash = "" ;
    _size = 0 ;
    _name = "" ;
}

void RetroShareLink::check()
{
    _valid = true;

    switch (_type) {
    case TYPE_UNKNOWN:
        _valid = false;
        break;
    case TYPE_FILE:
        if(_size > (((uint64_t)1)<<40))	// 1TB. Who has such large files?
            _valid = false;

        if(!checkName(_name))
            _valid = false;

        if(!checkHash(_hash))
            _valid = false;
        break;
    case TYPE_PERSON:
        if(_size != 0)
            _valid = false;

        if(_name.isEmpty())
            _valid = false;

        if(_hash.isEmpty())
            _valid = false;
        break;
    case TYPE_FORUM:
        if(_size != 0)
            _valid = false;

        if(_name.isEmpty())
            _valid = false;

        if(_hash.isEmpty())
            _valid = false;
        break;
    case TYPE_CHANNEL:
        if(_size != 0)
            _valid = false;

        if(_name.isEmpty())
            _valid = false;

        if(_hash.isEmpty())
            _valid = false;
        break;
    }

    if(!_valid) // we should throw an exception instead of this crap, but drbob doesn't like exceptions. Why ???
    {
        _type = TYPE_UNKNOWN;
        _hash = "" ;
        _name = "" ;
        _size = 0 ;
    }
}

QString RetroShareLink::toString(bool encoded /*= true*/) const
{
    switch (_type) {
    case TYPE_UNKNOWN:
        break;
    case TYPE_FILE:
        {
            QUrl url;
            url.setScheme(RSLINK_SCHEME);
            url.setHost(HOST_FILE);
            url.addQueryItem(FILE_NAME, _name);
            url.addQueryItem(FILE_SIZE, QString::number(_size));
            url.addQueryItem(FILE_HASH, _hash);

            if (encoded) {
                return url.toEncoded();
            }

            return url.toString();
        }
    case TYPE_PERSON:
        {
            QUrl url;
            url.setScheme(RSLINK_SCHEME);
            url.setHost(HOST_PERSON);
            url.addQueryItem(PERSON_NAME, _name);
            url.addQueryItem(PERSON_HASH, _hash);

            if (encoded) {
                return url.toEncoded();
            }

            return url.toString();
        }
    case TYPE_FORUM:
        {
            QUrl url;
            url.setScheme(RSLINK_SCHEME);
            url.setHost(HOST_FORUM);
            url.addQueryItem(FORUM_NAME, _name);
            url.addQueryItem(FORUM_ID, _hash);
            if (!_msgId.isEmpty()) {
                url.addQueryItem(FORUM_MSGID, _msgId);
            }

            if (encoded) {
                return url.toEncoded();
            }

            return url.toString();
        }
    case TYPE_CHANNEL:
        {
            QUrl url;
            url.setScheme(RSLINK_SCHEME);
            url.setHost(HOST_CHANNEL);
            url.addQueryItem(CHANNEL_NAME, _name);
            url.addQueryItem(CHANNEL_ID, _hash);
            if (!_msgId.isEmpty()) {
                url.addQueryItem(CHANNEL_MSGID, _msgId);
            }

            if (encoded) {
                return url.toEncoded();
            }

            return url.toString();
        }
    }

    return "";
}

QString RetroShareLink::niceName() const
{
    if (type() == TYPE_PERSON) {
        return PeerDefs::rsid(name().toStdString(), hash().toStdString());
    }

    return name();
}

QString RetroShareLink::toHtml() const
{
    return QString("<a href=\"") + toString(true) + "\">" + niceName() + "</a>" ;
}

QString RetroShareLink::toHtmlFull() const
{
    return QString("<a href=\"") + toString(true) + "\">" + toString(false) + "</a>" ;
}

QString RetroShareLink::toHtmlSize() const
{
    return QString("<a href=\"") + toString(true) + "\">" + name() +"</a>" + " " + "<font color=\"blue\">" + "(" +  misc::friendlyUnit(_size) + ")" +"</font>";
}

bool RetroShareLink::checkName(const QString& name)
{
    if(name == "")
        return false ;

    for(int i=0;i<name.length();++i)
    {
        QChar::Category cat( name[i].category() ) ;

        if(	cat == QChar::Separator_Line
                || cat == QChar::Other_NotAssigned
                )
        {
#ifdef DEBUG_RSLINK
            std::cerr <<"Unwanted category " << cat << " at place " << i << " in string \"" << name.toStdString() << "\"" << std::endl ;
#endif
            return false ;
        }
    }

    return true ;
}

QUrl RetroShareLink::toUrl() const
{
    return QUrl(toString()) ;
}

bool RetroShareLink::checkHash(const QString& hash)
{
    if(hash.length() != 40)
        return false ;

    QByteArray qb(hash.toAscii()) ;

    for(int i=0;i<qb.length();++i)
    {
        unsigned char b(qb[i]) ;

        if(!((b>47 && b<58) || (b>96 && b<103)))
            return false ;
    }

    return true ;
}

bool RetroShareLink::process(int flag)
{
    if (valid() == false) {
        std::cerr << " RetroShareLink::process invalid request" << std::endl;
        return false;
    }

    switch (type()) {
    case TYPE_UNKNOWN:
        break;

    case TYPE_FILE:
        {
#ifdef DEBUG_RSLINK
            std::cerr << " RetroShareLink::process FileRequest : fileName : " << name().toUtf8().constData() << ". fileHash : " << hash().toStdString() << ". fileSize : " << size() << std::endl;
#endif

            // Get a list of available direct sources, in case the file is browsable only.
            std::list<std::string> srcIds;
            FileInfo finfo ;
            rsFiles->FileDetails(hash().toStdString(), RS_FILE_HINTS_REMOTE,finfo) ;

            for(std::list<TransferInfo>::const_iterator it(finfo.peers.begin());it!=finfo.peers.end();++it)
            {
#ifdef DEBUG_RSLINK
                std::cerr << "  adding peerid " << (*it).peerId << std::endl ;
#endif
                srcIds.push_back((*it).peerId) ;
            }

            if (rsFiles->FileRequest(name().toUtf8().constData(), hash().toStdString(), size(), "", RS_FILE_HINTS_NETWORK_WIDE, srcIds)) {
                if (flag & RSLINK_PROCESS_NOTIFY_SUCCESS) {
                    QMessageBox mb(QObject::tr("File Request Confirmation"), QObject::tr("The file has been added to your download list."),QMessageBox::Information,QMessageBox::Ok,0,0);
                    mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                    mb.exec();
                }
                return true;
            }

            if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
                QMessageBox mb(QObject::tr("File Request canceled"), QObject::tr("The file has not been added to your download list, because you already have it."),QMessageBox::Critical,QMessageBox::Ok,0,0);
                mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                mb.exec();
            }
            return false;
        }

    case TYPE_PERSON:
        {
#ifdef DEBUG_RSLINK
            std::cerr << " RetroShareLink::process FriendRequest : name : " << name().toStdString() << ". id : " << hash().toStdString() << std::endl;
#endif

            RsPeerDetails detail;
            if (rsPeers->getPeerDetails(hash().toStdString(), detail)) {
                if (detail.gpg_id == rsPeers->getGPGOwnId()) {
                    // it's me, do nothing
                    return true;
                }

                if (detail.accept_connection) {
                    // peer connection is already accepted
                    if (flag & RSLINK_PROCESS_NOTIFY_SUCCESS) {
                        QMessageBox mb(QObject::tr("Friend Request Confirmation"), QObject::tr("The friend is already in your list."),QMessageBox::Information,QMessageBox::Ok,0,0);
                        mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                        mb.exec();
                    }
                    return true;
                }

                if (rsPeers->setAcceptToConnectGPGCertificate(hash().toStdString(), true)) {
                    if (flag & RSLINK_PROCESS_NOTIFY_SUCCESS) {
                        QMessageBox mb(QObject::tr("Friend Request Confirmation"), QObject::tr("The friend has been added to your list."),QMessageBox::Information,QMessageBox::Ok,0,0);
                        mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                        mb.exec();
                    }
                    return true;
                }

                if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
                    QMessageBox mb(QObject::tr("Friend Request canceled"), QObject::tr("The friend could not be added to your list."),QMessageBox::Critical,QMessageBox::Ok,0,0);
                    mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                    mb.exec();
                }
                return false;
            }

            if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
                QMessageBox mb(QObject::tr("Friend Request canceled"), QObject::tr("The friend could not be found."),QMessageBox::Critical,QMessageBox::Ok,0,0);
                mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                mb.exec();
            }
            return false;
        }

    case TYPE_FORUM:
        {
#ifdef DEBUG_RSLINK
            std::cerr << " RetroShareLink::process ForumRequest : name : " << name().toStdString() << ". id : " << hash().toStdString() << ". msgId : " << msgId().toStdString() << std::endl;
#endif

            ForumInfo fi;
            if (!rsForums->getForumInfo(id().toStdString(), fi)) {
                if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
                    QMessageBox mb(QObject::tr("Forum Request canceled"), QObject::tr("The forum \"%1\" could not be found.").arg(name()),QMessageBox::Critical,QMessageBox::Ok,0,0);
                    mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                    mb.exec();
                }
                return false;
            }

            ForumMsgInfo msg;
            if (!msgId().isEmpty()) {
                if (!rsForums->getForumMessage(fi.forumId, msgId().toStdString(), msg)) {
                    if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
                        QMessageBox mb(QObject::tr("Forum Request canceled"), QObject::tr("The forum message in forum \"%1\" could not be found.").arg(name()),QMessageBox::Critical,QMessageBox::Ok,0,0);
                        mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                        mb.exec();
                    }
                    return false;
                }
            }

            MainWindow::showWindow(MainWindow::Forums);
            ForumsDialog *forumsDialog = dynamic_cast<ForumsDialog*>(MainWindow::getPage(MainWindow::Forums));
            if (!forumsDialog) {
                return false;
            }

            return forumsDialog->navigate(fi.forumId, msg.msgId);
        }

    case TYPE_CHANNEL:
        {
#ifdef DEBUG_RSLINK
            std::cerr << " RetroShareLink::process ChannelRequest : name : " << name().toStdString() << ". id : " << hash().toStdString() << ". msgId : " << msgId().toStdString() << std::endl;
#endif

            ChannelInfo ci;
            if (!rsChannels->getChannelInfo(id().toStdString(), ci)) {
                if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
                    QMessageBox mb(QObject::tr("Channel Request canceled"), QObject::tr("The channel \"%1\" could not be found.").arg(name()),QMessageBox::Critical,QMessageBox::Ok,0,0);
                    mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                    mb.exec();
                }
                return false;
            }

            ChannelMsgInfo msg;
            if (!msgId().isEmpty()) {
                if (!rsChannels->getChannelMessage(ci.channelId, msgId().toStdString(), msg)) {
                    if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
                        QMessageBox mb(QObject::tr("Channel Request canceled"), QObject::tr("The channel message in channel \"%1\" could not be found.").arg(name()),QMessageBox::Critical,QMessageBox::Ok,0,0);
                        mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
                        mb.exec();
                    }
                    return false;
                }
            }

            MainWindow::showWindow(MainWindow::Channels);
            ChannelFeed *channelFeed = dynamic_cast<ChannelFeed*>(MainWindow::getPage(MainWindow::Channels));
            if (!channelFeed) {
                return false;
            }

            return channelFeed->navigate(ci.channelId, msg.msgId);
        }
    }

    std::cerr << " RetroShareLink::process unknown type: " << type() << std::endl;

    if (flag & RSLINK_PROCESS_NOTIFY_ERROR) {
        QMessageBox mb(QObject::tr("File Request Error"), QObject::tr("The file link is malformed."),QMessageBox::Critical,QMessageBox::Ok,0,0);
        mb.setWindowIcon(QIcon(QString::fromUtf8(":/images/rstray3.png")));
        mb.exec();
    }
    return false;
}

void RSLinkClipboard::copyLinks(const std::vector<RetroShareLink>& links) 
{
    QString res ;
    for(uint32_t i=0;i<links.size();++i)
        res += links[i].toString() + "\n" ;

    QApplication::clipboard()->setText(res) ;
}

void RSLinkClipboard::pasteLinks(std::vector<RetroShareLink> &links) 
{
    return parseClipboard(links);
}

void RSLinkClipboard::parseClipboard(std::vector<RetroShareLink> &links)
{
    // parse clipboard for links.
    //
    links.clear();
    QString text = QApplication::clipboard()->text() ;

    std::cerr << "Parsing clipboard:" << text.toStdString() << std::endl ;

    QRegExp rx(QString("retroshare://(%1)[^\r\n]+").arg(HOST_REGEXP));

    int pos = 0;

    while((pos = rx.indexIn(text, pos)) != -1)
    {
        QString url(text.mid(pos, rx.matchedLength()));
        RetroShareLink link(url);

        if(link.valid())
        {
            // check that the link is not already in the list:
            bool already = false ;
            for(uint32_t i=0;i<links.size();++i)
                if(links[i] == link)
                {
                already = true ;
                break ;
            }

            if(!already)
            {
                links.push_back(link) ;
#ifdef DEBUG_RSLINK
                std::cerr << "captured link: " << link.toString().toStdString() << std::endl ;
#endif
            }
        }
#ifdef DEBUG_RSLINK
        else
            std::cerr << "invalid link" << std::endl ;
#endif

        pos += rx.matchedLength();
    }
}

QString RSLinkClipboard::toString()
{
    std::vector<RetroShareLink> links;
    parseClipboard(links);

    QString res ;
    for(uint32_t i=0;i<links.size();++i)
        res += links[i].toString() + "\n" ;

    return res ;
}

QString RSLinkClipboard::toHtml()
{
    std::vector<RetroShareLink> links;
    parseClipboard(links);

    QString res ;
    for(uint32_t i=0;i<links.size();++i)
        res += links[i].toHtml() + "<br/>" ;

    return res ;
}

QString RSLinkClipboard::toHtmlFull()
{
    std::vector<RetroShareLink> links;
    parseClipboard(links);

    QString res ;
    for(uint32_t i=0;i<links.size();++i)
        res += links[i].toHtmlFull() + "<br/>" ;

    return res ;
}

bool RSLinkClipboard::empty(RetroShareLink::enumType type /*= RetroShareLink::TYPE_UNKNOWN*/)
{
    std::vector<RetroShareLink> links;
    parseClipboard(links);

    if (type == RetroShareLink::TYPE_UNKNOWN) {
        return links.empty();
    }

    for (std::vector<RetroShareLink>::iterator link = links.begin(); link != links.end(); link++) {
        if (link->type() == type) {
            return false;
        }
    }

    return true;
}

/*static*/ int RSLinkClipboard::process(RetroShareLink::enumType type /*= RetroShareLink::TYPE_UNKNOWN*/, int flag /*= RSLINK_PROCESS_NOTIFY_ALL*/)
{
    std::vector<RetroShareLink> links;
    pasteLinks(links);

    int count = 0;

    for (uint32_t i = 0; i < links.size(); i++) {
        if (links[i].valid() && (type == RetroShareLink::TYPE_UNKNOWN || links[i].type() == type)) {
            if (links[i].process(flag)) {
                count++;
            }
        }
    }

    return count;
}
