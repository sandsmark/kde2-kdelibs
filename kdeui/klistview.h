/* This file is part of the KDE libraries
   Copyright (C) 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Charles Samuels <charles@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef KLISTVIEW_H
#define KLISTVIEW_H

#include <qvector.h>
#include <qlistview.h>

#include <qlist.h>

class QDragObject;
/**
 * This Widget extends the functionality of QListView to honor the system
 * wide settings for Single Click/Double Click mode, AutoSelection and
 * ChangeCursorOverLink (TM).
 *
 * There is a new signal executed(). It gets connected to either
 * @ref QListView::clicked() or  @ref QListView::doubleClicked() depending on the KDE
 * wide Single Click/Double Click settings. It is strongly recommended that
 * you use this signal instead of the above mentioned. This way you don�t
 * need to care about the current settings.
 * If you want to get informed when the user selects something connect to the
 * QListView::selectionChanged() signal.
 *
 * Drag-and-Drop is supported with the signal @ref #dropped(), just @ref #setAcceptDrops(true)
 * and connect it to a suitable slot.
 * To see where you are dropping, @ref setDropVisualizer(true).
 * And also you'll need @ref acceptDrag(QDropEvent*)
 *
 * KListView is drag-enabled, too: to benefit from that you've got derive from it.
 * Reimplement @ref dragObject() and (possibly) @ref startDrag(),
 * and @ref setDragEnabled(true).
 *
 * @version $Id$
 */
class KListView : public QListView
{
  Q_OBJECT

public:
  /**
   * Possible selection modes.
   *
   * The first four correspond directly to @ref QListView::SelectionMode, while
   * the Konqueror selection mode is defined as follows:
   *   @li home: move to the first
   *   @li end: move to the last
   *   @li PgUp/PgDn: move one page up/down
   *   @li up/down: move one item up/down
   *   @li insert: toggle selection of current and move to the next
   *   @li space: toggle selection of the current
   *   @li SHIFT+CTRL+up: move to the previous item and toggle selection of this one
   *   @li SHIFT+CTRL+down: toggle selection of the current item and move to the next
   *   @li SHIFT+CTRL+end: toggle selection from (including) the current
   *   item to (including) the last item
   *   @li SHIFT+CTRL+home: toggle selection from (including) the current
   *   item to the (including) the first item
   *   @li SHIFT+CTRL+PgDn: toggle selection from (including) the current
   *   item to (excluding) the item one page down
   *   @li SHIFT+CTRL+PgUp: toggle selection from (excluding) the current
   *   item to (including) the item one page up
   *
   *    The combinations work the same with SHIFT instead of CTRL, except
   *    that if you start selecting something using SHIFT everything selected
   *    before will be deselected first.
   *
   *   This way e.g. SHIFT+up/PgUp then SHIFT+down/PgDn leaves no item selected
   */
  enum SelectionModeExt {
	Single = QListView::Single,
	Multi = QListView::Multi,
	Extended = QListView::Extended,
	NoSelection = QListView::NoSelection,
	Konqueror
  };

  /**
   * Constructor.
   *
   * The parameters @p parent and @p name are handled by
   * @ref QListView, as usual.
   */
  KListView (QWidget *parent = 0, const char *name = 0);

  /**
   * Destructor.
   */
  virtual ~KListView();

 /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void setAcceptDrops (bool);

  /**
   * This function determines whether the given coordinates are within the
   * execute area. The execute area is the part of a @ref QListViewItem where mouse
   * clicks or double clicks respectively generate a @ref #executed() signal.
   * Depending on @ref QListView::allColumnsShowFocus() this is either the
   * whole item or only the first column.
   * @return true if point is inside execute area of an item, false in all
   * other cases including the case that it is over the viewport.
   */
  virtual bool isExecuteArea( const QPoint& point);

  /**
   * @return a list containing the currently selected items.
   */
  QList<QListViewItem> selectedItems() const;

  /**
   * Arbitrarily move @p item to @p parent, positioned immediately after item @p after.
   */
  void moveItem(QListViewItem *item, QListViewItem *parent, QListViewItem *after);

  /**
   * @return the last item of this listview.
   */
  QListViewItem *lastItem() const;

  /**
   * For future expansions.
   *
   * Do not use.
   * @deprecated
   */
  bool itemsMovable() const;

  /**
   * @return whether inplace-renaming has been enabled.
   *
   * @see #setItemsRenameable()
   */
  bool itemsRenameable() const;

