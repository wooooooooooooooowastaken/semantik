// Thomas Nagy 2007-2011 GPLV3

#include "canvas_flag.h"
#include "data_item.h"
#include  <QtGlobal>
#include "sem_model.h"
#include "canvas_view.h"
#include  <QMenu> 
#include <QToolTip>
#include  <QColorDialog> 
#include<KToolBar> 
#include  <QActionGroup> 
#include "canvas_item.h"
#include "canvas_link.h"
#include<QCoreApplication>
#include "canvas_sort.h"
#include "semantik.h"
#include <QTextCursor> 
#include  <QApplication> 
#include <QtDebug>
#include  <QX11Info>
#include <QScrollBar>
#include  <QGraphicsScene>
#include <QWheelEvent>
#include  <QMatrix>
#include <QPointF>

 #include <QGraphicsTextItem>

#include <math.h>

canvas_view::canvas_view(QWidget *i_oWidget, sem_model *i_oControl) : QGraphicsView(i_oWidget)
{
	m_oSemantikWindow = i_oWidget;

	//m_oRubbery = new QRubberBand(QRubberBand::Rectangle, this);
	//m_oRubbery->setGeometry(QRect(0, 0, 0, 0));

	QGraphicsScene *l_oScene = new QGraphicsScene(this);
	l_oScene->setSceneRect(-400, -400, 400, 400);
	setScene(l_oScene);

	//setCacheMode(CacheBackground);
	setRenderHint(QPainter::Antialiasing);

#ifndef Q_OS_DARWIN

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	setMinimumSize(200, 200);

#ifdef O
	//setTransformationAnchor(AnchorUnderMouse);
	//setResizeAnchor(AnchorViewCenter);
#endif

	/*QMatrix k(0, 1, 1, 0, 0, 0);
	  QMatrix kv(1, 0, 0, -1, 0, 0);*/
	m_oControl = i_oControl;
	/*setMatrix(l_oMatrix);
	  scale(0.8, 0.8);*/
#endif

	m_iLastMode = m_iMode = select_mode;

	m_bPressed = false;

	m_oRubberLine = new rubber_line(QRubberBand::Line, this);

	QBrush l_oBrush = QBrush();
	//QColor l_oColor = QColor("#000077");
	//l_oColor.setAlpha(200);

	QColor l_oColor = QColor(0, 0, 150, 100);
	l_oBrush.setColor(l_oColor);
	//m_oRubber->setBrush(l_oBrush);

	QAction *l_o = NULL;

	m_oAddItemAction = new QAction(trUtf8("Insert child"), this);
	m_oAddItemAction->setShortcut(notr("Ctrl+Return"));
	connect(m_oAddItemAction, SIGNAL(triggered()), this, SLOT(slot_add_item()));
	addAction(m_oAddItemAction);

	m_oDeleteAction = new QAction(trUtf8("Delete selection"), this);
	m_oDeleteAction->setShortcut(notr("Del"));
	connect(m_oDeleteAction, SIGNAL(triggered()), this, SLOT(slot_delete()));
	addAction(m_oDeleteAction);

	m_oInsertSiblingAction = l_o = new QAction(trUtf8("Insert sibling"), this);
	l_o->setShortcut(notr("Shift+Return"));
	connect(l_o, SIGNAL(triggered()), this, SLOT(slot_add_sibling()));
	addAction(l_o);

	m_oMoveUpAction = l_o = new QAction(trUtf8("Move up"), this); l_o->setShortcut(notr("Alt+Up")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(0));
	m_oMoveDownAction = l_o = new QAction(trUtf8("Move down"), this); l_o->setShortcut(notr("Alt+Down")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(1));
	m_oMoveLeftAction = l_o = new QAction(trUtf8("Move left"), this); l_o->setShortcut(notr("Alt+Left")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(2));
	m_oMoveRightAction = l_o = new QAction(trUtf8("Move right"), this); l_o->setShortcut(notr("Alt+Right")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(3));

	m_oSelectUpAction = l_o = new QAction(trUtf8("Select up"), this); l_o->setShortcut(notr("Up")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(0));
	m_oSelectDownAction = l_o = new QAction(trUtf8("Select down"), this); l_o->setShortcut(notr("Down")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(1));
	m_oSelectLeftAction = l_o = new QAction(trUtf8("Select left"), this); l_o->setShortcut(notr("Left")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(2));
	m_oSelectRightAction = l_o = new QAction(trUtf8("Select right"), this); l_o->setShortcut(notr("Right")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(3));


	m_oNextRootAction = l_o = new QAction(trUtf8("Next root"), this); l_o->setShortcut(notr("PgDown")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_next_root())); addAction(l_o); l_o->setData(QVariant(1));


	m_oEditAction = new QAction(trUtf8("Toggle edit"), this);
	m_oEditAction->setShortcut(notr("Return"));
	addAction(m_oEditAction);
	connect(m_oEditAction, SIGNAL(triggered()), this, SLOT(slot_toggle_edit()));

	m_oMenu = new QMenu(this);
	m_oMenu->addAction(m_oAddItemAction);
	m_oMenu->addAction(m_oEditAction);
	m_oMenu->addAction(m_oDeleteAction);

	QAction *fullAction = new QAction(trUtf8("Toggle fullscreen"), this);
	connect(fullAction, SIGNAL(triggered()), this, SLOT(toggle_fullscreen())); addAction(l_o);
	m_oMenu->addAction(fullAction);

	m_oColorMenu = m_oMenu->addMenu(trUtf8("Colors"));
	m_oDataMenu = m_oMenu->addMenu(trUtf8("Data type"));


#define newAction(s, v, dest)  dest = l_o = new QAction(s, this); \
	connect(l_o, SIGNAL(triggered()), this, SLOT(slot_change_data())); \
	addAction(l_o); l_o->setData(QVariant(v)); \
	m_oDataMenu->addAction(l_o); \
	l_o->setCheckable(true);

	newAction(trUtf8("Text"), VIEW_TEXT, m_oTextType);
	newAction(trUtf8("Diagram"), VIEW_DIAG, m_oDiagramType);
	newAction(trUtf8("Table"), VIEW_TABLE, m_oTableType);
	newAction(trUtf8("Image"), VIEW_IMG, m_oImageType);

	set_mode(select_mode);

	m_bDeleting = false;
	//connect(scene(), SIGNAL(selectionChanged()), this, SLOT(selection_changed())); // TODO check with m_bPressed
}

