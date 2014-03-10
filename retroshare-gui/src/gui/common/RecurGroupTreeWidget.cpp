/****************************************************************
 * This file is distributed under the following license:
 *
 * Copyright (c) 2010, RetroShare Team
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

#include <QMenu>
#include <QToolButton>
#include <QMutableListIterator>
#include <QDebug>

#include "RecurGroupTreeWidget.h"
#include "ui_GroupTreeWidget.h"

#include "RSItemDelegate.h"
#include "PopularityDefs.h"
#include "gui/settings/rsharesettings.h"
#include "RSTreeWidgetItem.h"

#include <stdint.h>
#include <iostream>

#define COLUMN_NAME        0
#define COLUMN_POPULARITY  1
#define COLUMN_COUNT       2
#define COLUMN_DATA        COLUMN_NAME

#define ROLE_ID              Qt::UserRole
#define ROLE_NAME            Qt::UserRole + 1
#define ROLE_DESCRIPTION     Qt::UserRole + 2
#define ROLE_POPULARITY      Qt::UserRole + 3
#define ROLE_LASTPOST        Qt::UserRole + 4
#define ROLE_SEARCH_SCORE    Qt::UserRole + 5
#define ROLE_SUBSCRIBE_FLAGS Qt::UserRole + 6
#define ROLE_COLOR           Qt::UserRole + 7
#define ROLE_PARENT_ID       Qt::UserRole + 8

#define FILTER_NAME_INDEX  0
#define FILTER_DESC_INDEX  1

RecurGroupTreeWidget::RecurGroupTreeWidget(QWidget *parent) :
		QWidget(parent), ui(new Ui::GroupTreeWidget)
{
	ui->setupUi(this);

	displayMenu = NULL;
	actionSortAscending = NULL;
//	actionSortDescending = NULL;
	actionSortByName = NULL;
	actionSortByPopularity = NULL;
	actionSortByLastPost = NULL;

	compareRole = new RSTreeWidgetItemCompareRole;
	compareRole->setRole(COLUMN_DATA, ROLE_NAME);

	/* Connect signals */
	connect(ui->filterLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterChanged()));
	connect(ui->filterLineEdit, SIGNAL(filterChanged(int)), this, SLOT(filterChanged()));

	connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
	connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(itemActivated(QTreeWidgetItem*,int)));
	if (!style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick, NULL, this)) {
		// need signal itemClicked too
		connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemActivated(QTreeWidgetItem*,int)));
	}

	/* Add own item delegate */
	RSItemDelegate *itemDelegate = new RSItemDelegate(this);
	itemDelegate->setSpacing(QSize(0, 2));
	ui->treeWidget->setItemDelegate(itemDelegate);

	/* Initialize tree widget */
	ui->treeWidget->setColumnCount(COLUMN_COUNT);

	/* Set header resize modes and initial section sizes */
	QHeaderView *header = ui->treeWidget->header ();
	header->setResizeMode(COLUMN_NAME, QHeaderView::Stretch);
	header->resizeSection(COLUMN_NAME, 170);
	header->setResizeMode(COLUMN_POPULARITY, QHeaderView::Fixed);
	header->resizeSection(COLUMN_POPULARITY, 25);

	/* add filter actions */
	ui->filterLineEdit->addFilter(QIcon(), tr("Title"), FILTER_NAME_INDEX , tr("Search Title"));
	ui->filterLineEdit->addFilter(QIcon(), tr("Description"), FILTER_DESC_INDEX , tr("Search Description"));
	ui->filterLineEdit->setCurrentFilter(FILTER_NAME_INDEX);

	/* Initialize display button */
	initDisplayMenu(ui->displayButton);
}

RecurGroupTreeWidget::~RecurGroupTreeWidget()
{
	delete ui;
}

void RecurGroupTreeWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	case QEvent::StyleChange:
		updateColors();
		break;
	default:
		// remove compiler warnings
		break;
	}
}

void RecurGroupTreeWidget::addToolButton(QToolButton *toolButton)
{
	if (!toolButton) {
		return;
	}

	/* Initialize button */
	toolButton->setAutoRaise(true);
	toolButton->setIconSize(ui->displayButton->iconSize());
	toolButton->setFocusPolicy(ui->displayButton->focusPolicy());

	ui->titleBarFrame->layout()->addWidget(toolButton);
}