  /**
   * @return whether dragging is enabled.
   */
  bool dragEnabled() const;

  /**
   * @return true if AutoOpen is enabled.
   *
   * @see #setAutoOpen()
   */
  bool autoOpen() const;

  /**
   * @return true if @p column is renamable.
   */
  bool isRenameable (int column) const;

  /**
   * @return true if drawing of the drop-visualizer has been enabled.
   */
  bool dropVisualizer() const;

  /**
   * @return the column for which tooltips are displayed (or -1 if none set).
   */
  int tooltipColumn() const;

  /**
   * For future expansions.
   *
   * Do not use.
   * @deprecated
   */
  bool createChildren() const;

  /**
   * For future expansions.
   *
   * Do not use.
   * @deprecated
   */
  bool dropHighlighter() const;

  /**
   * The dropVisualizerWidth defaults to 4.
   *
   * @return the current width of the drop-visualizer.
   */
  inline int dropVisualizerWidth () const { return mDropVisualizerWidth; }

  /**
   * @return the "extended" selection mode of this listview.
   *
   * @see SelectionModeExt
   * @see setSelectionModeExt
   */
  SelectionModeExt selectionModeExt () const;

signals:

  /**
   * This signal is emitted whenever the user executes an listview item.
   * That means depending on the KDE wide Single Click/Double Click
   * setting the user clicked or double clicked on that item.
   * @param item is the pointer to the executed listview item.
   *
   * Note that you may not delete any @ref QListViewItem objects in slots
   * connected to this signal.
   */
  void executed( QListViewItem *item );

  /**
   * This signal is emitted whenever the user executes an listview item.
   * That means depending on the KDE wide Single Click/Double Click
   * setting the user clicked or double clicked on that item.
   * @param item is the pointer to the executed listview item.
   * @param pos is the position where the user has clicked
   * @param c is the column into which the user clicked.
   *
   * Note that you may not delete any @ref QListViewItem objects in slots
   * connected to this signal.
   */
  void executed( QListViewItem *item, const QPoint &pos, int c );

  /**
   * This signal gets emitted whenever the user double clicks into the
   * listview.
   * @param item is the pointer to the clicked listview item.
   * @param pos is the position where the user has clicked, and
   * @param c is the column into which the user clicked.
   *
   * Note that you may not delete any @ref QListViewItem objects in slots
   * connected to this signal.
   *
   * This signal is more or less here for the sake of completeness.
   * You should normally not need to use this. In most cases it�s better
   * to use @ref #executed() instead.
   */
  void doubleClicked( QListViewItem *item, const QPoint &pos, int c );

  /**
   * This signal gets emitted whenever something acceptable is
   * dropped onto the listview.
   *
   * @param e is the drop event itself (it has already been accepted)
   * @param after is the item after which the drop occured (or 0L, if
   * the drop was above all items)
   *
   * @see #acceptDrop()
   */
  void dropped (QDropEvent * e, QListViewItem *after);
  
  /**
   * This signal gets emitted whenever something acceptable is
   * dropped onto the listview.
   *
   * This is an overloaded version of the above (provided to simplify
   * processing drops outside of the class).
   *
   * @param list is the listview
   * @param e is the drop event itself (it has already been accepted)
   * @param after is the item after which the drop occured (or 0L, if
   * the drop was above all items
   */
  void dropped (KListView* list, QDropEvent* e, QListViewItem* after);

  /**
   * This signal gets emitted whenever something acceptable is
   * dropped onto the listview.
   *
   * This function also provides a parent, in the event that your listview
   * is a tree
   * @param list is the listview
   * @param e is the drop event itself (it has already been accepted)
   * @param parent the item that is to be the parent of the new item
   * @param after is the item after which the drop occured (or 0L, if
   * the drop was above all items
   */
  void dropped (KListView* list, QDropEvent* e, QListViewItem* parent, QListViewItem* after);

  /**
   * This signal gets emitted whenever something acceptable is
   * dropped onto the listview.
   *
   * This function also provides a parent, in the event that your listview
   * is a tree
   * @param e is the drop event itself (it has already been accepted)
   * @param parent the item that is to be the parent of the new item
   * @param after is the item after which the drop occured (or 0L, if
   * the drop was above all items
   */
  void dropped(QDropEvent* e, QListViewItem* parent, QListViewItem* after);

  /**
   * This signal is emitted when ever the user moves an item in the list via
   * DnD.
   * If more than one item is moved at the same time, this signal is only emitted
   * once.
   */
  void moved();