void canvas_view::check_selection() {

	if (m_bDeleting) return;

	QList<int> lst;
	foreach(canvas_item* k, selection()) {
		lst.append(k->Id());
	}

	// stupid set intersection
	QList<int> unlst;
	foreach(data_item* x, m_oControl->m_oItems.values()) {
		if (x->m_bSelected) {
			unlst.append(x->m_iId);
		}
	}

	foreach(int i, lst) {
		if (!unlst.contains(i))
			goto undo;
	}

	foreach (int i, unlst) {
		if (!lst.contains(i))
			goto undo;
	}

	mem_sel *sel;
	goto end;
	undo:
		sel = new mem_sel(m_oControl);
		sel->sel = lst;
		sel->apply();
		//qDebug()<<"selected"<<lst<<"unselected"<<unlst;
	end:
		;
}

void canvas_view::slot_next_root()
{
	QList<canvas_item*> sel = selection();
	switch (((QAction*) QObject::sender())->data().toInt())
	{
		case 0:
			if (sel.size()>1) deselect_all();
			m_oControl->prev_root();
			break;
		case 1:
			if (sel.size()>1) deselect_all();
			m_oControl->next_root();
			break;
		default:
			break;
	}
	if (sel.size()==1) ensureVisible(sel[0]);
}

void canvas_view::edit_off() {
	canvas_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == CANVAS_ITEM_T) {
			sel = (canvas_item*) tmp;

			sel->setTextInteractionFlags(Qt::NoTextInteraction);
			if (sel->toPlainText() == QObject::trUtf8("")) {
				sel->setPlainText(QObject::trUtf8("Empty"));
				sel->update_links();
			}

			m_oAddItemAction->setEnabled(true);
			m_oInsertSiblingAction->setEnabled(true);
			m_oDeleteAction->setEnabled(true);
			m_oNextRootAction->setEnabled(true);

			m_oMoveUpAction->setEnabled(true);
			m_oMoveDownAction->setEnabled(true);
			m_oMoveLeftAction->setEnabled(true);
			m_oMoveRightAction->setEnabled(true);
			m_oSelectUpAction->setEnabled(true);
			m_oSelectDownAction->setEnabled(true);
			m_oSelectLeftAction->setEnabled(true);
			m_oSelectRightAction->setEnabled(true);
			m_oControl->check_undo(true);
		}
	}
}

void canvas_view::slot_toggle_edit()
{
	canvas_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = (canvas_item*) tmp;
			}
		}
	}

	if (sel) {
		if (sel->textInteractionFlags() & Qt::TextEditorInteraction) {
			sel->setTextInteractionFlags(Qt::NoTextInteraction);
			if (sel->toPlainText() == QObject::trUtf8("")) {
				sel->setPlainText(QObject::trUtf8("Empty"));
				sel->update(); // seems to update faster
				sel->update_links();
			}
		} else {
			sel->setTextInteractionFlags(Qt::TextEditorInteraction);
			if (sel->toPlainText() == QObject::trUtf8("Empty")) {
				sel->setPlainText("");
				sel->update(); // seems to update faster
				sel->update_links();
			}
			sel->setFocus();

			m_oAddItemAction->setEnabled(false);
			m_oInsertSiblingAction->setEnabled(false);
			m_oDeleteAction->setEnabled(false);
			m_oNextRootAction->setEnabled(false);

			m_oMoveUpAction->setEnabled(false);
			m_oMoveDownAction->setEnabled(false);
			m_oMoveLeftAction->setEnabled(false);
			m_oMoveRightAction->setEnabled(false);
			m_oSelectUpAction->setEnabled(false);
			m_oSelectDownAction->setEnabled(false);
			m_oSelectLeftAction->setEnabled(false);
			m_oSelectRightAction->setEnabled(false);
			m_oControl->check_undo(false);
			return;
		}
	}

	m_oAddItemAction->setEnabled(true);
	m_oInsertSiblingAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
	m_oNextRootAction->setEnabled(true);

	m_oMoveUpAction->setEnabled(true);
	m_oMoveDownAction->setEnabled(true);
	m_oMoveLeftAction->setEnabled(true);
	m_oMoveRightAction->setEnabled(true);
	m_oSelectUpAction->setEnabled(true);
	m_oSelectDownAction->setEnabled(true);
	m_oSelectLeftAction->setEnabled(true);
	m_oSelectRightAction->setEnabled(true);
	m_oControl->check_undo(true);
}


void canvas_view::slot_move()
{
	QList<canvas_item*> sel = selection();
	if (sel.size() < 1) return;
	switch (((QAction*) QObject::sender())->data().toInt())
	{
		case 0: move_sel(0, -20); break;
		case 1: move_sel(0, 20); break;
		case 2: move_sel(-20, 0); break;
		case 3: move_sel(20, 0); break;
		default: break;
	}
}

void canvas_view::slot_sel()
{
	QList<canvas_item*> sel = selection();
	if (sel.size() != 1) return;
	int l_iId = sel[0]->Id();
	switch (((QAction*) QObject::sender())->data().toInt())
	{
		case 0: m_oControl->select_item_keyboard(l_iId, 3); break;
		case 1: m_oControl->select_item_keyboard(l_iId, 4); break;
		case 2: m_oControl->select_item_keyboard(l_iId, 1); break;
		case 3: m_oControl->select_item_keyboard(l_iId, 2); break;
		default: break;
	}
	if (sel.size()==1) ensureVisible(sel[0]);
}

void canvas_view::slot_add_item()
{
	QList<canvas_item*> sel = selection();
	int l_iId = NO_ITEM;
	if (sel.size() == 1) {
		l_iId = sel[0]->Id();
	}

	mem_add *add = new mem_add(m_oControl);
	add->init();
	add->item->m_iXX = m_oLastPoint.x();
	add->item->m_iYY = m_oLastPoint.y();
	add->parent = l_iId;
	add->apply();

	// FIXME probably
	reorganize(); // this was here before
	deselect_all();
	m_oItems.value(add->item->m_iId)->setSelected(true);
}

void canvas_view::slot_add_sibling()
{
	QList<canvas_item*> sel = selection();
	if (sel.size() != 1) return;
	int l_iId = m_oControl->parent_of(sel[0]->Id());
	if (l_iId == NO_ITEM) return;
	deselect_all();

	mem_add *add = new mem_add(m_oControl);
	add->init();
	add->item->m_iXX = m_oLastPoint.x();
	add->item->m_iYY = m_oLastPoint.y();
	add->parent = l_iId;
	add->apply();
	reorganize(); // this was here before
	deselect_all();
	m_oItems.value(add->item->m_iId)->setSelected(true);
}

void canvas_view::slot_delete()
{
	QList<int> l_oLst;
	foreach (canvas_item *l_oItem, selection()) {
		l_oLst.push_back(l_oItem->Id());
	}

	if (l_oLst.isEmpty()) {
		return;
	}

	// TODO compose?
	mem_sel *sel = new mem_sel(m_oControl);
	sel->sel;
	sel->unsel = l_oLst;
	sel->apply();

	mem_delete *del = new mem_delete(m_oControl);
	del->init(l_oLst);
	del->apply();
}