void RecurGroupTreeWidget::processSettings(RshareSettings *settings, bool load)
{
	if (settings == NULL) {
		return;
	}

	const int SORTBY_NAME = 1;
	const int SORTBY_POPULRITY = 2;
	const int SORTBY_LASTPOST = 3;

	if (load) {
		// load settings

		// state of sort
		int sortby = settings->value("GroupSortBy").toInt();
		switch (sortby) {
		case SORTBY_NAME:
			if (actionSortByName) {
				actionSortByName->setChecked(true);
			}
			break;
		case SORTBY_POPULRITY:
			if (actionSortByPopularity) {
				actionSortByPopularity->setChecked(true);
			}
			break;
		case SORTBY_LASTPOST:
			if (actionSortByLastPost) {
				actionSortByLastPost->setChecked(true);
			}
			break;
		}
	} else {
		// save settings

		// state of sort
		int sortby = SORTBY_NAME;
		if (actionSortByName && actionSortByName->isChecked()) {
			sortby = SORTBY_NAME;
		} else if (actionSortByPopularity && actionSortByPopularity->isChecked()) {
			sortby = SORTBY_POPULRITY;
		} else if (actionSortByLastPost && actionSortByLastPost->isChecked()) {
			sortby = SORTBY_LASTPOST;
		}
		settings->setValue("GroupSortBy", sortby);
	}
}

void RecurGroupTreeWidget::initDisplayMenu(QToolButton *toolButton)
{
	displayMenu = new QMenu();
//	QActionGroup *actionGroup = new QActionGroup(displayMenu);
//
//	actionSortDescending = displayMenu->addAction(QIcon(":/images/sort_decrease.png"), tr("Sort Descending Order"), this, SLOT(sort()));
//	actionSortDescending->setCheckable(true);
//	actionSortDescending->setActionGroup(actionGroup);
//
//	actionSortAscending = displayMenu->addAction(QIcon(":/images/sort_incr.png"), tr("Sort Ascending Order"), this, SLOT(sort()));
//	actionSortAscending->setCheckable(true);
//	actionSortAscending->setChecked(true); // set standard to sort ascending
//	actionSortAscending->setActionGroup(actionGroup);
//
//	displayMenu->addSeparator();

	QActionGroup *actionGroup = new QActionGroup(displayMenu);
	actionSortByName = displayMenu->addAction(QIcon(), tr("Sort by Name"), this, SLOT(sort()));
	actionSortByName->setCheckable(true);
	actionSortByName->setChecked(true); // set standard to sort by name
	actionSortByName->setActionGroup(actionGroup);

	actionSortByPopularity = displayMenu->addAction(QIcon(), tr("Sort by Popularity"), this, SLOT(sort()));
	actionSortByPopularity->setCheckable(true);
	actionSortByPopularity->setActionGroup(actionGroup);

	actionSortByLastPost = displayMenu->addAction(QIcon(), tr("Sort by Last Post"), this, SLOT(sort()));
	actionSortByLastPost->setCheckable(true);
	actionSortByLastPost->setActionGroup(actionGroup);

	toolButton->setMenu(displayMenu);
}

void RecurGroupTreeWidget::updateColors()
{
	QBrush brush;
	QBrush standardBrush = ui->treeWidget->palette().color(QPalette::Text);

	QTreeWidgetItemIterator itemIterator(ui->treeWidget);
	QTreeWidgetItem *item;
	while ((item = *itemIterator) != NULL) {
		itemIterator++;

		int color = item->data(COLUMN_DATA, ROLE_COLOR).toInt();
		if (color >= 0) {
			brush = QBrush(mTextColor[color]);
		} else {
			brush = standardBrush;
		}

		item->setForeground(COLUMN_NAME, brush);
	}
}

void RecurGroupTreeWidget::customContextMenuRequested(const QPoint &pos)
{
	emit treeCustomContextMenuRequested(pos);
}

void RecurGroupTreeWidget::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Q_UNUSED(previous);

	QString id;

	if (current) {
		id = current->data(COLUMN_DATA, ROLE_ID).toString();
	}

	emit treeCurrentItemChanged(id);
}

