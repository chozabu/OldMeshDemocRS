/****************************************************************
 *  RetroShare is distributed under the following license:
 *
 *  Copyright (C) 2006, crypton
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

#include <QFile>
#include <QFileInfo>
#include "common/vmessagebox.h"

#include "rsiface/rsiface.h"
#include "rsiface/rspeers.h"
#include "rsiface/rsmsgs.h"
#include "rsiface/rsstatus.h"
#include "rsiface/rsnotify.h"

#include "rshare.h"
#include "MessengerWindow.h"

#include "chat/PopupChatDialog.h"
#include "msgs/MessageComposer.h"
#include "connect/ConfCertDialog.h"
#include "util/PixmapMerging.h"
#include "LogoBar.h"
#include "util/Widget.h"
#include "settings/rsharesettings.h"

#include "gui/connect/ConnectFriendWizard.h"

#include "ShareManager.h"

#include <iostream>
#include <sstream>

/* Images for context menu icons */
#define IMAGE_REMOVEFRIEND       ":/images/removefriend16.png"
#define IMAGE_EXPIORTFRIEND      ":/images/exportpeers_16x16.png"
#define IMAGE_CHAT               ":/images/chat.png"
#define IMAGE_MSG                ":/images/message-mail.png"
#define IMAGE_CONNECT            ":/images/connect_friend.png"
#define IMAGE_PEERINFO           ":/images/peerdetails_16x16.png"
#define IMAGE_AVAIBLE            ":/images/user/identityavaiblecyan24.png"
#define IMAGE_CONNECT2           ":/images/reload24.png"

/* Images for Status icons */
#define IMAGE_ONLINE             ":/images/im-user.png"
#define IMAGE_OFFLINE            ":/images/im-user-offline.png"
#define IMAGE_AWAY             ":/images/im-user-away.png"
#define IMAGE_BUSY            ":/images/im-user-busy.png"
#define IMAGE_INACTIVE		":/images/im-user-inactive.png"

/******
 * #define MSG_DEBUG 1
 *****/
MessengerWindow* MessengerWindow::mv = 0;

MessengerWindow* MessengerWindow::getInstance()
{
	if(mv == 0)
	{
		mv = new MessengerWindow();
	}
	return mv;
}

void MessengerWindow::releaseInstance()
{
	if(mv != 0)
	{
		delete mv;
	}
}

/** Constructor */
MessengerWindow::MessengerWindow(QWidget* parent, Qt::WFlags flags)
    : 	RWindow("MessengerWindow", parent, flags), maxTimeBeforeIdle(30)
{
	/* Invoke the Qt Designer generated object setup routine */
	ui.setupUi(this);
	
	connect( ui.messengertreeWidget, SIGNAL( customContextMenuRequested( QPoint ) ), this, SLOT( messengertreeWidgetCostumPopupMenu( QPoint ) ) );
  connect( ui.messengertreeWidget, SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int)), this, SLOT(chatfriend()));

	connect( ui.avatarButton, SIGNAL(clicked()), SLOT(getAvatar()));
	connect( ui.shareButton, SIGNAL(clicked()), SLOT(openShareManager()));
  connect( ui.addIMAccountButton, SIGNAL(clicked( bool ) ), this , SLOT( addFriend() ) );
  connect( ui.actionHide_Offline_Friends, SIGNAL(triggered()), this, SLOT(insertPeers()));
  connect(ui.clearButton, SIGNAL(clicked()), this, SLOT(clearFilter()));
  
  connect(ui.messagelineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(savestatusmessage()));
  connect(ui.filterPatternLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(filterRegExpChanged()));

  QTimer *timer = new QTimer(this);
  timer->connect(timer, SIGNAL(timeout()), this, SLOT(updateMessengerDisplay()));
  timer->start(1000); /* one second */


	/* to hide the header  */
	ui.messengertreeWidget->header()->hide(); 
 
  /* Set header resize modes and initial section sizes */
	ui.messengertreeWidget->setColumnCount(4);
  ui.messengertreeWidget->setColumnHidden ( 3, true);
  ui.messengertreeWidget->setColumnHidden ( 2, true);
  //ui.messengertreeWidget->sortItems( 0, Qt::AscendingOrder );

	QHeaderView * _header = ui.messengertreeWidget->header () ;   
	_header->setResizeMode (0, QHeaderView::Stretch);
	_header->setResizeMode (1, QHeaderView::Custom);
	_header->setStretchLastSection(false);


	_header->resizeSection ( 0, 200 );
  _header->resizeSection ( 1, 42 );      
 
	//LogoBar
	_rsLogoBarmessenger = NULL;
	_rsLogoBarmessenger = new LogoBar(ui.logoframe);
	Widget::createLayout(ui.logoframe)->addWidget(_rsLogoBarmessenger);
  
	ui.statuscomboBox->setMinimumWidth(20);
	ui.messagelineEdit->setMinimumWidth(20);

  itemFont = QFont("ARIAL", 10);
	itemFont.setBold(true);

  isIdle = false;
  loadOwnStatus(); // hack; placed in constructor to preempt sendstatus, so status loaded from file
  insertPeers(); 		
  updateAvatar();
  loadmystatusmessage();
  
  ui.clearButton->hide();

  displayMenu();
  updateMessengerDisplay();
  
  /* Hide platform specific features */