void canvas_view::show_sort(int i_iId, bool i_b)
{
	int j=0;
	for (int i=0; i<m_oControl->m_oLinks.size(); i++)
	{
		QPoint l_oP = m_oControl->m_oLinks.at(i);
		if (l_oP.x() == i_iId)
		{
			++j;
			canvas_item *l_oRect = m_oItems.value(l_oP.y());

			QString l_sNum = QString::number(j);
			if (l_sNum != l_oRect->m_sNum)
			{
				l_oRect->m_sNum = l_sNum;
				l_oRect->m_oSort->update();
			}
			l_oRect->m_oSort->setVisible(i_b);
		}
	}
}

void canvas_view::move_sel(int i_iX, int i_iY)
{
	QList<canvas_item*> sel = selection();
	check_canvas_size();
	foreach (canvas_item *l_oItem, sel) { l_oItem->moveBy(i_iX, i_iY); }
	foreach (canvas_item *l_oItem, sel) { l_oItem->update_links(); ensureVisible(l_oItem); }
}

void canvas_view::update_cursor() {
	switch (m_iMode)
	{
		case select_mode:
			setDragMode(QGraphicsView::RubberBandDrag);
			viewport()->setCursor(Qt::ArrowCursor);
			break;
		case link_mode:
			setDragMode(QGraphicsView::NoDrag);
			viewport()->setCursor(Qt::CrossCursor);
			break;
		case sort_mode:
			setDragMode(QGraphicsView::NoDrag);
			viewport()->setCursor(Qt::ArrowCursor);
			break;
		case scroll_mode:
			setDragMode(QGraphicsView::ScrollHandDrag);
			break;
		default:
			qDebug()<<"unknown mode"<<m_iMode;
			break;
	}
}

void canvas_view::set_mode(mode_type i_iMode, mode_type i_iSaveMode)
{
	if (i_iMode == m_iMode) {
		return;
	}

	deselect_all();

	m_iMode = i_iMode;
	update_cursor();
	m_iLastMode = (i_iSaveMode != no_mode) ? i_iSaveMode : m_iMode;

	// reset
	m_bPressed = false;
	// unneeded statements
	//m_oRubbery->hide();
	m_oRubberLine->hide();
}


void canvas_view::zoom_in()
{
	double i_iScaleFactor = 1.05;
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	check_canvas_size();
}

void canvas_view::zoom_out()
{
	double i_iScaleFactor = 0.95;
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	check_canvas_size();
}

void canvas_view::wheelEvent(QWheelEvent *i_oEvent)
{
	QPointF l_o = mapToScene(i_oEvent->pos());
	qreal i_iScaleFactor = pow(2.0, i_oEvent->delta() / 440.0);
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	centerOn(l_o + mapToScene(viewport()->rect().center()) - mapToScene(i_oEvent->pos()));
}

void canvas_view::synchro_doc(const hash_params& i_o)
{
	int l_iCmd = i_o[data_commande].toInt();
	switch (l_iCmd)
	{
		case cmd_add_item:
			{
				int l_iId = i_o[data_id].toInt();
				Q_ASSERT(! m_oItems.contains(l_iId));

				canvas_item*l_oR = new canvas_item(this, l_iId);

				l_oR->setSelected(true);

				QRectF l_oRect = l_oR->boundingRect();
				l_oR->setPos(m_oLastPoint - QPointF(l_oRect.width()/2, l_oRect.height()/2));

				data_item *l_oData = *m_oControl + l_iId;
				l_oData->m_iXX = l_oR->pos().x();
				l_oData->m_iYY = l_oR->pos().y();

				m_oItems[l_iId] = l_oR;
			}
			break;
		case cmd_update_item:
			{
				if (i_o[data_orig].toInt() == VIEW_CANVAS) return;
				int j = i_o[data_id].toInt();
				m_oItems.value(j)->update_data();
				m_oItems.value(j)->update();
			}
			break;
		case cmd_select_item:
			{
			}
			break;
		case cmd_link:
			{
				canvas_item *l_oR1 = m_oItems.value(i_o[data_id].toInt());
				canvas_item *l_oR2 = m_oItems.value(i_o[data_parent].toInt());
				canvas_link * l_oLink = new canvas_link(this, l_oR2, l_oR1);
				l_oLink->update_pos();
				l_oR1->update();
			}
			break;
		case cmd_unlink:
			{
			}
			break;
		case cmd_remove_item:
			{
			}
			break;
		case cmd_sort_item:
			{
				QList<canvas_item*>sel = selection();
				show_sort(sel[0]->Id(), m_iMode == sort_mode);
			}
			break;
		case cmd_save_data:
			{
				QList<canvas_item*> l_oList = m_oItems.values();
				foreach (canvas_item *l_oItem, l_oList)
				{
					data_item *l_oData = *m_oControl + l_oItem->Id();
					l_oData->m_iXX = l_oItem->pos().x();
					l_oData->m_iYY = l_oItem->pos().y();
					l_oData->m_iWW = l_oItem->boundingRect().width();
					l_oData->m_iHH = l_oItem->boundingRect().height();
				}
			}
			break;
		case cmd_open_map:
			{
				check_canvas_size();
				fit_zoom();
			}
			break;
		default:
			break;
	}
}

void canvas_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {

	edit_off();
	foreach (int k, sel) {
		Q_ASSERT(m_oItems[k] != NULL);
		if (!m_oItems[k]->isSelected())
			m_oItems[k]->setSelected(true);
	}

	foreach (int k, unsel) {
		//qDebug()<<"unselect "<<k;
		Q_ASSERT(m_oItems[k] != NULL);
		if (m_oItems[k]->isSelected())
			m_oItems[k]->setSelected(false);
	}


	/*
	QList<canvas_item*> sel = selection();
	if (i_o[data_orig].toInt() == VIEW_CANVAS)
	{
	check_selected();
		return;
	}
	int l_iId = i_o[data_id].toInt();
	if (l_iId == NO_ITEM)
	{
		deselect_all(false);
	}
	else if (sel.size() == 1)
	{
		if (sel[0]->Id() != l_iId)
		{
			sel[0]->setSelected(false);
			m_oItems.value(l_iId)->setSelected(true);
		}
	}
	else
	{
		add_select(m_oItems.value(l_iId), false);
	}
	check_selected();*/
}

