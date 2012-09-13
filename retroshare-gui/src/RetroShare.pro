CONFIG += qt gui uic qrc resources uitools idle bitdht

# Below is for GXS services.
CONFIG += photoshare 
CONFIG += wikipoos 
CONFIG += thewire 
CONFIG += identities
CONFIG += forumsv2 
CONFIG += posted 
CONFIG += unfinished 
CONFIG += gxsgui 

#CONFIG += pluginmgr 

# Other Disabled Bits.
#CONFIG += framecatcher
#CONFIG += blogs


QT     += network xml script 

TEMPLATE = app
TARGET = RetroShare

DEFINES += RS_RELEASE_VERSION
RCC_DIR = temp/qrc
UI_DIR  = temp/ui
MOC_DIR = temp/moc

CONFIG += debug
debug {
	QMAKE_CFLAGS += -g
	QMAKE_CXXFLAGS -= -O2
	QMAKE_CXXFLAGS += -O0
	QMAKE_CFLAGS -= -O2
	QMAKE_CFLAGS += -O0
}

INCLUDEPATH *= retroshare-gui

# treat warnings as error for better removing
#QMAKE_CFLAGS += -Werror
#QMAKE_CXXFLAGS += -Werror

################################# Linux ##########################################
# Put lib dir in QMAKE_LFLAGS so it appears before -L/usr/lib
linux-* {
	#CONFIG += version_detail_bash_script
	QMAKE_CXXFLAGS *= -D_FILE_OFFSET_BITS=64

	PRE_TARGETDEPS *= ../../libretroshare/src/lib/libretroshare.a
	PRE_TARGETDEPS *= ../../openpgpsdk/src/lib/libops.a

	LIBS += ../../libretroshare/src/lib/libretroshare.a
	LIBS += ../../openpgpsdk/src/lib/libops.a -lbz2
	LIBS += -lssl -lupnp -lixml -lXss -lgnome-keyring
	LIBS += -lsqlite3
	LIBS *= -rdynamic
	DEFINES *= HAVE_XSS # for idle time, libx screensaver extensions
	DEFINES *= UBUNTU
}

linux-g++ {
	OBJECTS_DIR = temp/linux-g++/obj
}

linux-g++-64 {
	OBJECTS_DIR = temp/linux-g++-64/obj
}

version_detail_bash_script {
	DEFINES += ADD_LIBRETROSHARE_VERSION_INFO
	QMAKE_EXTRA_TARGETS += write_version_detail
	PRE_TARGETDEPS = write_version_detail
	write_version_detail.commands = ./version_detail.sh
}

install_rs {
	INSTALLS += binary_rs
	binary_rs.path = $$(PREFIX)/usr/bin
	binary_rs.files = ./RetroShare
}

#################### Cross compilation for windows under Linux ###################

win32-x-g++ {
		OBJECTS_DIR = temp/win32-x-g++/obj

		LIBS += ../../libretroshare/src/lib.win32xgcc/libretroshare.a
		LIBS += ../../../../lib/win32-x-g++-v0.5/libssl.a
		LIBS += ../../../../lib/win32-x-g++-v0.5/libcrypto.a
		LIBS += ../../../../lib/win32-x-g++-v0.5/libgpgme.dll.a
		LIBS += ../../../../lib/win32-x-g++-v0.5/libminiupnpc.a
		LIBS += ../../../../lib/win32-x-g++-v0.5/libz.a
		LIBS += -L${HOME}/.wine/drive_c/pthreads/lib -lpthreadGCE2
		LIBS += -lQtUiTools
		LIBS += -lws2_32 -luuid -lole32 -liphlpapi -lcrypt32 -gdi32
		LIBS += -lole32 -lwinmm

		DEFINES *= WINDOWS_SYS WIN32 WIN32_CROSS_UBUNTU

		INCLUDEPATH += ../../../../gpgme-1.1.8/src/
		INCLUDEPATH += ../../../../libgpg-error-1.7/src/

		RC_FILE = gui/images/retroshare_win.rc
}

#################################### Windows #####################################