  /**
   * This signal is emitted when ever the user moves an item in the list via
   * DnD.
   * If more than one item is moved at the same time, parameters afterFirst and
   * afterNow will reflect what was true before the move.
   * This differs from @ref moved() so be careful.  All the items will have been
   * moved before this is emitted in @moved(), which is not true in this method.
   * @param item the item that was moved
   * @param afterFirst the item that parameter item was in before the move, in the list
   * @param afterNow the item it's currently after.
   *
   **/
  void moved(QListViewItem *item, QListViewItem *afterFirst, QListViewItem *afterNow);

  /**
   * This signal is emitted after all the items have been moved. It reports info for
   * each and every item moved, in order.  The first element in @param items associates
   * with the first of afterFirst and afterNow.
   **/
  void moved(QVector<QListViewItem> &items, QVector<QListViewItem> &afterFirst, QVector<QListViewItem> &afterNow);

  /**
   * This signal gets emitted when an item is renamed via in-place renaming.
   *
   * @param item is the renamed item.
   * @param str is the new value of column @p col.
   * @param col is the renamed column.
   */
  void itemRenamed(QListViewItem* item, const QString &str, int col);

  /**
   * Same as above, but without the extra information.
   */
  void itemRenamed(QListViewItem* item);

  /**
   * This signal is emitted when the shortcut key for popup-menus is pressed.
   *
   * Normally you should not use this, just connect a slot to signal
   * @ref contextMenu (KListView*, QListViewItem*, const QPoint&) to correctly
   * handle showing context menus regardless of settings.
   *
   * @param list is this listview.
   * @param item is the @ref currentItem() at the time the key was pressed. May be 0L.
   */
  void menuShortCutPressed (KListView* list, QListViewItem* item);

  /**
   * This signal is emitted whenever a context-menu should be shown for item @p i.
   * It automatically adjusts for all settings involved (Menu key, showMenuOnPress/Click).
   *
   * @param l is this listview.
   * @param i is the item for which the menu should be shown. May be 0L.
   * @param p is the point at which the menu should be shown.
   */
  void contextMenu (KListView* l, QListViewItem* i, const QPoint& p);

public slots:
  /**
   * Rename column @p c of @p item.
   */
  virtual void rename(QListViewItem *item, int c);

  /**
   * is column renameable? Set it.  by default, all columns
   * are not renameable.  If you want more intelligent
   * selection, you'll have to derive from KListView,
   * and override @ref rename() and call only call it
   * if you want the item to be renamed.
   **/
  void setRenameable (int column, bool yesno=true);

  /**
   * For future expansions.
   *
   * Do not use.
   * @deprecated
   */
  virtual void setItemsMovable(bool b);

  /**
   * Enables inplace-renaming of items.
   *
   * @see itemsRenameable()
   * @see setRenameable()
   */
  virtual void setItemsRenameable(bool b);

  /**
   * Enable/Disable the dragging of items.
   */
  virtual void setDragEnabled(bool b);

  /**
   * Enable/Disable AutoOpen.
   */
  virtual void setAutoOpen(bool b);

  /**
   * Enable/Disable the drawing of a drop-visualizer
   * (a bar that shows where a dropped item would be inserted).
   */
  virtual void setDropVisualizer(bool b);

  /**
   * Set the width of the (default) drop-visualizer.
   */
  void setDropVisualizerWidth (int w);

  /**
   * Set which column should be used for automatic tooltips.
   *
   * @param column is the column for which tooltips will be shown.
   * Set -1 to disable this feature.
   */
  virtual void setTooltipColumn(int column);

  /**
   * For future expansion.
   *
   * Do not use.
   * @deprecated
   * Highlight a parent if I drop into its children
   */
  virtual void setDropHighlighter(bool b);

  /**
   * For future expansions.
   *
   * Do not use.
   * @deprecated
   */
  virtual void setCreateChildren(bool b);

  /**
   * Set the selection mode.
   *
   * A different name was chosen to avoid API-clashes with @ref QListView::setSelectionMode().
   */
  void setSelectionModeExt (SelectionModeExt mode);

protected:
  /**
   * Determine whether a drop on position @p p would count as
   * being above or below the QRect @p rect.
   *
   * @param rect is the rectangle we examine.
   * @param p is the point located in the rectangle, p is assumed to be in
   * viewport coordinates.
   */
  inline bool below (const QRect& rect, const QPoint& p)
  {
	return (p.y() > (rect.top() + (rect.bottom() - rect.top())/2));
  }