void canvas_view::sync_colors() {
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	l_o->m_oColorsToolBar->clear();

	while (l_o->m_oColorGroup->actions().size() > m_oControl->m_oColorSchemes.size()+1)
	{
		QAction* l_oA = l_o->m_oColorGroup->actions().takeFirst();
		l_o->m_oColorsToolBar->removeAction(l_oA);
		m_oColorMenu->removeAction(l_oA);
		delete l_oA;
	}

	while (l_o->m_oColorGroup->actions().size() < m_oControl->m_oColorSchemes.size()+1)
	{
		new QAction(QIcon(), trUtf8("Color"), l_o->m_oColorGroup);
	}

	l_o->m_oColorGroup->removeAction(l_o->m_oCustomColorAct);
	l_o->m_oColorGroup->addAction(l_o->m_oCustomColorAct);
	m_oColorMenu->removeAction(l_o->m_oCustomColorAct);
	m_oColorMenu->addAction(l_o->m_oCustomColorAct);

	for (int i=0; i<m_oControl->m_oColorSchemes.size(); ++i)
	{
		color_scheme l_oScheme = m_oControl->m_oColorSchemes[i];
		QAction *l_oAction = l_o->m_oColorGroup->actions()[i];

		QPixmap l_oPix(22, 22);
		QPainter l_oP(&l_oPix);

		l_oAction->setText(l_oScheme.m_sName);

		l_oPix.fill(l_oScheme.m_oInnerColor);
		//TODO pen ?
		l_oP.drawRect(0, 0, 21, 21);
		l_oAction->setIcon(QIcon(l_oPix));
	}

	// la première action est pour la couleur de la racine
	for (int i=1; i<l_o->m_oColorGroup->actions().size(); ++i)
	{
		QAction *l_oAct = l_o->m_oColorGroup->actions()[i];
		l_o->m_oColorsToolBar->addAction(l_oAct);
		m_oColorMenu->addAction(l_oAct);
	}
}

void canvas_view::sync_flags() {
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	l_o->m_oFlagsToolBar->clear();

	while (l_o->m_oFlagGroup->actions().size() > m_oControl->m_oFlagSchemes.size())
	{
		QAction* l_oA = l_o->m_oFlagGroup->actions().takeFirst();
		delete l_oA;
	}

	while (l_o->m_oFlagGroup->actions().size() < m_oControl->m_oFlagSchemes.size())
	{
		new QAction(QIcon(), trUtf8("Flag"), l_o->m_oFlagGroup);
	}

	for (int i=0; i<m_oControl->m_oFlagSchemes.size(); ++i)
	{
		flag_scheme* l_oScheme = m_oControl->m_oFlagSchemes[i];
		QAction *l_oAction = l_o->m_oFlagGroup->actions()[i];

		l_oAction->setText(l_oScheme->m_sName);
		l_oAction->setIcon(l_oScheme->_icon());
	}

	foreach(QAction* l_oAct, l_o->m_oFlagGroup->actions())
	{
		l_o->m_oFlagsToolBar->addAction(l_oAct);
		l_oAct->setCheckable(true);
	}
}

void canvas_view::change_colors(QAction* i_oAct)
{
	static QColor l_oColor;

	int l_iIdx = -1;
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	for (int i=1; i<l_o->m_oColorGroup->actions().size(); ++i)
	{
		if (l_o->m_oColorGroup->actions()[i]==i_oAct)
		{
			l_iIdx = i;
			if (i==l_o->m_oColorGroup->actions().size()-1)
			{
				l_oColor = QColorDialog::getColor(l_oColor, this);
				if (!l_oColor.isValid()) return;
			}
			break;
		}
	}

	mem_color* col = new mem_color(m_oControl);
	col->newColor = l_iIdx;
	col->apply();
}

void canvas_view::change_flags(QAction* i_oAction)
{
	int l_iIndex = 0;
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	for (int i=0; i<l_o->m_oFlagGroup->actions().size(); ++i)
	{
		QAction *l_oAct = l_o->m_oFlagGroup->actions()[i];
		if (l_oAct == i_oAction) l_iIndex = i;
	}
	QString l_sName = m_oControl->m_oFlagSchemes[l_iIndex]->m_sId;
	canvas_item *l_oItem = selection()[0];
	data_item *l_oData = *m_oControl + l_oItem->Id();

	if (i_oAction->isChecked()) l_oData->m_oFlags.push_back(l_sName);
	else l_oData->m_oFlags.removeAll(l_sName);
	l_oItem->update_flags();

	/*
	mem_flag* flag = new mem_flag(m_oControl),
	flag->newFlags = ;
	flag->apply();
	*/
}

void canvas_view::check_selected()
{
	QList <canvas_item*> sel = selection();
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	data_item *l_oData = NULL;
	if (sel.size() == 1)
	{
		l_oData = *m_oControl + sel[0]->Id();
	}

	//foreach(QAction* l_oAct, l_o->m_oFlagGroup->actions())
	for (int i=0; i<l_o->m_oFlagGroup->actions().size(); ++i)
	{
		QAction *l_oAct = l_o->m_oFlagGroup->actions()[i];

		if (sel.size() != 1) l_oAct->setChecked(false);
		else
		{
			QString l_sName = m_oControl->m_oFlagSchemes[i]->m_sId;
			l_oAct->setChecked(l_oData->m_oFlags.contains(l_sName));
		}
		l_oAct->setEnabled(sel.size());
	}
	foreach(QAction* l_oAct, l_o->m_oColorGroup->actions())
	{
		l_oAct->setEnabled(sel.size() >= 1);
	}
}

void canvas_view::deselect_all(bool i_oSignal)
{
	QList<QGraphicsItem*> lst = scene()->selectedItems();
	foreach (QGraphicsItem *tmp, lst) {
		tmp->setSelected(false);

		if (tmp->type() == CANVAS_ITEM_T) {
			canvas_item *sel = (canvas_item*) tmp;
			sel->setZValue(99);
			sel->setTextInteractionFlags(Qt::NoTextInteraction);
		}
	}

	/*
	foreach (canvas_item *l_oItem, m_oSelected)
	{
		l_oItem->set_selected(false);
		l_oItem->setZValue(99);
		if (m_iMode == sort_mode || m_iLastMode == sort_mode)
		{
			show_sort(l_oItem->Id(), false);
			emit sig_message("", -1);
		}
		l_oItem->update();
	}
	m_oSelected.clear();

	notify_select(i_oSignal);
	*/
}

void canvas_view::add_select(canvas_item* i_oItem, bool i_oSignal)
{
	/*
	Q_ASSERT(i_oItem != NULL);

	if (m_oSelected.size() == 1)
	{
		QFocusEvent l_oEv = QFocusEvent(QEvent::FocusOut);
		//m_oSelected[0]->focus_out(&l_oEv);
	}

	m_iSortCursor = 0;
	m_oSelected.push_back(i_oItem);
	i_oItem->set_selected(true);
	i_oItem->setZValue(100);
	if (m_iMode == sort_mode)
	{
		show_sort(i_oItem->Id(), true);
		emit sig_message(trUtf8("Click to set Item %1").arg(QString::number(m_iSortCursor+1)), -1);
	}
	i_oItem->update();

	notify_select(i_oSignal);
	*/
}