win32 {

        # Switch on extra warnings
        QMAKE_CFLAGS += -Wextra
        QMAKE_CXXFLAGS += -Wextra

        # Switch off optimization for release version
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE += -O0
        QMAKE_CFLAGS_RELEASE -= -O2
        QMAKE_CFLAGS_RELEASE += -O0

        # Switch on optimization for debug version
        #QMAKE_CXXFLAGS_DEBUG += -O2
        #QMAKE_CFLAGS_DEBUG += -O2

#    PRE_TARGETDEPS += ../../libretroshare/src/lib/libretroshare.a
    PRE_TARGETDEPS += ../../libretroshare/libretroshare-build-desktop/lib/libretroshare.a

    LIBS += ../../libretroshare/libretroshare-build-desktop/lib/libretroshare.a
    LIBS += C:\Development\Rs\v0.5-gxs-b1\openpgpsdk\openpgpsdk-build-desktop\lib\libops.a
    LIBS += C:\Development\Libraries\sqlite\sqlite-autoconf-3070900\lib\libsqlite3.a
    LIBS += -L"../../../lib"
    LIBS += -lssl -lcrypto -lgpgme -lpthreadGC2d -lminiupnpc -lz -lbz2
# added after bitdht
#    LIBS += -lws2_32
        LIBS += -luuid -lole32 -liphlpapi -lcrypt32-cygwin -lgdi32
        LIBS += -lole32 -lwinmm
        RC_FILE = gui/images/retroshare_win.rc

        # export symbols for the plugins
        #LIBS += -Wl,--export-all-symbols,--out-implib,lib/libretroshare-gui.a

    GPG_ERROR_DIR = ../../../../libgpg-error-1.7
    GPGME_DIR  = ../../../../gpgme-1.1.8
    GPG_ERROR_DIR = ../../../../lib/libgpg-error-1.7
    GPGME_DIR  = ../../../../lib/gpgme-1.1.8
    INCLUDEPATH += . $${GPGME_DIR}/src $${GPG_ERROR_DIR}/src



}

##################################### MacOS ######################################

macx {
    # ENABLE THIS OPTION FOR Univeral Binary BUILD.
    	CONFIG += ppc x86
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4

	CONFIG += version_detail_bash_script
	LIBS += ../../libretroshare/src/lib/libretroshare.a
	LIBS += ../../openpgpsdk/src/lib/libops.a -lbz2
        LIBS += -lssl -lcrypto -lz 
        #LIBS += -lssl -lcrypto -lz -lgpgme -lgpg-error -lassuan
	LIBS += ../../../miniupnpc-1.0/libminiupnpc.a
	LIBS += -framework CoreFoundation
	LIBS += -framework Security

    	INCLUDEPATH += .
	#DEFINES* = MAC_IDLE # for idle feature
	CONFIG -= uitools


}

##################################### FreeBSD ######################################

freebsd-* {
	INCLUDEPATH *= /usr/local/include/gpgme
	LIBS *= ../../libretroshare/src/lib/libretroshare.a
	LIBS *= -lssl
	LIBS *= -lgpgme
	LIBS *= -lupnp
	LIBS *= -lgnome-keyring
	PRE_TARGETDEPS *= ../../libretroshare/src/lib/libretroshare.a
}

############################## Common stuff ######################################

# On Linux systems that alredy have libssl and libcrypto it is advisable
# to rename the patched version of SSL to something like libsslxpgp.a and libcryptoxpg.a

# ###########################################

bitdht {
        LIBS += ../../libbitdht/libbitdht-build-desktop/lib/libbitdht.a
        PRE_TARGETDEPS *= ../../libbitdht/libbitdht-build-desktop/lib/libbitdht.a
}

win32 {
# must be added after bitdht
    LIBS += -lws2_32
}

DEPENDPATH += . \
            rsiface \
            control \
            gui \
            lang \
            util \
            gui/bwgraph \
            gui/chat \
            gui/connect \
            gui/images \
            gui/common \
            gui/toaster \
            gui/help/browser \
            gui/elastic

INCLUDEPATH += ../../libretroshare/src/