#ifdef Q_WS_WIN
#endif
}


void MessengerWindow::messengertreeWidgetCostumPopupMenu( QPoint point )
{
      QTreeWidgetItem *c = getCurrentPeer();
	  	if (!c) 
	  	{
 	  	  //no peer selected
	  	  return;
	  	}

      QMenu contextMnu( this );

      QAction* expandAll = new QAction(tr( "Expand all" ), this );
      connect( expandAll , SIGNAL( triggered() ), ui.messengertreeWidget, SLOT (expandAll()) );

      QAction* collapseAll = new QAction(tr( "Collapse all" ), this );
      connect( collapseAll , SIGNAL( triggered() ), ui.messengertreeWidget, SLOT(collapseAll()) );

      chatAct = new QAction(QIcon(IMAGE_CHAT), tr( "Chat" ), this );
      connect( chatAct , SIGNAL( triggered() ), this, SLOT( chatfriend() ) );

      sendMessageAct = new QAction(QIcon(IMAGE_MSG), tr( "Message Friend" ), this );
      connect( sendMessageAct , SIGNAL( triggered() ), this, SLOT( sendMessage() ) );

      connectfriendAct = new QAction(QIcon(IMAGE_CONNECT), tr( "Connect To Friend" ), this );
      connect( connectfriendAct , SIGNAL( triggered() ), this, SLOT( connectfriend() ) );

      configurefriendAct = new QAction(QIcon(IMAGE_PEERINFO), tr( "Peer Details" ), this );
      connect( configurefriendAct , SIGNAL( triggered() ), this, SLOT( configurefriend() ) );

      //profileviewAct = new QAction(QIcon(IMAGE_PEERINFO), tr( "Profile View" ), this );
      //connect( profileviewAct , SIGNAL( triggered() ), this, SLOT( viewprofile() ) );

      exportfriendAct = new QAction(QIcon(IMAGE_EXPIORTFRIEND), tr( "Export Friend" ), this );
      connect( exportfriendAct , SIGNAL( triggered() ), this, SLOT( exportfriend() ) );

      if (c->type() == 0) {
          //this is a GPG key
          removefriendAct = new QAction(QIcon(IMAGE_REMOVEFRIEND), tr( "Deny Friend" ), this );
      } else {
          removefriendAct = new QAction(QIcon(IMAGE_REMOVEFRIEND), tr( "Remove Friend Location" ), this );
      }
      connect( removefriendAct , SIGNAL( triggered() ), this, SLOT( removefriend() ) );


      QWidget *widget = new QWidget();
      widget->setStyleSheet( ".QWidget{background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #FEFEFE, stop:1 #E8E8E8); border: 1px solid #CCCCCC;}");  
      
      QHBoxLayout *hbox = new QHBoxLayout();
      hbox->setMargin(0);
      hbox->setSpacing(6);
    
      iconLabel = new QLabel( this );
      iconLabel->setPixmap(QPixmap(":/images/user/friends24.png"));
      iconLabel->setMaximumSize( iconLabel->frameSize().height() + 24, 24 );
      hbox->addWidget(iconLabel);
       
      if (c->type() == 0) {
          //this is a GPG key
         textLabel = new QLabel( tr("<strong>GPG Key</strong>"), this );
      } else {
         textLabel = new QLabel( tr("<strong>RetroShare instance</strong>"), this );
      }

      hbox->addWidget(textLabel);
      
      spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
      hbox->addItem(spacerItem); 
       
      widget->setLayout( hbox );
    
      QWidgetAction *widgetAction = new QWidgetAction(this); 
      widgetAction->setDefaultWidget(widget); 

      contextMnu.addAction( widgetAction);
      contextMnu.addAction( chatAct);
      contextMnu.addAction( sendMessageAct);
      contextMnu.addAction( configurefriendAct);
      //contextMnu.addAction( profileviewAct);
      if (c->type() != 0) {
          //this is a SSL key
          contextMnu.addAction( connectfriendAct);
      }
      contextMnu.addAction( removefriendAct);
      //contextMnu.addAction( exportfriendAct);
      contextMnu.addSeparator();
      contextMnu.addAction( expandAll);
      contextMnu.addAction( collapseAll);
      contextMnu.exec(QCursor::pos());
}