void RecurGroupTreeWidget::itemActivated(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(column);

	QString id;

	if (item) {
		id = item->data(COLUMN_DATA, ROLE_ID).toString();
	}

	emit treeItemActivated(id);
}

QTreeWidgetItem *RecurGroupTreeWidget::addCategoryItem(const QString &name, const QIcon &icon, bool expand)
{
	QFont font;
	QTreeWidgetItem *item = new QTreeWidgetItem();
	font = item->font(COLUMN_NAME);
	font.setBold(true);
	font.setPointSize(10);
	item->setText(COLUMN_NAME, name);
	item->setData(COLUMN_DATA, ROLE_NAME, name);
	item->setFont(COLUMN_NAME, font);
	item->setIcon(COLUMN_NAME, icon);
	item->setSizeHint(COLUMN_NAME, QSize(18, 18));
	item->setForeground(COLUMN_NAME, QBrush(textColorCategory()));
	item->setData(COLUMN_DATA, ROLE_COLOR, GROUPTREEWIDGET_COLOR_CATEGORY);

	ui->treeWidget->addTopLevelItem(item);

	item->setExpanded(expand);

	return item;
}

QString RecurGroupTreeWidget::itemId(QTreeWidgetItem *item)
{
	if (item == NULL) {
		return "";
	}

	return item->data(COLUMN_DATA, ROLE_ID).toString();
}

QTreeWidgetItem* RecurGroupTreeWidget::recursiveIdSearch(QTreeWidgetItem *currentItem, const QString &id){
	int childCount = currentItem->childCount();
	for (int child = 0; child < childCount; child++) {
		QTreeWidgetItem *childItem = currentItem->child(child);
		if (childItem->data(COLUMN_DATA, ROLE_ID).toString() == id) {
			/* Found child */
			return childItem;
		}
		QTreeWidgetItem* recur = recursiveIdSearch(childItem, id);
		if(recur) return recur;
	}
	return NULL;
}

void RecurGroupTreeWidget::checkOrphans(QList<QTreeWidgetItem  *> &orphanList, QTreeWidgetItem *justInserted){

	if(orphanList.length()==0)return;
	QString newID = justInserted->data(COLUMN_DATA, ROLE_ID).toString();

	QList<QTreeWidgetItem *> oList = orphanList;
	QMutableListIterator<QTreeWidgetItem *> i(oList);
	while (i.hasNext()) {
		QTreeWidgetItem *orphan = i.next();
		QString parentID = orphan->data(COLUMN_DATA, ROLE_PARENT_ID).toString();
		if (parentID.compare(newID)==0){
			justInserted->addChild(orphan);
			i.remove();
			checkOrphans(orphanList,orphan);
		}
	}
}