# Input
HEADERS +=  rshare.h \
            retroshare-gui/configpage.h \
            retroshare-gui/RsAutoUpdatePage.h \
            retroshare-gui/mainpage.h \
            gui/notifyqt.h \
            control/bandwidthevent.h \
            control/eventtype.h \
            gui/QuickStartWizard.h \
            gui/DetailsDialog.h \
            gui/DLListDelegate.h \
            gui/ULListDelegate.h \
            gui/StartDialog.h \
            gui/NetworkDialog.h \
            gui/GenCertDialog.h \
            gui/TransfersDialog.h \
            gui/graphframe.h \
            gui/linetypes.h \
            gui/mainpagestack.h \
            gui/MainWindow.h \
            gui/RSHumanReadableDelegate.h \
            gui/TurtleRouterDialog.h \
            gui/TurtleRouterStatistics.h \
            gui/AboutDialog.h \
            gui/ForumsDialog.h \
            gui/forums/ForumDetails.h \
            gui/forums/EditForumDetails.h \
            gui/forums/CreateForum.h \
            gui/forums/CreateForumMsg.h \
            gui/NetworkView.h \
            gui/TrustView.h \
            gui/MessengerWindow.h \
            gui/FriendsDialog.h \
            gui/FriendRecommendDialog.h \
            gui/RemoteDirModel.h \
            gui/RetroShareLink.h \
            gui/SearchTreeWidget.h \
            gui/SearchDialog.h \
            gui/SharedFilesDialog.h \
            gui/ShareManager.h \
            gui/ShareDialog.h \
            gui/SFListDelegate.h \
            gui/SoundManager.h \
            gui/FileTransferInfoWidget.h \
            gui/HelpDialog.h \
            gui/InfoDialog.h \
            gui/LogoBar.h \
            gui/xprogressbar.h \
            gui/plugins/PluginInterface.h \
            gui/im_history/ImHistoryBrowser.h \
            gui/im_history/IMHistoryItemDelegate.h \
            gui/im_history/IMHistoryItemPainter.h \
            lang/languagesupport.h \
            util/stringutil.h \
            util/win32.h \
            util/RetroStyleLabel.h \
            util/dllexport.h \
            util/NonCopyable.h \
            util/rsutildll.h \
            util/dllexport.h \
            util/global.h \
            util/rsqtutildll.h \
            util/Interface.h \
            util/PixmapMerging.h \
            util/MouseEventFilter.h \
            util/EventFilter.h \
            util/EventReceiver.h \
            util/Widget.h \
            util/rsversion.h \
            util/RsAction.h \
            util/printpreview.h \
            util/log.h \
            util/misc.h \
            util/HandleRichText.h \
            util/ObjectPainter.h \
            gui/bwgraph/bwgraph.h \
            gui/profile/ProfileWidget.h \
            gui/profile/StatusMessage.h \
            gui/chat/PopupChatWindow.h \
            gui/chat/PopupChatDialog.h \
            gui/chat/ChatTabWidget.h \
            gui/chat/ChatWidget.h \
            gui/chat/ChatDialog.h \
            gui/ChatLobbyWidget.h \
            gui/chat/ChatLobbyDialog.h \
            gui/chat/CreateLobbyDialog.h \
            gui/chat/ChatStyle.h \
            gui/channels/CreateChannel.h \
            gui/channels/ChannelDetails.h \
            gui/channels/CreateChannelMsg.h \
            gui/channels/EditChanDetails.h \
            gui/channels/ShareKey.h \
            gui/connect/ConfCertDialog.h \
            gui/connect/FriendRequest.h \
            gui/msgs/MessageComposer.h \
            gui/msgs/MessageWindow.h \
            gui/msgs/MessageWidget.h \
            gui/msgs/TagsMenu.h \
            gui/msgs/textformat.h \
            gui/images/retroshare_win.rc.h \
            gui/settings/rsharesettings.h \
            gui/settings/RsharePeerSettings.h \
            gui/settings/rsettings.h \
            gui/settings/rsettingswin.h \
            gui/settings/GeneralPage.h \
            gui/settings/DirectoriesPage.h \
            gui/settings/ServerPage.h \
            gui/settings/NetworkPage.h \
            gui/settings/NotifyPage.h \
            gui/settings/CryptoPage.h \
            gui/settings/MessagePage.h \
            gui/settings/NewTag.h \
            gui/settings/ForumPage.h \
            gui/settings/PluginsPage.h \
            gui/settings/PluginItem.h \
            gui/settings/AppearancePage.h \
            gui/settings/FileAssociationsPage.h \
            gui/settings/SoundPage.h \
            gui/settings/TransferPage.h \
            gui/settings/ChatPage.h \
            gui/settings/RelayPage.h \
            gui/settings/AddFileAssociationDialog.h \
            gui/toaster/MessageToaster.h \
            gui/toaster/OnlineToaster.h \
            gui/toaster/DownloadToaster.h \
            gui/toaster/ChatToaster.h \
            gui/toaster/GroupChatToaster.h \
            gui/toaster/ChatLobbyToaster.h \
            gui/toaster/FriendRequestToaster.h \
            gui/common/vmessagebox.h \
            gui/common/RsUrlHandler.h \
            gui/common/RsCollectionFile.h \
            gui/common/RsCollectionDialog.h \
            gui/common/rwindow.h \
            gui/common/html.h \
            gui/common/AvatarDefs.h \
            gui/common/StatusDefs.h \
            gui/common/TagDefs.h \
            gui/common/GroupDefs.h \
            gui/common/Emoticons.h \
            gui/common/RSListWidgetItem.h \
            gui/common/RSTreeWidgetItem.h \
            gui/common/RSTabWidget.h \
            gui/common/RSItemDelegate.h \
            gui/common/PeerDefs.h \
            gui/common/FilesDefs.h \
            gui/common/PopularityDefs.h \
            gui/common/GroupTreeWidget.h \
            gui/common/RSTreeView.h \
            gui/common/AvatarWidget.h \
            gui/common/FriendList.h \
            gui/common/FriendSelectionWidget.h \
            gui/common/HashBox.h \
            gui/common/LineEditClear.h \
            gui/common/DropLineEdit.h \
            gui/common/LinkTextBrowser.h \
            gui/style/RSStyle.h \
            gui/style/StyleDialog.h \
            gui/MessagesDialog.h \
            gui/help/browser/helpbrowser.h \
            gui/help/browser/helptextbrowser.h \
            gui/statusbar/peerstatus.h \
            gui/statusbar/natstatus.h \
            gui/statusbar/dhtstatus.h \
            gui/statusbar/ratesstatus.h \
            gui/statusbar/hashingstatus.h \
            gui/statusbar/discstatus.h \
            gui/statusbar/SoundStatus.h \
            gui/advsearch/advancedsearchdialog.h \
            gui/advsearch/expressionwidget.h \
            gui/advsearch/guiexprelement.h \
            gui/elastic/graphwidget.h \
            gui/elastic/edge.h \
            gui/elastic/arrow.h \
            gui/elastic/node.h \
            gui/NewsFeed.h \
            gui/ChannelFeed.h \
            gui/feeds/FeedHolder.h \
            gui/feeds/ForumNewItem.h \
            gui/feeds/ForumMsgItem.h \
            gui/feeds/PeerItem.h \
            gui/feeds/MsgItem.h \
            gui/feeds/ChatMsgItem.h \
            gui/feeds/ChanNewItem.h \
            gui/feeds/ChanMsgItem.h \
            gui/feeds/SubFileItem.h \
            gui/feeds/SubDestItem.h \
            gui/feeds/AttachFileItem.h \
            gui/feeds/SecurityItem.h \
            gui/connect/ConnectFriendWizard.h \
            gui/groups/CreateGroup.h \
            gui/dht/DhtWindow.h \
            gui/bwctrl/BwCtrlWindow.h \
            gui/GetStartedDialog.h