void canvas_view::rm_select(canvas_item* i_oItem, bool i_oSignal)
{
	/*
	Q_ASSERT(i_oItem != NULL);
	i_oItem->set_selected(false);
	i_oItem->setZValue(99);
	if (m_iMode == sort_mode) show_sort(i_oItem->Id(), false);
	i_oItem->update();
	m_oSelected.removeAll(i_oItem);

	notify_select(i_oSignal);
	*/
}

void canvas_view::notify_select(bool i_oSignal)
{
	/*
	enable_actions();

	if (!i_oSignal) return;
	if (m_oSelected.size()==1)
		m_oControl->select_item(m_oSelected[0]->Id(), VIEW_CANVAS);
	else
		m_oControl->select_item(NO_ITEM, VIEW_CANVAS);
	*/
}

void canvas_view::enable_actions()
{
	foreach (QAction* l_o, actions())
	{
		l_o->setEnabled(true); // TODO we could disable the move actions one by one
	}
	enable_menu_actions();
}

void canvas_view::enable_menu_actions()
{
	QList<canvas_item*> sel = selection();
	m_oAddItemAction->setEnabled(sel.size()<=1);
	m_oDeleteAction->setEnabled(sel.size()>0);
	m_oEditAction->setEnabled(sel.size()==1);
	m_oColorMenu->setEnabled(sel.size()>=1);
	m_oDataMenu->setEnabled(sel.size()==1);

	if (sel.size() == 1)
	{
		data_item *l_oData = *m_oControl + sel[0]->Id();

		#define fafa(v, t) v->setChecked(l_oData->m_iDataType == t);
		fafa(m_oTextType, VIEW_TEXT);
		fafa(m_oDiagramType, VIEW_DIAG);
		fafa(m_oTableType, VIEW_TABLE);
		fafa(m_oImageType, VIEW_IMG);
	}

	foreach (QAction* l_o, m_oDataMenu->actions())
	{
		l_o->setEnabled(sel.size()==1);
	}
}

void canvas_view::mousePressEvent(QMouseEvent *i_oEv)
{
	m_oLastPressPoint = i_oEv->pos();
	if (i_oEv->button() == Qt::RightButton)
	{
		// first, we cannot edit an item when right-click is selected
		edit_off();

		// select the item under the cursor if available and show the popup menu
		m_oLastPoint = mapToScene(i_oEv->pos());
		QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
		if (l_oItem && l_oItem->type() == CANVAS_ITEM_T)
		{
			if (!l_oItem->isSelected())
			{
				deselect_all();
				l_oItem->setSelected(true);
			}
		}
		else
		{
			deselect_all();
		}
		m_oMenu->popup(i_oEv->globalPos());
		i_oEv->accept();
		return;
	}

	if (i_oEv->button() == Qt::MidButton) {
		set_mode(scroll_mode, m_iMode);
	}

	m_bPressed = (i_oEv->button() != Qt::RightButton);

	// link items on click sequences
	QList<canvas_item*> sel = selection();
	if (sel.size() == 1 && QApplication::keyboardModifiers() & Qt::ShiftModifier) {

		QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
		if (l_oItem && l_oItem->type() == CANVAS_ITEM_T) {

			int id1 = sel.at(0)->Id();
			int id2 = ((canvas_item*) l_oItem)->Id();
			m_oControl->link_items(id1, id2);

			if (m_iMode != link_mode) {
				QList<int> unlst;
				unlst.append(id1);
				QList<int> lst;
				lst.append(id2);

				mem_sel *sel = new mem_sel(m_oControl);
				sel->sel = lst;
				sel->unsel = unlst;
				sel->apply();
			} else {
				deselect_all();
			}
			return;
		}
	}

	QGraphicsView::mousePressEvent(i_oEv);
}


void canvas_view::mouseReleaseEvent(QMouseEvent *i_oEv)
{
	QGraphicsView::mouseReleaseEvent(i_oEv);

	set_mode(m_iLastMode);
	m_iLastMode = m_iMode;

	if (i_oEv->button() == Qt::RightButton) return;
	m_bPressed = false;

	/*if (m_iLastMode != no_mode)
	{
		m_iMode = m_iLastMode;
		m_iLastMode = no_mode;
		viewport()->setCursor(m_iMode==link_mode?Qt::CrossCursor:Qt::ArrowCursor);
	}*/

	m_oRubberLine->setVisible(false);
	switch (m_iMode)
	{
		case select_mode:
			{
				check_selection();

				//qDebug()<<"mouse release event!";
				QList<int> lst;
				foreach (QGraphicsItem* k, scene()->selectedItems()) {
					canvas_item* t = (canvas_item*) k;
					lst.append(t->Id());
				}
				if (lst.size()) {
					data_item *p = m_oControl->m_oItems[lst[0]];
					canvas_item *v = m_oItems[lst[0]];
					if (qAbs(p->m_iXX - v->pos().x()) + qAbs(p->m_iYY - v->pos().y()) > 0.1) {
						mem_move *mv = new mem_move(m_oControl);
						mv->sel = lst;
						for (int i = 0; i < lst.size(); ++i) {
							data_item *it = m_oControl->m_oItems[lst[i]];
							mv->oldPos.append(QPointF(it->m_iXX, it->m_iYY));
							mv->newPos.append(m_oItems[lst[i]]->pos());
						}
						mv->apply();
					} else { qDebug()<<"move too small"; }
				}
			}
			break;
		case link_mode:
			{
				canvas_item *l_oR1 = NULL;
				canvas_item *l_oR2 = NULL;

				foreach (QGraphicsItem *l_oI1, scene()->items(mapToScene(m_oLastPressPoint)))
				{
					if (l_oI1->type() == CANVAS_ITEM_T)
					{
						l_oR1 = (canvas_item*) l_oI1;
						break;
					}
				}

				foreach (QGraphicsItem *l_oI1, scene()->items(mapToScene(i_oEv->pos())))
				{
					if (l_oI1->type() == CANVAS_ITEM_T)
					{
						l_oR2 = (canvas_item*) l_oI1;
						break;
					}
				}

				if (l_oR1 && l_oR2 && l_oR1 != l_oR2)
				{
					m_oControl->link_items(l_oR1->Id(), l_oR2->Id());
					// TODO
					deselect_all();
					/*
					QList<int> unlst;
					unlst.append(l_oR1->m_iId);
					QList<int> lst;
					lst.append(l_oR2->m_iId);

					mem_sel *sel = new mem_sel(m_oControl);
					sel->sel = lst;
					sel->unsel = unlst;
					sel->apply();
					*/
				}
				m_oRubberLine->hide();
			}
			break;
	}
	update_cursor();
}