void MessengerWindow::updateMessengerDisplay()
{
	if(RsAutoUpdatePage::eventsLocked())
		return ;
        // add self nick and Avatar to Friends.
        RsPeerDetails pd ;
        if (rsPeers->getPeerDetails(rsPeers->getOwnId(),pd)) {
        
                QString titleStr("<span style=\"font-size:14pt; font-weight:500;"
                       "color:#FFFFFF;\">%1</span>");
                ui.nicklabel->setText(titleStr.arg(QString::fromStdString(pd.name) + tr(" - ") + QString::fromStdString(pd.location))) ;
        }

        insertPeers();
        savestatus();
}

/* get the list of peers from the RsIface.  */
void  MessengerWindow::insertPeers()
{
   std::list<std::string> gpgFriends;
   std::list<std::string>::iterator it;

   std::list<StatusInfo> statusInfo;
   rsStatus->getStatus(statusInfo);

  // if(isIdle)
    //   QMessageBox::StandardButton sb = QMessageBox::warning ( NULL, tr("Idle"),
      //                  tr("You are Idle"), QMessageBox::Ok);

        if (!rsPeers) {
                /* not ready yet! */
                std::cerr << "PeersDialog::insertPeers() not ready yet : rsPeers unintialized."  << std::endl;
                return;
        }

        rsPeers->getGPGAcceptedList(gpgFriends);

        //add own gpg id, if we have more than on location (ssl client)
        std::list<std::string> ownSslContacts;
        rsPeers->getSSLChildListOfGPGId(rsPeers->getGPGOwnId(), ownSslContacts);
        if (ownSslContacts.size() > 0) {
            gpgFriends.push_back(rsPeers->getGPGOwnId());
        }

        /* get a link to the table */
        QTreeWidget *peertreeWidget = ui.messengertreeWidget;

        //remove items that are not fiends anymore
        int index = 0;
        while (index < peertreeWidget->topLevelItemCount()) {
            std::string gpg_widget_id = (peertreeWidget->topLevelItem(index))->text(3).toStdString();
            std::list<std::string>::iterator gpgfriendIt;
            bool found = false;
            for (gpgfriendIt =  gpgFriends.begin(); gpgfriendIt != gpgFriends.end(); gpgfriendIt++) {
                if (gpg_widget_id == *gpgfriendIt) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                delete (peertreeWidget->takeTopLevelItem(index));
            } else {
                index++;
            }
        }


        //add the gpg friends
        for(it = gpgFriends.begin(); it != gpgFriends.end(); it++) {
//            if (*it == rsPeers->getGPGOwnId()) {
//                continue;
//            }

            /* make a widget per friend */
            QTreeWidgetItem *gpg_item;
            QList<QTreeWidgetItem *> list = peertreeWidget->findItems(QString::fromStdString(*it), Qt::MatchExactly, 3);
            if (list.size() == 1) {
                gpg_item = list.front();
            } else {
                gpg_item = new QTreeWidgetItem(0); //set type to 0 for custom popup menu
                gpg_item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
            }

            RsPeerDetails detail;
            if ((!rsPeers->getPeerDetails(*it, detail) || !detail.accept_connection)
                && detail.gpg_id != rsPeers->getGPGOwnId()) {
                //don't accept anymore connection, remove from the view
                delete (peertreeWidget->takeTopLevelItem(peertreeWidget->indexOfTopLevelItem(gpg_item)));
                continue;
            }

            //use to mark item as updated
            gpg_item->setData(0, Qt::UserRole, true);            

            gpg_item -> setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter );
            
            gpg_item -> setSizeHint(0,  QSize( 27,27 ) ); 

            /* not displayed, used to find back the item */
            gpg_item -> setText(3, QString::fromStdString(detail.id));

            //remove items that are not friends anymore
            int childIndex = 0;
            while (childIndex < gpg_item->childCount()) {
                std::string ssl_id = (gpg_item->child(childIndex))->text(3).toStdString();
                if (!rsPeers->isFriend(ssl_id)) {
                    delete (gpg_item->takeChild(childIndex));
                } else {
                    childIndex++;
                }
            }

            //update the childs (ssl certs)
            bool gpg_connected = false;
            bool gpg_online = false;
            std::list<std::string> sslContacts;
            rsPeers->getSSLChildListOfGPGId(detail.gpg_id, sslContacts);
            for(std::list<std::string>::iterator sslIt = sslContacts.begin(); sslIt != sslContacts.end(); sslIt++) {
                QTreeWidgetItem *sslItem = NULL;

                //find the corresponding sslItem child item of the gpg item
                bool newChild = true;
                for (int childIndex = 0; childIndex < gpg_item->childCount(); childIndex++) {
                    if (gpg_item->child(childIndex)->text(3).toStdString() == *sslIt) {
                        sslItem = gpg_item->child(childIndex);
                        newChild = false;
                        break;
                    }
                }

                RsPeerDetails sslDetail;
                if (!rsPeers->getPeerDetails(*sslIt, sslDetail) || !rsPeers->isFriend(*sslIt)) {
                    std::cerr << "Removing widget from the view : id : " << *sslIt << std::endl;
                    if (sslItem) {
                        //child has disappeared, remove it from the gpg_item
                        gpg_item->removeChild(sslItem);
                    }
                    continue;
                }

                if (sslItem == NULL) {
                   sslItem = new QTreeWidgetItem(1); //set type to 1 for custom popup menu
                }

                /* not displayed, used to find back the item */
                sslItem -> setText(3, QString::fromStdString(sslDetail.id));

                if (rsMsgs->getCustomStateString(sslDetail.id) != "") {
                    sslItem -> setText( 0, tr("location : ") + QString::fromStdString(sslDetail.location) + " " + QString::fromStdString(sslDetail.autoconnect) );
                    sslItem -> setToolTip( 0, tr("location : ") + QString::fromStdString(sslDetail.location) + tr(" - ") + QString::fromStdString(rsMsgs->getCustomStateString(sslDetail.id)));
                    gpg_item -> setText(0, QString::fromStdString(detail.name) + tr("\n") + QString::fromStdString(rsMsgs->getCustomStateString(sslDetail.id)));

                } else {
                    sslItem -> setText( 0, tr("location : ") + QString::fromStdString(sslDetail.location) + " " + QString::fromStdString(sslDetail.autoconnect));
                    sslItem -> setToolTip( 0, tr("location : ") + QString::fromStdString(sslDetail.location));
                    gpg_item -> setText(0, QString::fromStdString(detail.name) + tr("\n") + QString::fromStdString(sslDetail.location));
                }
                

                /* not displayed, used to find back the item */                
                //sslItem -> setText(1, QString::fromStdString(sslDetail.autoconnect));
                
                int i;
                if (sslDetail.state & RS_PEER_STATE_CONNECTED) {
                    sslItem->setHidden(false);
                    gpg_connected = true;
                    
                    /* change color and icon */            
                    sslItem -> setIcon(0,(QIcon(":/images/connect_established.png")));
                    QFont font;
                    font.setBold(true);
                    for(i = 0; i < 3; i++) {
                        sslItem -> setTextColor(i,(Qt::darkBlue));
                        sslItem -> setFont(i,font);
                    }
               } else if (sslDetail.state & RS_PEER_STATE_ONLINE) {
                    sslItem->setHidden(ui.actionHide_Offline_Friends->isChecked());
                    gpg_online = true;
                        
                    QFont font;
                    font.setBold(true);
                    for(i = 0; i < 3; i++) {
                        sslItem -> setTextColor(i,(Qt::black));
                        sslItem -> setFont(i,font);
                    }
                } else {
                    sslItem->setHidden(ui.actionHide_Offline_Friends->isChecked());
                    if (sslDetail.autoconnect !="Offline") {
                        sslItem -> setIcon(0, (QIcon(":/images/connect_creating.png")));
                    } else {
                        sslItem -> setIcon(0, (QIcon(":/images/connect_no.png")));
                    }

                    QFont font;
                    font.setBold(false);
                    for(i = 0; i < 3; i++) {
                        sslItem -> setTextColor(i,(Qt::black));
                        sslItem -> setFont(i,font);
                    }
                }

                #ifdef PEERS_DEBUG
                std::cerr << "PeersDialog::insertPeers() inserting sslItem." << std::endl;
                #endif
                /* add sl child to the list. If item is already in the list, it won't be duplicated thanks to Qt */
                gpg_item->addChild(sslItem);
                if (newChild) {
                    gpg_item->setExpanded(true);
                }
            }


            int i = 0;
            if (gpg_connected) {
                gpg_item->setHidden(false);
                //gpg_item -> setText(1, tr("Online")); // set to online regardless on update

                std::list<StatusInfo>::iterator it = statusInfo.begin();


                for(; it != statusInfo.end() ; it++){

                std::list<std::string>::iterator cont_it = sslContacts.begin();

                // don't forget the kids
                for(;  cont_it != sslContacts.end(); cont_it++){


                if((it->id == *cont_it) && (rsPeers->isOnline(*cont_it))){

								std::string status;
								rsStatus->getStatusString(it->status, status);
								gpg_item -> setText(1, QString::fromStdString(status));
								
                  QFont font;
                  font.setBold(true);

                  unsigned char *data = NULL;
                  int size = 0 ;
                  rsMsgs->getAvatarData(it->id ,data,size);

                  if(size != 0){

                  QPixmap avatar ;
                  avatar.loadFromData(data,size,"PNG") ;
                  QIcon avatar_icon(avatar);
                  QSize av_icon_size(100, 100);
                  gpg_item-> setIcon(1, avatar_icon);
                  delete[] data;

                  }else
                  {
                    gpg_item -> setIcon(1,(QIcon(":/images/no_avatar_70.png")));
                  }
                  if(it->status == RS_STATUS_INACTIVE)
                  {
                    gpg_item -> setIcon(0,(QIcon(IMAGE_INACTIVE)));
                    gpg_item -> setToolTip(0, tr("Peer Idle"));
                    
                    for(i = 0; i < 3; i++) {
                    gpg_item -> setTextColor(i,(Qt::gray));
                    gpg_item -> setFont(i,font);
                    }                    
                  }
                  else
                  if(it->status == RS_STATUS_ONLINE)
                  {
                    gpg_item -> setIcon(0,(QIcon(IMAGE_ONLINE)));
                    gpg_item -> setToolTip(0, tr("Peer Online"));
                    
                    for(i = 0; i < 3; i++) {
                    gpg_item -> setTextColor(i,(Qt::darkBlue));
                    gpg_item -> setFont(i,font);
                    }
                  }
                  else
                  if(it->status == RS_STATUS_AWAY)
                  {
                    gpg_item -> setIcon(0,(QIcon(IMAGE_AWAY)));
                    gpg_item -> setToolTip(0, tr("Peer Away"));
                    
                    for(i = 0; i < 3; i++) {
                    gpg_item -> setTextColor(i,(Qt::gray));
                    gpg_item -> setFont(i,font);
                    } 
                  }
                  else
                  if(it->status == RS_STATUS_BUSY)
                  {
                    gpg_item -> setIcon(0,(QIcon(IMAGE_BUSY)));
                    gpg_item -> setToolTip(0, tr("Peer Busy"));
                    
                    for(i = 0; i < 3; i++) {
                    gpg_item -> setTextColor(i,(Qt::gray));
                    gpg_item -> setFont(i,font);
                    } 
                  }
                  
                  }
                 }
                }

                                
            } else if (gpg_online) {
                gpg_item->setHidden(ui.actionHide_Offline_Friends->isChecked());
                gpg_item -> setIcon(0,(QIcon(IMAGE_AVAIBLE)));
                //gpg_item -> setText(1, tr("Available"));
                QFont font;
                font.setBold(true);
                for(i = 0; i < 3; i++) {
                    gpg_item -> setTextColor(i,(Qt::black));
                    gpg_item -> setFont(i,font);
                }
            } else {
                gpg_item->setHidden(ui.actionHide_Offline_Friends->isChecked());
                gpg_item -> setIcon(0,(QIcon(IMAGE_OFFLINE)));
                //gpg_item -> setText(1, tr("Offline"));
                QFont font;
                font.setBold(false);
                for(i = 0; i < 3; i++) {
                    gpg_item -> setTextColor(i,(Qt::black));
                    gpg_item -> setFont(i,font);
                }
            }

            /* add gpg item to the list. If item is already in the list, it won't be duplicated thanks to Qt */
            peertreeWidget->addTopLevelItem(gpg_item);
        }
        
    if (ui.filterPatternLineEdit->text().isEmpty() == false) {
        FilterItems();
    }
}