FORMS +=    gui/StartDialog.ui \
            gui/GenCertDialog.ui \
            gui/AboutDialog.ui \
            gui/QuickStartWizard.ui \
            gui/NetworkDialog.ui \
            gui/TransfersDialog.ui \
            gui/ForumsDialog.ui \
            gui/MainWindow.ui \
            gui/TurtleRouterDialog.ui \
            gui/TurtleRouterStatistics.ui \
            gui/forums/CreateForum.ui \
            gui/forums/CreateForumMsg.ui \
            gui/forums/ForumDetails.ui \
            gui/forums/EditForumDetails.ui \
            gui/NetworkView.ui \
            gui/TrustView.ui \
            gui/MessengerWindow.ui \
            gui/FriendsDialog.ui \
            gui/SearchDialog.ui \
            gui/SharedFilesDialog.ui \
            gui/ShareManager.ui \
            gui/ShareDialog.ui \
            gui/MessagesDialog.ui \
            gui/help/browser/helpbrowser.ui \
            gui/HelpDialog.ui \
            gui/InfoDialog.ui \
            gui/DetailsDialog.ui \
            gui/FriendRecommendDialog.ui \
            gui/bwgraph/bwgraph.ui \
            gui/profile/ProfileWidget.ui \
            gui/profile/StatusMessage.ui \
            gui/channels/CreateChannel.ui \
            gui/channels/CreateChannelMsg.ui \
            gui/channels/ChannelDetails.ui \
            gui/channels/EditChanDetails.ui \
            gui/channels/ShareKey.ui \
            gui/chat/PopupChatWindow.ui \
            gui/chat/PopupChatDialog.ui \
            gui/chat/ChatTabWidget.ui \
            gui/chat/ChatWidget.ui \
            gui/chat/ChatLobbyDialog.ui \
            gui/chat/CreateLobbyDialog.ui \
            gui/ChatLobbyWidget.ui \
            gui/connect/ConfCertDialog.ui \
            gui/connect/FriendRequest.ui \
            gui/connect/ConnectFriendWizard.ui \
            gui/msgs/MessageComposer.ui \
            gui/msgs/MessageWindow.ui\
            gui/msgs/MessageWidget.ui\
            gui/settings/settings.ui \
            gui/settings/GeneralPage.ui \
            gui/settings/DirectoriesPage.ui \
            gui/settings/ServerPage.ui \
            gui/settings/NetworkPage.ui \
            gui/settings/NotifyPage.ui \
            gui/settings/CryptoPage.ui \
            gui/settings/MessagePage.ui \
            gui/settings/NewTag.ui \
            gui/settings/ForumPage.ui \
            gui/settings/PluginsPage.ui \
            gui/settings/AppearancePage.ui \
            gui/settings/TransferPage.ui \
            gui/settings/SoundPage.ui \
            gui/settings/ChatPage.ui \
            gui/settings/RelayPage.ui \
            gui/settings/PluginItem.ui \
            gui/toaster/MessageToaster.ui \
            gui/toaster/OnlineToaster.ui \
            gui/toaster/DownloadToaster.ui \
            gui/toaster/ChatToaster.ui \
            gui/toaster/GroupChatToaster.ui \
            gui/toaster/ChatLobbyToaster.ui \
            gui/toaster/FriendRequestToaster.ui \
            gui/advsearch/AdvancedSearchDialog.ui \
            gui/advsearch/expressionwidget.ui \
            gui/NewsFeed.ui \
            gui/ChannelFeed.ui \
            gui/feeds/ForumNewItem.ui \
            gui/feeds/ForumMsgItem.ui \
            gui/feeds/PeerItem.ui \
            gui/feeds/MsgItem.ui \
            gui/feeds/ChatMsgItem.ui \
            gui/feeds/ChanNewItem.ui \
            gui/feeds/ChanMsgItem.ui \
            gui/feeds/SubFileItem.ui \
            gui/feeds/SubDestItem.ui \
            gui/feeds/AttachFileItem.ui \
            gui/feeds/SecurityItem.ui \
            gui/im_history/ImHistoryBrowser.ui \
            gui/groups/CreateGroup.ui \
            gui/common/GroupTreeWidget.ui \
            gui/common/AvatarWidget.ui \
            gui/common/FriendList.ui \
            gui/common/FriendSelectionWidget.ui \
            gui/common/HashBox.ui \
            gui/common/RsCollectionDialog.ui \
            gui/style/StyleDialog.ui \
            gui/dht/DhtWindow.ui \
            gui/bwctrl/BwCtrlWindow.ui \
            gui/GetStartedDialog.ui \