void canvas_view::mouseDoubleClickEvent(QMouseEvent* i_oEv)
{
	if (i_oEv->button() != Qt::LeftButton) return;
	m_oLastPoint = mapToScene(i_oEv->pos());
	QGraphicsItem *l_oItem = itemAt(i_oEv->pos());
	int l_iAdded = NO_ITEM;
	if (m_iMode == select_mode || m_iMode == link_mode) {
		if (l_oItem) {
			if (l_oItem->type() == CANVAS_ITEM_T) {
				deselect_all();

				canvas_item *l_oR = (canvas_item*) l_oItem;
				l_oR->setSelected(false);

				mem_add *add = new mem_add(m_oControl);
				add->init();
				add->item->m_iXX = m_oLastPoint.x();
				add->item->m_iYY = m_oLastPoint.y();
				add->parent = l_oR->Id();
				add->apply();

				//l_iAdded = m_oControl->add_item(l_oR->Id(), NO_ITEM, true);
				//QList<canvas_item*> sel = selection();
				//if (sel.size() == 1) sel[0]->setFocus();
				check_canvas_size();
			} else if (l_oItem->type() == CANVAS_LINK_T) {
				canvas_link *l_oLink = (canvas_link*) l_oItem;
				mem_unlink *link = new mem_unlink(m_oControl);
				link->child = l_oLink->m_oTo->Id();
				link->parent = l_oLink->m_oFrom->Id();
				link->apply();
			}
		} else if (i_oEv->modifiers() != Qt::ControlModifier) {
			//l_iAdded = m_oControl->add_item();
			//QList<canvas_item*> sel = selection();
			//if (sel.size() == 1) sel[0]->setFocus();

			mem_add *add = new mem_add(m_oControl);
			add->init();
			add->item->m_iXX = m_oLastPoint.x();
			add->item->m_iYY = m_oLastPoint.y();
			add->apply();
		}
	}
}

void canvas_view::check_canvas_size()
{
	// using viewport()->rect() was a bad idea
	QRect l_oRect = viewport()->rect();
	if (m_oItems.size() < 1) // no rectangle
	{
		scene()->setSceneRect(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));
		return;
	}

	QRectF l_oR2;

	qreal x, y, z, t;
	canvas_item *l_o = m_oItems.values()[0];

	x = z = l_o->x() + l_o->boundingRect().width()/2;
	y = t = l_o->y() + l_o->boundingRect().height()/2;

	QList<canvas_item*> l_oList = m_oItems.values();
	foreach (canvas_item *l_oItem, l_oList)
	{
		if (l_oItem->x() < x) x = l_oItem->x();
		if (l_oItem->y() < y) y = l_oItem->y();
		if (l_oItem->x2() > z) z = l_oItem->x2();
		if (l_oItem->y2() > t) t = l_oItem->y2();
	}
	x -=100; y -= 100; z += 100, t += 100;

	l_oR2 = QRectF(QPointF(x, y), QPointF(z, t));
	l_oR2 = l_oR2.united(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));
	if (l_oR2 == sceneRect()) return;
	scene()->setSceneRect(l_oR2);
}

void canvas_view::fit_zoom()
{
	//QRectF l_o = scene()->sceneRect();
	//fitInView(QRectF(l_o.topLeft()+QPointF(100, 100), l_o.size()+QSizeF(200, -200)), Qt::KeepAspectRatio);
#if 0
	QRectF l_o;
	foreach (QGraphicsItem *it, items())
	{
		l_o |= it->boundingRect();
	}
#endif
	//check_canvas_size();
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void canvas_view::slot_change_data()
{
	m_oControl->change_data(selection()[0]->Id(), ((QAction*) QObject::sender())->data().toInt());
}

void canvas_view::toggle_fullscreen()
{
	if (isFullScreen())
	{
		setWindowModality(Qt::NonModal);
		//setFullScreen(false);
		setWindowState(Qt::WindowNoState);
		setWindowFlags(Qt::Widget);
		show();
	}
	else
	{
		setWindowFlags(Qt::Window);
		//setFullScreen(true);
		setWindowState(Qt::WindowFullScreen);
		setWindowModality(Qt::ApplicationModal);
		show();
	}
}

rubber_line::rubber_line(QRubberBand::Shape i, QWidget* j) : QRubberBand(i, j)
{
}

void rubber_line::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	//painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(Qt::red));
	if (_direction > 0)
	{
		painter.drawLine(QPoint(0, 0), QPoint(size().width(), size().height()));
	}
	else
	{
		painter.drawLine(QPoint(size().width(), 0), QPoint(0, size().height()));
	}
}

void rubber_line::setGeometry(const QRect& i_o)
{
	_direction = i_o.width() * i_o.height();
	QRubberBand::setGeometry(i_o.normalized());
}

#define HSPACER 32.
#define WSPACER 32.

double canvas_view::compute_height(QMap<int, double> &map, QMap<int, QList<int> >&children, int id) {
	double size = 0;

	QMap<int, QList<int> >::const_iterator it = children.find(id);
	if (it != children.end()) {
		QList<int> tmp = it.value();
		size += (tmp.size() - 1) * HSPACER;
		foreach (int k, tmp) {
			size += compute_height(map, children, k);
		}
	}

	double tmp = m_oItems[id]->boundingRect().height();
	if (size < tmp) size = tmp;

	map[id] = size;
	//qDebug()<<"size for"<<id<<" "<<size;
	return size;
}

void canvas_view::compute_width(QMap<int, double> &map, QMap<int, QList<int> >&children, int id, int level) {
	double w = m_oItems[id]->boundingRect().width();
	QMap<int, double>::iterator jt = map.find(level);
	if (jt != map.end()) {
		double val = jt.value();
		map[level] = val > w ? val : w;
	} else {
		map[level] = w;
	}

	QMap<int, QList<int> >::iterator it = children.find(id);
	if (it != children.end()) {
		QList<int> tmp = it.value();
		foreach (int sub, tmp) {
			compute_width(map, children, sub, level+1);
		}
	}
}