/* Utility Fns */
std::string getPeersRsCertId(QTreeWidgetItem *i)
{
        std::string id = (i -> text(3)).toStdString();
	return id;
}

/** Add a Friend ShortCut */
void MessengerWindow::addFriend()
{
    ConnectFriendWizard connwiz (this);

    connwiz.exec ();
}

/** Open a QFileDialog to browse for export a file. */
void MessengerWindow::exportfriend()
{
        QTreeWidgetItem *c = getCurrentPeer();

#ifdef PEERS_DEBUG
        std::cerr << "PeersDialog::exportfriend()" << std::endl;
#endif
	if (!c)
	{
#ifdef PEERS_DEBUG
                std::cerr << "PeersDialog::exportfriend() None Selected -- sorry" << std::endl;
#endif
		return;
	}

	std::string id = getPeersRsCertId(c);
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Certificate"), "",
	                                                     tr("Certificates (*.pqi)"));

	std::string file = fileName.toStdString();
	if (file != "")
	{
#ifdef PEERS_DEBUG
        	std::cerr << "PeersDialog::exportfriend() Saving to: " << file << std::endl;
        	std::cerr << std::endl;
#endif
		if (rsPeers)
		{
                        rsPeers->saveCertificateToFile(id, file);
		}
	}

}

void MessengerWindow::chatfriend()
{
    QTreeWidgetItem *i = getCurrentPeer();

    if (!i)
    	return;

    emit startChat(i);

}