SOURCES +=  main.cpp \
            rshare.cpp \
            gui/notifyqt.cpp \
            gui/AboutDialog.cpp \
            gui/QuickStartWizard.cpp \
            gui/DetailsDialog.cpp \
            gui/DLListDelegate.cpp \
            gui/ULListDelegate.cpp \
            gui/StartDialog.cpp \
            gui/GenCertDialog.cpp \
            gui/NetworkDialog.cpp \
            gui/TransfersDialog.cpp \
            gui/graphframe.cpp \
            gui/mainpagestack.cpp \
            gui/TurtleRouterDialog.cpp \
            gui/TurtleRouterStatistics.cpp \
            gui/MainWindow.cpp \
            gui/ForumsDialog.cpp \
            gui/forums/ForumDetails.cpp \
            gui/forums/EditForumDetails.cpp \
            gui/forums/CreateForum.cpp \
            gui/forums/CreateForumMsg.cpp \
            gui/NetworkView.cpp \
            gui/TrustView.cpp \
            gui/MessengerWindow.cpp \
            gui/FriendsDialog.cpp \
            gui/FriendRecommendDialog.cpp \
            gui/RemoteDirModel.cpp \
            gui/RsAutoUpdatePage.cpp \
            gui/RetroShareLink.cpp \
            gui/SearchTreeWidget.cpp \
            gui/SearchDialog.cpp \
            gui/SharedFilesDialog.cpp \
            gui/ShareManager.cpp \
            gui/ShareDialog.cpp \
            gui/SFListDelegate.cpp \
            gui/SoundManager.cpp \
            gui/MessagesDialog.cpp \
            gui/FileTransferInfoWidget.cpp \
            gui/im_history/ImHistoryBrowser.cpp \
            gui/im_history/IMHistoryItemDelegate.cpp \
            gui/im_history/IMHistoryItemPainter.cpp \
            gui/help/browser/helpbrowser.cpp \
            gui/help/browser/helptextbrowser.cpp \
            gui/HelpDialog.cpp \
            gui/InfoDialog.cpp \
            gui/LogoBar.cpp \
            gui/xprogressbar.cpp \
            lang/languagesupport.cpp \
            util/stringutil.cpp \
            util/win32.cpp \
            util/RetroStyleLabel.cpp \
            util/WidgetBackgroundImage.cpp \
            util/NonCopyable.cpp \
            util/PixmapMerging.cpp \
            util/MouseEventFilter.cpp \
            util/EventFilter.cpp \
            util/EventReceiver.cpp \
            util/Widget.cpp \
            util/RsAction.cpp \
            util/rsversion.cpp \
            util/printpreview.cpp \
            util/log.cpp \
            util/misc.cpp \
            util/HandleRichText.cpp \
            util/ObjectPainter.cpp \
            gui/bwgraph/bwgraph.cpp \
            gui/profile/ProfileWidget.cpp \
            gui/profile/StatusMessage.cpp \
            gui/channels/CreateChannel.cpp \
            gui/channels/CreateChannelMsg.cpp \
            gui/channels/ChannelDetails.cpp \
            gui/channels/EditChanDetails.cpp \
            gui/channels/ShareKey.cpp \
            gui/chat/PopupChatWindow.cpp \
            gui/chat/PopupChatDialog.cpp \
            gui/chat/ChatTabWidget.cpp \
            gui/chat/ChatWidget.cpp \
            gui/chat/ChatDialog.cpp \
            gui/ChatLobbyWidget.cpp \
            gui/chat/ChatLobbyDialog.cpp \
            gui/chat/CreateLobbyDialog.cpp \
            gui/chat/ChatStyle.cpp \
            gui/connect/ConfCertDialog.cpp \
            gui/connect/FriendRequest.cpp \
            gui/msgs/MessageComposer.cpp \
            gui/msgs/MessageWidget.cpp \
            gui/msgs/MessageWindow.cpp \
            gui/msgs/TagsMenu.cpp \
            gui/common/vmessagebox.cpp \
            gui/common/RsCollectionFile.cpp \
            gui/common/RsCollectionDialog.cpp \
            gui/common/RsUrlHandler.cpp \
            gui/common/rwindow.cpp \
            gui/common/html.cpp \
            gui/common/AvatarDefs.cpp \
            gui/common/StatusDefs.cpp \
            gui/common/TagDefs.cpp \
            gui/common/GroupDefs.cpp \
            gui/common/Emoticons.cpp \
            gui/common/RSListWidgetItem.cpp \
            gui/common/RSTreeWidgetItem.cpp \
            gui/common/RSTabWidget.cpp \
            gui/common/RSItemDelegate.cpp \
            gui/common/PeerDefs.cpp \
            gui/common/FilesDefs.cpp \
            gui/common/PopularityDefs.cpp \
            gui/common/GroupTreeWidget.cpp \
            gui/common/RSTreeView.cpp \
            gui/common/AvatarWidget.cpp \
            gui/common/FriendList.cpp \
            gui/common/FriendSelectionWidget.cpp \
            gui/common/HashBox.cpp \
            gui/common/LineEditClear.cpp \
            gui/common/DropLineEdit.cpp \
            gui/common/LinkTextBrowser.cpp \
            gui/style/RSStyle.cpp \
            gui/style/StyleDialog.cpp \
            gui/settings/rsharesettings.cpp \
            gui/settings/RsharePeerSettings.cpp \
            gui/settings/rsettings.cpp \
            gui/settings/rsettingswin.cpp \
            gui/settings/GeneralPage.cpp \
            gui/settings/DirectoriesPage.cpp \
            gui/settings/ServerPage.cpp \
            gui/settings/NetworkPage.cpp \
            gui/settings/NotifyPage.cpp \
            gui/settings/CryptoPage.cpp \
            gui/settings/MessagePage.cpp \
            gui/settings/NewTag.cpp \
            gui/settings/ForumPage.cpp \
            gui/settings/PluginsPage.cpp \
            gui/settings/PluginItem.cpp \
            gui/settings/AppearancePage.cpp \
            gui/settings/FileAssociationsPage.cpp \
            gui/settings/SoundPage.cpp \
            gui/settings/TransferPage.cpp \
            gui/settings/ChatPage.cpp \
            gui/settings/RelayPage.cpp \
            gui/settings/AddFileAssociationDialog.cpp \
            gui/statusbar/peerstatus.cpp \
            gui/statusbar/natstatus.cpp \
            gui/statusbar/dhtstatus.cpp \
            gui/statusbar/ratesstatus.cpp \
            gui/statusbar/hashingstatus.cpp \
            gui/statusbar/discstatus.cpp \
            gui/statusbar/SoundStatus.cpp \
            gui/toaster/MessageToaster.cpp \
            gui/toaster/DownloadToaster.cpp \
            gui/toaster/OnlineToaster.cpp \
            gui/toaster/ChatToaster.cpp \
            gui/toaster/GroupChatToaster.cpp \
            gui/toaster/ChatLobbyToaster.cpp \
            gui/toaster/FriendRequestToaster.cpp \
            gui/advsearch/advancedsearchdialog.cpp \
            gui/advsearch/expressionwidget.cpp \
            gui/advsearch/guiexprelement.cpp \
            gui/elastic/graphwidget.cpp \
            gui/elastic/edge.cpp \
            gui/elastic/arrow.cpp \
            gui/elastic/node.cpp \
            gui/NewsFeed.cpp \
            gui/ChannelFeed.cpp \
            gui/feeds/ForumNewItem.cpp \
            gui/feeds/ForumMsgItem.cpp \
            gui/feeds/PeerItem.cpp \
            gui/feeds/MsgItem.cpp \
            gui/feeds/ChatMsgItem.cpp \
            gui/feeds/ChanNewItem.cpp \
            gui/feeds/ChanMsgItem.cpp \
            gui/feeds/SubFileItem.cpp \
            gui/feeds/SubDestItem.cpp \
            gui/feeds/AttachFileItem.cpp \
            gui/feeds/SecurityItem.cpp \
            gui/connect/ConnectFriendWizard.cpp \
            gui/groups/CreateGroup.cpp \
            gui/dht/DhtWindow.cpp \
            gui/bwctrl/BwCtrlWindow.cpp \
            gui/GetStartedDialog.cpp