void canvas_view::reorganize() {
	QList<int> roots = m_oControl->all_roots();
	QMap<int, double> height;

	QMap<int, QList<int> > children;

	for (int i=0; i < m_oControl->m_oLinks.size(); ++i) {
		QPoint l_oP = m_oControl->m_oLinks.at(i);

		QMap<int, QList<int> >::iterator it = children.find(l_oP.x());
		if (it != children.end()) {
			it.value() << l_oP.y();
		} else {
			QList<int> tmp;
			tmp << l_oP.y();
			children[l_oP.x()] = tmp;
		}
	}

	foreach (int k, roots) {
		double ref = compute_height(height, children, k);
		QMap<int, QList<int> >::iterator it = children.find(k);
		if (it != children.end()) {
			QList<int> tmp = it.value();

			ref -= (tmp.size() - 1) * HSPACER;
			int mid = 0;
			int max = 0;
			int tot = 0;
			double left_height = 0;
			foreach (int sub, tmp) {
				tot += height[sub];
				if (tot * (ref - tot) >= max) {
					max = tot * (ref - tot);
					mid = sub;

					left_height += height[sub] + HSPACER;
				} else {
					break;
				}
			}

			QMap<int, double> width;
			compute_width(width, children, k, 0);

			left_height -= HSPACER;

			int left = 1;
			double acc_height = m_oItems[k]->y() + m_oItems[k]->boundingRect().height() / 2 - left_height / 2;
			foreach (int sub, tmp) {

				// put the element in place, then recurse

				double y = acc_height + height[sub] / 2 - m_oItems[sub]->boundingRect().height() / 2;
				if (left) {
					double x = m_oItems[k]->x() + m_oItems[k]->boundingRect().width() - width[0] - WSPACER;
					m_oItems[sub]->setPos(x - m_oItems[sub]->boundingRect().width(), y);
				} else {
					double x = m_oItems[k]->x() + width[0] + WSPACER;
					m_oItems[sub]->setPos(x, y);
				}

				acc_height += height[sub] + HSPACER;

				m_oItems[k]->update_links();
				pack(width, height, children, sub, 1, left);

				// now to the right
				if (sub == mid) {
					left = 0;
					acc_height = m_oItems[k]->y() + m_oItems[k]->boundingRect().height() / 2 - (height[k] - left_height - HSPACER) / 2;
				}
			}
		}
	}
}

void canvas_view::pack(QMap<int, double> &width, QMap<int, double> &height, QMap<int, QList<int> >&children, int id, int level, int left) {
	QMap<int, QList<int> >::iterator it = children.find(id);
	if (it != children.end()) {
		QList<int> tmp = it.value();
		double acc_height = m_oItems[id]->y() + m_oItems[id]->boundingRect().height() / 2 - height[id] / 2;
		foreach (int sub, tmp) {
			double y = acc_height + height[sub] / 2 - m_oItems[sub]->boundingRect().height()/2;
			if (left) {
				double x = m_oItems[id]->x() + m_oItems[id]->boundingRect().width() - width[0] - WSPACER;
				m_oItems[sub]->setPos(x - m_oItems[sub]->boundingRect().width(), y);
			} else {
				double x = m_oItems[id]->x() + width[0] + WSPACER;
				m_oItems[sub]->setPos(x, y);
			}
			acc_height += height[sub] + HSPACER;
			pack(width, height, children, sub, level+1, left);
		}
		m_oItems[id]->update_links();
	}
}

QList<canvas_item*> canvas_view::selection() {
	// FIXME use scene()->selectedItems()
	QList<canvas_item*> ret;

	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
			ret.append((canvas_item*) tmp);
		}
	}
	return ret;
}

void canvas_view::mouseMoveEvent(QMouseEvent *i_oEv) {
	switch (m_iMode) {
		case select_mode:
			{
				QGraphicsView::mouseMoveEvent(i_oEv);
				foreach (QGraphicsItem*tmp, scene()->selectedItems()) {
					if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
						((canvas_item*) tmp)->update_links();
					}
				}
			}
			break;

		case link_mode:
			if (m_bPressed) {
				bool c1 = ((m_oLastPoint - i_oEv->pos()).manhattanLength() >= QApplication::startDragDistance());
				if (c1)
				{
					QRect l_oSel = QRect(m_oLastPressPoint, i_oEv->pos());
					m_oRubberLine->setGeometry(l_oSel);
				}
				m_oRubberLine->setVisible(c1);
			}
			break;
		case scroll_mode:
			if (m_bPressed) {
				QScrollBar *h = horizontalScrollBar();
				QScrollBar *v = verticalScrollBar();
				QPoint d = i_oEv->pos() - m_oLastPressPoint;
				h->setValue(h->value() - d.x());
				v->setValue(v->value() - d.y());
				m_oLastPressPoint = i_oEv->pos();
			}
			break;
	}
}

void canvas_view::notify_add_item(int id) {
	Q_ASSERT(! m_oItems.contains(id));
	canvas_item* l_oR = new canvas_item(this, id);
	m_oItems[id] = l_oR;
	l_oR->update_data();
	//l_oR->setSelected(true); // NOTE: do not call methods that create events here
}

void canvas_view::notify_delete_item(int id) {
	m_bDeleting = true;
	canvas_item *l_oR1 = m_oItems.value(id);
	Q_ASSERT(l_oR1!=NULL);
	scene()->removeItem(l_oR1);
	m_oItems.remove(id);
	delete l_oR1;
	m_bDeleting = false;
}

void canvas_view::notify_link_items(int id1, int id2) {
	canvas_item *l_oR1 = m_oItems.value(id1);
	canvas_item *l_oR2 = m_oItems.value(id2);
	canvas_link * l_oLink = new canvas_link(this, l_oR1, l_oR2);
	l_oLink->update_pos();
	l_oR2->update();
}

void canvas_view::notify_unlink_items(int id1, int id2) {
	canvas_item *l_oR1 = m_oItems.value(id1);
	canvas_item *l_oR2 = m_oItems.value(id2);

	foreach (QGraphicsItem *l_oItem, items())
	{
		if (l_oItem->type() == CANVAS_LINK_T)
		{
			canvas_link *l_oLink = (canvas_link*) l_oItem;
			if (
				(l_oLink->m_oFrom == l_oR1 && l_oLink->m_oTo == l_oR2)
				||
				(l_oLink->m_oFrom == l_oR2 && l_oLink->m_oTo == l_oR1)
			   )
			{
				l_oLink->hide();
				l_oLink->rm_link();
				delete l_oLink;
				break;
			}
		}
	}
	l_oR1->update();
	l_oR2->update();
}

void canvas_view::notify_move(const QList<int>&sel, const QList<QPointF>&pos) {
	for (int i = 0; i < sel.size(); ++i) {
		m_oItems[sel[i]]->setPos(pos[i]);
		m_oItems[sel[i]]->update_links();
	}
}

void canvas_view::notify_repaint(int id) {
	m_oItems[id]->update();
}

%: include  	"canvas_view.moc" 