QTreeWidgetItem *MessengerWindow::getCurrentPeer()
{
	/* get the current, and extract the Id */

	/* get a link to the table */
        QTreeWidget *peerWidget = ui.messengertreeWidget;
        QTreeWidgetItem *item = peerWidget -> currentItem();
        if (!item)
        {
#ifdef PEERS_DEBUG
		std::cerr << "Invalid Current Item" << std::endl;
#endif
		return NULL;
	}

#ifdef PEERS_DEBUG
	/* Display the columns of this item. */
	std::ostringstream out;
        out << "CurrentPeerItem: " << std::endl;

	for(int i = 1; i < 6; i++)
	{
		QString txt = item -> text(i);
		out << "\t" << i << ":" << txt.toStdString() << std::endl;
	}
	std::cerr << out.str();
#endif
	return item;
}

/* So from the Peers Dialog we can call the following control Functions:
 * (1) Remove Current.              FriendRemove(id)
 * (2) Allow/DisAllow.              FriendStatus(id, accept)
 * (2) Connect.                     FriendConnectAttempt(id, accept)
 * (3) Set Address.                 FriendSetAddress(id, str, port)
 * (4) Set Trust.                   FriendTrustSignature(id, bool)
 * (5) Configure (GUI Only) -> 3/4
 *
 * All of these rely on the finding of the current Id.
 */