RESOURCES += gui/images.qrc lang/lang.qrc gui/help/content/content.qrc

TRANSLATIONS +=  \
            lang/retroshare_en.ts \
            lang/retroshare_es.ts \
            lang/retroshare_da.ts \
            lang/retroshare_de.ts \
			lang/retroshare_fi.ts \
            lang/retroshare_fr.ts \
            lang/retroshare_ja_JP.ts  \
            lang/retroshare_ko.ts  \
            lang/retroshare_ru.ts  \
            lang/retroshare_tr.ts \
            lang/retroshare_sv.ts \
            lang/retroshare_zh_CN.ts

unfinishedtranslations {

       TRANSLATIONS +=  \
            lang/retroshare_bg.ts \
            lang/retroshare_af.ts  \
            lang/retroshare_gr.ts  \
            lang/retroshare_it.ts  \
            lang/retroshare_nl.ts \
            lang/retroshare_pl.ts  \
            lang/retroshare_pt.ts  \
            lang/retroshare_sl.ts \
            lang/retroshare_sr.ts \
            lang/retroshare_zh_TW.ts

}

# Shifted Qt4.4 dependancies to here.
#    qmake CONFIG=pluginmgr

pluginmgr {

        SOURCES += gui/PluginsPage.cpp \
            	gui/PluginManagerWidget.cpp \
            	gui/PluginManager.cpp

        HEADERS += gui/PluginsPage.h  \
            	gui/PluginManagerWidget.h \
            	gui/PluginManager.h

        DEFINES *= PLUGINMGR

}