void RecurGroupTreeWidget::fillGroupItems(QTreeWidgetItem *categoryItem, const QList<GroupRecurItemInfo> &itemList)
{
	if (categoryItem == NULL) {
		return;
	}

	QString filterText = ui->filterLineEdit->text();

	QList<QTreeWidgetItem *> orphans;

	/* Iterate all items */
	QList<GroupRecurItemInfo>::const_iterator it;
	for (it = itemList.begin(); it != itemList.end(); it++) {
		const GroupRecurItemInfo &itemInfo = *it;

		QTreeWidgetItem *item = recursiveIdSearch(categoryItem, itemInfo.id);

		if (item == NULL) {
			item = new RSTreeWidgetItem(compareRole);
			item->setData(COLUMN_DATA, ROLE_ID, itemInfo.id);
			item->setData(COLUMN_DATA, ROLE_PARENT_ID, itemInfo.parentId);

			if(itemInfo.parentId.length()<10){
				categoryItem->addChild(item);
				checkOrphans(orphans,item);
			}else{
				QTreeWidgetItem *parentItem = recursiveIdSearch(categoryItem, itemInfo.parentId);
				if(parentItem){
					parentItem->addChild(item);
					checkOrphans(orphans,item);
				}else{
					orphans.append(item);
				}
			}
		}

		item->setText(COLUMN_NAME, itemInfo.name);
		item->setData(COLUMN_DATA, ROLE_NAME, itemInfo.name);
		item->setData(COLUMN_DATA, ROLE_DESCRIPTION, itemInfo.description);

		/* Set last post */
		qlonglong lastPost = itemInfo.lastpost.toTime_t();
		item->setData(COLUMN_DATA, ROLE_LASTPOST, -lastPost); // negative for correct sorting

		/* Set icon */
		item->setIcon(COLUMN_NAME, itemInfo.icon);

		/* Set popularity */
		QString tooltip = PopularityDefs::tooltip(itemInfo.popularity);
		item->setIcon(COLUMN_POPULARITY, PopularityDefs::icon(itemInfo.popularity));
		item->setData(COLUMN_DATA, ROLE_POPULARITY, -itemInfo.popularity); // negative for correct sorting

		/* Set tooltip */
		if (itemInfo.privatekey) {
			tooltip += "\n" + tr("Private Key Available");
		}
		item->setToolTip(COLUMN_NAME, tooltip);
		item->setToolTip(COLUMN_POPULARITY, tooltip);

		item->setData(COLUMN_DATA, ROLE_SUBSCRIBE_FLAGS, itemInfo.subscribeFlags);

		/* Set color */
		QBrush brush;
		if (itemInfo.privatekey) {
			brush = QBrush(textColorPrivateKey());
			item->setData(COLUMN_DATA, ROLE_COLOR, GROUPTREEWIDGET_COLOR_PRIVATEKEY);
		} else {
			brush = ui->treeWidget->palette().color(QPalette::Text);
			item->setData(COLUMN_DATA, ROLE_COLOR, GROUPTREEWIDGET_COLOR_STANDARD);
		}
		item->setForeground(COLUMN_NAME, brush);

		/* Calculate score */
		calculateScore(item, filterText);
	}

	/* Add remaning topics whose parents are unknown */
	QMutableListIterator<QTreeWidgetItem *> i(orphans);
	while (i.hasNext()) {
		QTreeWidgetItem *orphan = i.next();
		categoryItem->addChild(orphan);
		std::cerr << "adding orphan\n"<< orphan->data(COLUMN_DATA, ROLE_PARENT_ID).toString().toStdString() <<"\n";
		std::cerr << orphan->data(COLUMN_DATA, ROLE_ID).toString().toStdString() <<"\n";
		std::cerr << "\n";
	}

	/* Remove all items not in list */
	int child = 0;
	int childCount = categoryItem->childCount();
	while (child < childCount) {
		QString id = categoryItem->child(child)->data(COLUMN_DATA, ROLE_ID).toString();

		for (it = itemList.begin(); it != itemList.end(); it++) {
			if (it->id == id) {
				break;
			}
		}

		if (it == itemList.end()) {
			delete(categoryItem->takeChild(child));
			childCount = categoryItem->childCount();
		} else {
			child++;
		}
	}

	resort(categoryItem, NULL);
}

void RecurGroupTreeWidget::setUnreadCount(QTreeWidgetItem *item, int unreadCount)
{
	if (item == NULL) {
		return;
	}

	QString name = item->data(COLUMN_DATA, ROLE_NAME).toString();
	QFont font = item->font(COLUMN_NAME);

	if (unreadCount) {
		name += QString(" (%1)").arg(unreadCount);
		font.setBold(true);
	} else {
		font.setBold(false);
	}

	item->setText(COLUMN_NAME, name);
	item->setFont(COLUMN_NAME, font);

}

QTreeWidgetItem *RecurGroupTreeWidget::getItemFromId(const QString &id)
{
	if (id.isEmpty()) {
		return NULL;
	}

	/* Search exisiting item */
	QTreeWidgetItemIterator itemIterator(ui->treeWidget);
	QTreeWidgetItem *item;
	while ((item = *itemIterator) != NULL) {
		itemIterator++;

		if (item->parent() == NULL) {
			continue;
		}
		if (item->data(COLUMN_DATA, ROLE_ID).toString() == id) {
			return item;
		}
	}
	return NULL ;
}