void MessengerWindow::removefriend()
{
        QTreeWidgetItem *c = getCurrentPeer();
#ifdef PEERS_DEBUG
        std::cerr << "PeersDialog::removefriend()" << std::endl;
#endif
	if (!c)
	{
#ifdef PEERS_DEBUG
        	std::cerr << "PeersDialog::removefriend() Noone Selected -- sorry" << std::endl;
#endif
		return;
	}

	if (rsPeers)
	{
		rsPeers->removeFriend(getPeersRsCertId(c));
		emit friendsUpdated() ;
	}
}

void MessengerWindow::connectfriend()
{
	QTreeWidgetItem *c = getCurrentPeer();
#ifdef PEERS_DEBUG
	std::cerr << "PeersDialog::connectfriend()" << std::endl;
#endif
	if (!c)
	{
#ifdef PEERS_DEBUG
        	std::cerr << "PeersDialog::connectfriend() Noone Selected -- sorry" << std::endl;
#endif
		return;
	}

	if (rsPeers)
	{
		rsPeers->connectAttempt(getPeersRsCertId(c));
		c -> setIcon(0,(QIcon(IMAGE_CONNECT2)));
	}
}

/* GUI stuff -> don't do anything directly with Control */
void MessengerWindow::configurefriend()
{
	ConfCertDialog::show(getPeersRsCertId(getCurrentPeer()));
}

void MessengerWindow::updatePeersAvatar(const QString& peer_id)
{
	std::cerr << "PeersDialog: Got notified of new avatar for peer " << peer_id.toStdString() << std::endl ;

	PopupChatDialog *pcd = getPrivateChat(peer_id.toStdString(),rsPeers->getPeerName(peer_id.toStdString()), 0);
	pcd->updatePeerAvatar(peer_id.toStdString());
}

//============================================================================

PopupChatDialog *
MessengerWindow::getPrivateChat(std::string id, std::string name, uint chatflags)
{
   /* see if it exists already */
   PopupChatDialog *popupchatdialog = NULL;
   bool show = false;

   if (chatflags & RS_CHAT_REOPEN)
   {
  	show = true;
	std::cerr << "reopen flag so: enable SHOW popupchatdialog()";
	std::cerr << std::endl;
   }


   std::map<std::string, PopupChatDialog *>::iterator it;
   if (chatDialogs.end() != (it = chatDialogs.find(id)))
   {
   	/* exists already */
   	popupchatdialog = it->second;
   }
   else
   {
   	popupchatdialog = new PopupChatDialog(id, name);
	chatDialogs[id] = popupchatdialog;

	if (chatflags & RS_CHAT_OPEN_NEW)
	{
		std::cerr << "new chat so: enable SHOW popupchatdialog()";
		std::cerr << std::endl;

		show = true;
	}
   }

   if (show)
   {
	std::cerr << "SHOWING popupchatdialog()";
	std::cerr << std::endl;

	popupchatdialog->show();
   }

   /* now only do these if the window is visible */
   if (popupchatdialog->isVisible())
   {
	   if (chatflags & RS_CHAT_FOCUS)
	   {
		std::cerr << "focus chat flag so: GETFOCUS popupchatdialog()";
		std::cerr << std::endl;

		popupchatdialog->getfocus();
	   }
	   else
	   {
		std::cerr << "no focus chat flag so: FLASH popupchatdialog()";
		std::cerr << std::endl;

		popupchatdialog->flash();
	   }
   }
   else
   {
	std::cerr << "not visible ... so leave popupchatdialog()";
	std::cerr << std::endl;
   }

   return popupchatdialog;
}