blogs {

DEPENDPATH += gui/unfinished \

HEADERS += gui/unfinished/blogs/BlogsDialog.h \
           gui/unfinished/blogs/CreateBlog.h \
           gui/unfinished/blogs/CreateBlogMsg.h \
           gui/unfinished/blogs/BlogsMsgItem.h \
           gui/unfinished/blogs/BlogDetails.h \
           gui/feeds/BlogNewItem.h \
           gui/feeds/BlogMsgItem.h \

FORMS += gui/unfinished/blogs/BlogsDialog.ui \
         gui/unfinished/blogs/CreateBlog.ui \
         gui/unfinished/blogs/CreateBlogMsg.ui \
         gui/unfinished/blogs/BlogsMsgItem.ui \
         gui/unfinished/blogs/BlogDetails.ui \
         gui/feeds/BlogNewItem.ui \
         gui/feeds/BlogMsgItem.ui \

SOURCES += gui/unfinished/blogs/BlogsDialog.cpp \
           gui/unfinished/blogs/CreateBlog.cpp \
           gui/unfinished/blogs/CreateBlogMsg.cpp \
           gui/unfinished/blogs/BlogsMsgItem.cpp \
           gui/unfinished/blogs/BlogDetails.cpp \
           gui/feeds/BlogNewItem.cpp \
           gui/feeds/BlogMsgItem.cpp \

DEFINES += BLOGS
}

# use_links {
# HEADERS += gui/AddLinksDialog.h \
#            gui/LinksDialog.h
# 
# FORMS += gui/AddLinksDialog.ui \
#          gui/LinksDialog.ui
# 
# SOURCES += gui/AddLinksDialog.cpp \
#            gui/LinksDialog.cpp
# 
# DEFINES += RS_USE_LINKS
# }


idle {

HEADERS += idle/idle.h

SOURCES += idle/idle.cpp \
	   idle/idle_platform.cpp
}

framecatcher {

HEADERS += util/framecatcher.h

SOURCES += util/framecatcher.cpp

LIBS += -lxine

DEFINES *= CHANNELS_FRAME_CATCHER

}

# BELOW IS GXS New Services.

unfinished {

DEPENDPATH += gui/unfinished \

HEADERS += gui/unfinished/ApplicationWindow.h \
           gui/unfinished/CalDialog.h \
           gui/unfinished/ExampleDialog.h \
           gui/unfinished/GamesDialog.h \

#           gui/unfinished/profile/ProfileView.h \
#           gui/unfinished/profile/ProfileEdit.h
#           gui/unfinished/StatisticDialog.h \
#           gui/unfinished/PhotoDialog.h \
#           gui/unfinished/PhotoShow.h \

FORMS += gui/unfinished/ApplicationWindow.ui \
         gui/unfinished/CalDialog.ui \
         gui/unfinished/ExampleDialog.ui \
         gui/unfinished/GamesDialog.ui \

#         gui/unfinished/profile/ProfileView.ui \
#         gui/unfinished/profile/ProfileEdit.ui
#         gui/unfinished/StatisticDialog.ui \
#         gui/unfinished/PhotoDialog.ui \
#         gui/unfinished/PhotoShow.ui \

SOURCES += gui/unfinished/ApplicationWindow.cpp \
           gui/unfinished/CalDialog.cpp \
           gui/unfinished/ExampleDialog.cpp \
           gui/unfinished/GamesDialog.cpp \

#           gui/unfinished/profile/ProfileView.cpp \
#           gui/unfinished/profile/ProfileEdit.cpp
#           gui/unfinished/StatisticDialog.cpp \
#           gui/unfinished/PhotoDialog.cpp \
#           gui/unfinished/PhotoShow.cpp \

          DEFINES *= UNFINISHED
}