  /**
   * An overloaded version of below(const QRect&, const QPoint&).
   *
   * It differs from the above only in what arguments it takes.
   *
   * @param i the item whose rect() is passed to the above function.
   * @param p is translated from contents coordinates to viewport coordinates
   * before being passed to the above function.
   */
  inline bool below (QListViewItem* i, const QPoint& p)
  {
	return below (itemRect(i), contentsToViewport(p));
  }

  /**
   * Emit signal @ref execute.
   * @internal
   */
  void emitExecute( QListViewItem *item, const QPoint &pos, int c );

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void focusOutEvent( QFocusEvent *fe );

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void leaveEvent( QEvent *e );

  /**
   * @return the tooltip for @p column of @p item.
   */
  virtual QString tooltip(QListViewItem* item, int column) const;

  /**
   * @return whether the tooltip for @p column of @p item shall be shown at point @p pos.
   */
  virtual bool showTooltip(QListViewItem *item, const QPoint &pos, int column) const;

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsDragMoveEvent (QDragMoveEvent *event);

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsMousePressEvent( QMouseEvent *e );

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsMouseMoveEvent( QMouseEvent *e );

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsMouseDoubleClickEvent ( QMouseEvent *e );

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsDragLeaveEvent (QDragLeaveEvent *event);

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsMouseReleaseEvent (QMouseEvent*);

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsDropEvent (QDropEvent*);

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void contentsDragEnterEvent (QDragEnterEvent *);

  /**
   * @return a dragobject encoding the current selection.
   *
   * @see setDragEnabled()
   */
  virtual QDragObject *dragObject() const;

  /**
   * @return true if the @p event provides some acceptable
   * format.
   */
  virtual bool acceptDrag (QDropEvent* event) const;

  /**
   * Paint the drag line. If painter is null, don't try to :)
   *
   * If after == 0 then the marker should be drawn at the top.
   *
   * @return the rectangle that you painted to.
   */
  virtual QRect drawDropVisualizer (QPainter *p, QListViewItem *parent, QListViewItem *after);	

  /**
   * For future expansion.
   *
   * Do not use.
   *
   * Highlight @p item.  painter may be null
   * return the rect drawn to
   * @deprecated
   */
  virtual QRect drawItemHighlighter(QPainter *painter, QListViewItem *item);

  /**
   * This method calls @ref dragObject() and starts the drag.
   *
   * Reimplement it to do fancy stuff like setting a pixmap or
   * using a non-default DragMode
   */
  virtual void startDrag();

  /**
   * Reimplemented for internal reasons.
   * Further reimplementations should call this function or else
   * some features may not work correctly.
   *
   * The API is unaffected.
   */
  virtual void keyPressEvent (QKeyEvent*);

private slots:
  /**
   * Update internal settings whenever the global ones change.
   * @internal
   */
  void slotSettingsChanged(int);

  void slotMouseButtonClicked( int btn, QListViewItem *item, const QPoint &pos, int c );
  void doneEditing(QListViewItem *item, int row);
  /**
   * Repaint the rect where I was drawing the drop line.
   */
  void cleanDropVisualizer();
  /**
   * For future expansion.
   *
   * Do not use.
   * @deprecated
   */
  void cleanItemHighlighter ();

  /**
   * Emit the @ref contextMenu signal. This slot is for mouse actions.
   */
  void emitContextMenu (QListViewItem*, const QPoint&, int);

  /**
   * Emit the @ref contextMenu signal. This slot is for key presses.
   */
  void emitContextMenu (KListView*, QListViewItem*);

  /**
   * Accessory slot for AutoSelect
   * @internal
   */
  void slotOnItem( QListViewItem *item );

  /**
   * Accessory slot for AutoSelect/ChangeCursorOverItem
   * @internal
   */
  void slotOnViewport();

  /**
   * Process AutoSelection.
   * @internal
   */
  void slotAutoSelect();

private:
  /**
   * Where is the nearest QListViewItem that I'm going to drop?
   **/
  void findDrop(const QPoint &pos, QListViewItem *&parent, QListViewItem *&after);

  /**
   * A special keyPressEvent (for Konqueror-style selection).
   */
  void konquerorKeyPressEvent (QKeyEvent*);

private:
  class KListViewPrivate;
  class Tooltip;
  KListViewPrivate *d;

  QRect mOldDropVisualizer;
  int mDropVisualizerWidth;
};

#endif