QTreeWidgetItem *RecurGroupTreeWidget::activateId(const QString &id, bool focus)
{
	QTreeWidgetItem *item = getItemFromId(id);
	if (item == NULL) {
		return NULL;
	}

	ui->treeWidget->setCurrentItem(item);
	if (focus) {
		ui->treeWidget->setFocus();
	}
	return item;
}

RSTreeWidget *RecurGroupTreeWidget::treeWidget()
{
	return ui->treeWidget;
}

int RecurGroupTreeWidget::subscribeFlags(const QString &id)
{
	QTreeWidgetItem *item = getItemFromId(id);
	if (item == NULL) {
		return 0;
	}

	return item->data(COLUMN_DATA, ROLE_SUBSCRIBE_FLAGS).toInt();
}

int RecurGroupTreeWidget::calculateScore(QTreeWidgetItem *item, const QString &filterText)
{

	int score = 0;
	if (!item) return score;

	/* Calculate one item */
	if (filterText.isEmpty()) {
		score = 0;
		item->setHidden(false);
		int count = item->childCount();
		for (int nIndex = 0; nIndex < count; ++nIndex) {
			calculateScore(item->child(nIndex), filterText);
		}
	} else {
		QString scoreString;

		switch (ui->filterLineEdit->currentFilter()) {
		case FILTER_NAME_INDEX:
			scoreString = item->data(COLUMN_DATA, ROLE_NAME).toString();
			break;
		case FILTER_DESC_INDEX:
			scoreString = item->data(COLUMN_DATA, ROLE_DESCRIPTION).toString();
			break;
		}

		score = scoreString.count(filterText, Qt::CaseInsensitive);

		int count = item->childCount();
		for (int nIndex = 0; nIndex < count; ++nIndex) {
			score += calculateScore(item->child(nIndex), filterText);
		}

		if (score) {
			item->setHidden(false);
		} else {
			item->setHidden(true);
		}
	}

	item->setData(COLUMN_DATA, ROLE_SEARCH_SCORE, -score); // negative for correct sorting

	return score;

}

void RecurGroupTreeWidget::calculateScores(const QString &filterText)
{
	/* Find out which has given word in it */
	QTreeWidgetItemIterator itemIterator(ui->treeWidget);

	int count = ui->treeWidget->topLevelItemCount();
	for (int child = 0; child < count; child++) {
		QTreeWidgetItem *catitem = ui->treeWidget->topLevelItem(child);
		int icount = catitem->childCount();
		for (int nIndex = 0; nIndex < icount; ++nIndex) {
			calculateScore(catitem->child(nIndex), filterText);
		}
	}
}

void RecurGroupTreeWidget::filterChanged()
{
	/* Recalculate score */
	calculateScores(ui->filterLineEdit->text());

	sort();
}

void RecurGroupTreeWidget::resort(QTreeWidgetItem *categoryItem, Qt::SortOrder *order)
{
	if(order == NULL){
		Qt::SortOrder so = (actionSortAscending == NULL || actionSortAscending->isChecked()) ? Qt::AscendingOrder : Qt::DescendingOrder;
		order = &so;

		if (ui->filterLineEdit->text().isEmpty() == false) {
			compareRole->setRole(COLUMN_DATA, ROLE_SEARCH_SCORE);
			compareRole->addRole(COLUMN_DATA, ROLE_LASTPOST);
		} else if (actionSortByName && actionSortByName->isChecked()) {
			compareRole->setRole(COLUMN_DATA, ROLE_NAME);
		} else if (actionSortByPopularity && actionSortByPopularity->isChecked()) {
			compareRole->setRole(COLUMN_DATA, ROLE_POPULARITY);
		} else if (actionSortByLastPost && actionSortByLastPost->isChecked()) {
			compareRole->setRole(COLUMN_DATA, ROLE_LASTPOST);
		}
	}

	categoryItem->sortChildren(COLUMN_DATA, *order);
	int count = categoryItem->childCount();
	for (int child = 0; child < count; child++) {
		resort(categoryItem->child(child), order);
	}
}

void RecurGroupTreeWidget::sort()
{
	int count = ui->treeWidget->topLevelItemCount();
	for (int child = 0; child < count; child++) {
		resort(ui->treeWidget->topLevelItem(child), NULL);
	}
}