photoshare {

	HEADERS += \
                gui/PhotoShare/PhotoDialog.h \
		gui/PhotoShare/PhotoDrop.h \
		gui/PhotoShare/PhotoSlideShow.h \
                util/TokenQueueV2.h \
                gui/PhotoShare/AlbumDialog.h \
                gui/PhotoShare/AlbumItem.h \
                gui/PhotoShare/PhotoItem.h \
                gui/PhotoShare/AlbumCreateDialog.h \
                gui/PhotoShare/PhotoShareItemHolder.h \
                gui/PhotoShare/PhotoShare.h

#gui/PhotoShare/PhotoDetailsDialog.h \
# gui/PhotoShare/PhotoAddDialog.h \
	FORMS += \
                gui/PhotoShare/PhotoDialog.ui \
                gui/PhotoShare/PhotoSlideShow.ui \
                gui/PhotoShare/AlbumItem.ui \
                gui/PhotoShare/AlbumDialog.ui \
                gui/PhotoShare/PhotoItem.ui \
                gui/PhotoShare/AlbumCreateDialog.ui \
                gui/PhotoShare/PhotoShare.ui
# gui/PhotoShare/PhotoAddDialog.ui \1
# gui/PhotoShare/PhotoDetailsDialog.ui \
	SOURCES += \
                gui/PhotoShare/PhotoDialog.cpp \
		gui/PhotoShare/PhotoDrop.cpp \
		gui/PhotoShare/PhotoSlideShow.cpp \
                gui/PhotoShare/AlbumDialog.cpp \
                util/TokenQueueV2.cpp \
                gui/PhotoShare/AlbumItem.cpp \
                gui/PhotoShare/PhotoItem.cpp \
                gui/PhotoShare/AlbumCreateDialog.cpp \
                gui/PhotoShare/PhotoShareItemHolder.cpp \
                gui/PhotoShare/PhotoShare.cpp
# gui/PhotoShare/PhotoAddDialog.cpp \
# gui/PhotoShare/PhotoDetailsDialog.cpp \
}


wikipoos {

	HEADERS += gui/WikiPoos/WikiDialog.h \
		gui/WikiPoos/WikiAddDialog.h \
		gui/WikiPoos/WikiEditDialog.h \

	FORMS += gui/WikiPoos/WikiDialog.ui \
		gui/WikiPoos/WikiAddDialog.ui \
		gui/WikiPoos/WikiEditDialog.ui \

	SOURCES += gui/WikiPoos/WikiDialog.cpp \
		gui/WikiPoos/WikiAddDialog.cpp \
		gui/WikiPoos/WikiEditDialog.cpp \

}



thewire {

	HEADERS += gui/TheWire/PulseItem.h \
		gui/TheWire/WireDialog.h \
		gui/TheWire/PulseAddDialog.h \

	FORMS += gui/TheWire/PulseItem.ui \
		gui/TheWire/WireDialog.ui \
		gui/TheWire/PulseAddDialog.ui \

	SOURCES += gui/TheWire/PulseItem.cpp \
		gui/TheWire/WireDialog.cpp \
		gui/TheWire/PulseAddDialog.cpp \

}

identities {

	HEADERS += util/TokenQueue.h \
		gui/Identity/IdDialog.h \
		gui/Identity/IdEditDialog.h \

	FORMS += gui/Identity/IdDialog.ui \
		gui/Identity/IdEditDialog.ui \

        SOURCES += util/TokenQueue.cpp \
		gui/Identity/IdDialog.cpp \
		gui/Identity/IdEditDialog.cpp \

}


forumsv2 {

	HEADERS += gui/ForumsV2Dialog.h \
            gui/forumsv2/ForumV2Details.h \
            gui/forumsv2/EditForumV2Details.h \
            gui/forumsv2/CreateForumV2.h \
            gui/forumsv2/CreateForumV2Msg.h \

	FORMS += gui/ForumsV2Dialog.ui \
            gui/forumsv2/ForumV2Details.ui \
            gui/forumsv2/EditForumV2Details.ui \
            gui/forumsv2/CreateForumV2.ui \
            gui/forumsv2/CreateForumV2Msg.ui \

	SOURCES += gui/ForumsV2Dialog.cpp \
            gui/forumsv2/ForumV2Details.cpp \
            gui/forumsv2/EditForumV2Details.cpp \
            gui/forumsv2/CreateForumV2.cpp \
            gui/forumsv2/CreateForumV2Msg.cpp \

}


posted {

	HEADERS += gui/Posted/PostedDialog.h \
            gui/Posted/PostedListDialog.h \
            gui/Posted/PostedItem.h \
            gui/Posted/PostedComments.h \

	FORMS += gui/Posted/PostedDialog.ui \
            gui/Posted/PostedListDialog.ui \
            gui/Posted/PostedItem.ui \
            gui/Posted/PostedComments.ui \

	SOURCES += gui/Posted/PostedDialog.cpp \
            gui/Posted/PostedListDialog.cpp \
            gui/Posted/PostedItem.cpp \
            gui/Posted/PostedComments.cpp \

}

gxsgui {

	HEADERS += gui/gxs/GxsGroupDialog.h \
            gui/gxs/ForumV2GroupDialog.h \
            gui/gxs/WikiGroupDialog.h \
            gui/gxs/PostedGroupDialog.h \
            gui/gxs/GxsCommentTreeWidget.h \
#            gui/gxs/GxsMsgDialog.h \

	FORMS += gui/gxs/GxsGroupDialog.ui \
#	    gui/gxs/GxsMsgDialog.ui \
#	    gui/gxs/GxsCommentTreeWidget.ui \

	SOURCES += gui/gxs/GxsGroupDialog.cpp \
            gui/gxs/ForumV2GroupDialog.cpp \
            gui/gxs/WikiGroupDialog.cpp \
            gui/gxs/PostedGroupDialog.cpp \
            gui/gxs/GxsCommentTreeWidget.cpp \
#            gui/gxs/GxsMsgDialog.cpp \


}