#if 0
	canvas_item *l_oRect = NULL;
	switch (m_iMode)
	{
		case select_mode:
			{
				m_oLastPressPoint = i_oEv->pos();
				m_oLastMovePoint = mapToScene(m_oLastPressPoint);


				m_bPressed = (i_oEv->button() == Qt::LeftButton);

				QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
				if (m_bPressed)
				{
					if (l_oItem && l_oItem->type() == CANVAS_ITEM_T)
					{
						l_oRect = (canvas_item*) l_oItem;
					}
					else
					{
						l_oItem = NULL;
					}

					if (l_oItem)
					{
						switch (i_oEv->modifiers())
						{
							case Qt::ShiftModifier:
								if (m_oSelected.contains(l_oRect))
									rm_select(l_oRect);
								else
									add_select(l_oRect);
								break;
							default:
								if (!m_oSelected.contains(l_oRect))
								{
									if (m_oSelected.size() == 1 && !m_oControl->parent_of(l_oRect->Id()))
									{
										if (QApplication::keyboardModifiers() & Qt::ControlModifier)
											m_oControl->link_items(m_oSelected[0]->Id(), l_oRect->Id());
									}
									deselect_all();
									add_select(l_oRect);
								}
						}
					}
					else
					{
						deselect_all();
					}
				}

			}
			break;
		case link_mode:
			{
				m_oLastPressPoint = i_oEv->pos();
				m_bPressed = (i_oEv->button() == Qt::LeftButton);

				deselect_all();
				QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
				if (l_oItem && l_oItem->type() == CANVAS_ITEM_T)
				{
					l_oRect = (canvas_item*) l_oItem;
					add_select(l_oRect);
				}
			}
			break;
		case sort_mode:
			{
				QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
				if (l_oItem && l_oItem->type() == CANVAS_ITEM_T)
				{
					l_oRect = (canvas_item*) l_oItem;
					deselect_all();
					add_select(l_oRect);
				}
				else if (l_oItem && l_oItem->type() == CANVAS_SORT_T)
				{
					qDebug()<<"sort called"<<endl;

					canvas_sort * l_oSort = (canvas_sort*) l_oItem;
					int l_iId = l_oSort->m_oFrom->Id();
					l_oRect = l_oSort->m_oFrom;

					qDebug()<<"sort_children("<<m_oSelected[0]->Id()<<" "<<l_iId<<" "<<m_iSortCursor;

					m_oControl->sort_children(m_oSelected[0]->Id(), l_iId, m_iSortCursor);
					show_sort(m_oSelected[0]->Id(), true);

					m_iSortCursor++;
					if (m_iSortCursor >= m_oControl->num_children(m_oSelected[0]->Id()))
						m_iSortCursor=0;

					emit sig_message(trUtf8("Click to set Item %1").arg(QString::number(m_iSortCursor+1)), -1);
					//qDebug();
				}
				else
				{
					deselect_all();
				}
			}
			break;
		case scroll_mode:
			viewport()->setCursor(Qt::ClosedHandCursor);
			m_oLastPressPoint = i_oEv->pos();
			m_bPressed = (i_oEv->button() == Qt::LeftButton || i_oEv->button() == Qt::MidButton);

			if (m_bPressed)
			{
				deselect_all();
				QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
				if (l_oItem && l_oItem->type() == CANVAS_ITEM_T)
				{
					l_oRect = (canvas_item*) l_oItem;
					add_select(l_oRect);
				}
			}

			break;
		default:
			break;
	}
}

void canvas_view::mouseMoveEvent___________________________(QMouseEvent *i_oEv)
{
	switch (m_iMode)
	{
		case select_mode:
			if (m_bPressed && m_oSelected.size() == 0)
			{
				bool c1 = ((m_oLastPressPoint - i_oEv->pos()).manhattanLength() >= QApplication::startDragDistance());
				if (m_oRubbery->isVisible() || c1)
				{
					QRect l_oSel = QRect(m_oLastPressPoint, i_oEv->pos()).normalized();
					m_oRubbery->setGeometry(l_oSel);
				}

				if (c1) m_oRubbery->show();
			}
			else if (m_bPressed)
			{
				// ugly, but it works

				QRectF l_oRect;
				l_oRect.setTopLeft(m_oLastMovePoint);
				m_oLastMovePoint = mapToScene(i_oEv->pos());
				l_oRect.setBottomRight(m_oLastMovePoint);
				m_oControl->set_dirty();
				foreach (canvas_item *l_oItem, m_oSelected)
				{
					l_oItem->moveBy(l_oRect.width(), l_oRect.height());
				}
				// FIXME
				foreach (canvas_item *l_oItem, m_oSelected)
				{
					l_oItem->update_links();
				}
				check_canvas_size();
			}
			break;
		case link_mode:
			if (m_bPressed)
			{
				bool c1 = ((m_oLastPressPoint - i_oEv->pos()).manhattanLength() >= QApplication::startDragDistance());
				if (m_oRubberLine->isVisible() || c1)
				{
					QRect l_oSel = QRect(m_oLastPressPoint, i_oEv->pos());
					m_oRubberLine->setGeometry(l_oSel);
				}

				if (c1)
				{
					m_oRubberLine->show();
				}
			}
			break;
		case sort_mode:
			break;
		case scroll_mode:
			if (m_bPressed)
			{
				QScrollBar *hBar = horizontalScrollBar();
				QScrollBar *vBar = verticalScrollBar();

				QPoint delta = i_oEv->pos() - m_oLastPressPoint;

				hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
				vBar->setValue(vBar->value() - delta.y());

				m_oLastPressPoint = i_oEv->pos();

				//viewport()->setCursor(Qt::ClosedHandCursor);
			}
			else
			{
				//viewport()->setCursor(Qt::OpenHandCursor);
			}
		default:
			break;
	}
}

void canvas_view::mouseReleaseEvent______________________________________(QMouseEvent *i_oEv)
{
	if (i_oEv->button() == Qt::RightButton) return;

	if (m_iLastMode != no_mode)
	{
		m_iMode = m_iLastMode;
		m_iLastMode = no_mode;
	}

	switch (m_iMode)
	{
		case select_mode:
			{
				m_bPressed = false;
				if (m_oRubbery->isVisible())
				{
					m_oRubbery->hide();
					QRect l_oRect = m_oRubbery->geometry();
					QList<QGraphicsItem *> l_oSel = scene()->items(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));

					foreach (QGraphicsItem *l_oItem, l_oSel)
					{
						if (l_oItem->type() == CANVAS_ITEM_T)
						{
							add_select((canvas_item*) l_oItem);
						}
					}
				}
			}
			break;
		case link_mode:
			{
				m_bPressed = false;

				canvas_item *l_oR1 = NULL;
				canvas_item *l_oR2 = NULL;

				foreach (QGraphicsItem *l_oI1, scene()->items(mapToScene(m_oLastPressPoint)))
				{
					if (l_oI1->type() == CANVAS_ITEM_T)
					{
						l_oR1 = (canvas_item*) l_oI1;
						break;
					}
				}

				foreach (QGraphicsItem *l_oI1, scene()->items(mapToScene(i_oEv->pos())))
				{
					if (l_oI1->type() == CANVAS_ITEM_T)
					{
						l_oR2 = (canvas_item*) l_oI1;
						break;
					}
				}

				if (l_oR1 && l_oR2 && l_oR1 != l_oR2)
				{
					m_oControl->link_items(l_oR1->Id(), l_oR2->Id());
					deselect_all();
				}
				m_oRubberLine->hide();
			}
			break;
		case sort_mode:
			break;
		case scroll_mode:
			viewport()->setCursor(Qt::OpenHandCursor);
			m_bPressed = false;
			break;
		default:
			break;
	}
	update_cursor();
}
#endif