//============================================================================


/** Overloads the default show  */
void MessengerWindow::show()
{

  if (!this->isVisible()) {
    QWidget::show();
  } else {
    QWidget::activateWindow();
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    QWidget::raise();
  }
}

void MessengerWindow::closeEvent (QCloseEvent * event)
{
	//Settings->saveWidgetInformation(this);

    hide();
    event->ignore();
}

/** Shows Share Manager */
void MessengerWindow::openShareManager()
{
	ShareManager::showYourself();

}

void MessengerWindow::sendMessage()
{
#ifdef MESSENGERWINDOW_DEBUG
    std::cerr << "SharedFilesDialog::sendMessage()" << std::endl;
#endif

    QTreeWidgetItem *i = getCurrentPeer();

    if (!i)
	return;

    std::string status = (i -> text(1)).toStdString();
    std::string name = (i -> text(2)).toStdString();
    std::string id = (i -> text(3)).toStdString();

    rsicontrol -> ClearInMsg();
    rsicontrol -> SetInMsg(id, true);
    std::list<std::string> sslIds;
    rsPeers->getSSLChildListOfGPGId(id, sslIds);
    for (std::list<std::string>::iterator it = sslIds.begin(); it != sslIds.end(); it++) {
        //put all sslChilds in message list
        rsicontrol -> SetInMsg(*it, true);
    }

    /* create a message */
    MessageComposer *nMsgDialog = new MessageComposer();

    nMsgDialog->newMsg();
    nMsgDialog->show();

    /* window will destroy itself! */
}

LogoBar & MessengerWindow::getLogoBar() const {
	return *_rsLogoBarmessenger;
}

void MessengerWindow::changeAvatarClicked() 
{

	updateAvatar();
}

void MessengerWindow::updateAvatar()
{
	unsigned char *data = NULL;
	int size = 0 ;

	rsMsgs->getOwnAvatarData(data,size); 

	std::cerr << "Image size = " << size << std::endl ;

	if(size == 0)
	   std::cerr << "Got no image" << std::endl ;

	// set the image
	QPixmap pix ;
	pix.loadFromData(data,size,"PNG") ;
	ui.avatarButton->setIcon(pix); // writes image into ba in PNG format

	delete[] data ;
}

void MessengerWindow::getAvatar()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Load File", QDir::homePath(), "Pictures (*.png *.xpm *.jpg)");
	if(!fileName.isEmpty())
	{
		picture = QPixmap(fileName).scaled(82,82, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

		std::cerr << "Sending avatar image down the pipe" << std::endl ;

		// send avatar down the pipe for other peers to get it.
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		picture.save(&buffer, "PNG"); // writes image into ba in PNG format

		std::cerr << "Image size = " << ba.size() << std::endl ;

		rsMsgs->setOwnAvatarData((unsigned char *)(ba.data()),ba.size()) ;	// last char 0 included.

		updateAvatar() ;
	}
}

/** Loads own personal status message */
void MessengerWindow::loadmystatusmessage()
{ 
    ui.messagelineEdit->setText(QString::fromStdString(rsMsgs->getCustomStateString()));
}

/** Save own status message */
void  MessengerWindow::savestatusmessage()
{
  Settings->setValueToGroup("Profile", "StatusMessage",ui.messagelineEdit->text());
	
  rsMsgs->setCustomStateString(ui.messagelineEdit->text().toStdString());
}

void MessengerWindow::on_actionSort_Peers_Descending_Order_activated()
{
  ui.messengertreeWidget->sortItems ( 0, Qt::DescendingOrder );
}

void MessengerWindow::on_actionSort_Peers_Ascending_Order_activated()
{
  ui.messengertreeWidget->sortItems ( 0, Qt::AscendingOrder );
}

void MessengerWindow::on_actionRoot_is_decorated_activated()
{
    ui.messengertreeWidget->setRootIsDecorated(true);
}

void MessengerWindow::on_actionRoot_isnot_decorated_activated()
{
    ui.messengertreeWidget->setRootIsDecorated(false);
}

void MessengerWindow::displayMenu()
{
  QMenu *lookmenu = new QMenu();
  lookmenu->addAction(ui.actionSort_Peers_Descending_Order); 
  lookmenu->addAction(ui.actionSort_Peers_Ascending_Order);
  lookmenu->addAction(ui.actionHide_Offline_Friends);
  
  QMenu *viewMenu = new QMenu( tr("View"), this );
	viewMenu->addAction(ui.actionRoot_is_decorated);
	viewMenu->addAction(ui.actionRoot_isnot_decorated);
  lookmenu->addMenu( viewMenu);

  ui.displaypushButton->setMenu(lookmenu);

}

/** Load own status Online,Away,Busy **/
void MessengerWindow::loadOwnStatus()
{

	std::string ownId = rsPeers->getOwnId();

	StatusInfo si;
	std::list<StatusInfo> statusList;
	std::list<StatusInfo>::iterator it;

	if (!rsStatus->getStatus(statusList))
	{
		return;
	}

	for(it=statusList.begin();  it != statusList.end(); it++){
		if(it->id == ownId)
			si = *it;
	}


	/* set status mode */
	int statusIndex = 0;
	switch(si.status)
	{
		case RS_STATUS_AWAY:
			statusIndex = 2;
			break;		
		case RS_STATUS_BUSY:
			statusIndex = 1;
			break;
		default:
		case RS_STATUS_ONLINE:
			statusIndex = 0;
			break;
	}
	
	ui.statuscomboBox->setCurrentIndex(statusIndex);
	
}

/** Save own status Online,Away,Busy **/
void MessengerWindow::savestatus()
{

	RsPeerDetails detail;
	std::string ownId = rsPeers->getOwnId();

	if (!rsPeers->getPeerDetails(ownId, detail))
	{
		return;
	}	
		
	StatusInfo si;
	int statusIndex;

	if(!isIdle)
		statusIndex = ui.statuscomboBox->currentIndex();
	else
		statusIndex = 3;

	/* Check if status has changed */
	int status = 0;
	switch(statusIndex)
	{
		case 3:
			status = RS_STATUS_INACTIVE;
			break;
		case 2:
			status = RS_STATUS_AWAY;
			break;
		case 1:
			status = RS_STATUS_BUSY;
			break;
		default:
		case 0:
			status = RS_STATUS_ONLINE;
			break;
	}
	
  si.id = ownId;
	si.status = status;

	rsStatus->sendStatus(si);
	
	return;
}

void MessengerWindow::checkAndSetIdle(int idleTime){

        if((idleTime >= (int) maxTimeBeforeIdle) && !isIdle){
		setIdle(true);
	}else
                if((idleTime < (int) maxTimeBeforeIdle) && isIdle){
			setIdle(false);
		}

	return;
}

void MessengerWindow::setIdle(bool idle){
	isIdle = idle;
}

/* clear Filter */
void MessengerWindow::clearFilter()
{
    ui.filterPatternLineEdit->clear();
    ui.filterPatternLineEdit->setFocus();
}

void MessengerWindow::filterRegExpChanged()
{

    QString text = ui.filterPatternLineEdit->text();

    if (text.isEmpty()) {
        ui.clearButton->hide();
    } else {
        ui.clearButton->show();
    }

    FilterItems();
}

void MessengerWindow::FilterItems()
{
    QString sPattern = ui.filterPatternLineEdit->text();

    int nCount = ui.messengertreeWidget->topLevelItemCount ();
    for (int nIndex = 0; nIndex < nCount; nIndex++) {
        FilterItem(ui.messengertreeWidget->topLevelItem(nIndex), sPattern);
    }
}

bool MessengerWindow::FilterItem(QTreeWidgetItem *pItem, QString &sPattern)
{
    bool bVisible = true;

    if (sPattern.isEmpty() == false) {
        if (pItem->text(0).contains(sPattern, Qt::CaseInsensitive) == false) {
            bVisible = false;
        }
    }

    int nVisibleChildCount = 0;
    int nCount = pItem->childCount();
    for (int nIndex = 0; nIndex < nCount; nIndex++) {
        if (FilterItem(pItem->child(nIndex), sPattern)) {
            nVisibleChildCount++;
        }
    }

    if (bVisible || nVisibleChildCount) {
        pItem->setHidden(false);
    } else {
        pItem->setHidden(true);
    }

    return (bVisible || nVisibleChildCount);
}
