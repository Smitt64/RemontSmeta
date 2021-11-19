#include "questionstreeview.h"
#include <QKeyEvent>

QuestionsTreeView::QuestionsTreeView(QWidget *parent) :
    QTreeView(parent)
{

}

QuestionsTreeView::~QuestionsTreeView()
{

}

void QuestionsTreeView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        // we captured the Enter key press, now we need to move to the next row
        qint32 nNextRow = currentIndex().row() + 1;
        if (nNextRow + 1 > model()->rowCount(currentIndex()))
        {
            // we are all the way down, we can't go any further
            nNextRow = nNextRow - 1;
        }

        if (state() == QAbstractItemView::EditingState)
        {
            // if we are editing, confirm and move to the row below
            QModelIndex oNextIndex = model()->index(nNextRow, currentIndex().column());
            setCurrentIndex(oNextIndex);
            selectionModel()->select(oNextIndex, QItemSelectionModel::ClearAndSelect);
        }
        else
        {
            // if we're not editing, start editing
            QModelIndex oNextIndex = currentIndex();

            if (oNextIndex.isValid() && (model()->flags(oNextIndex) & Qt::ItemIsEditable))
                edit(oNextIndex);
        }
    }
    else
    {
        // any other key was pressed, inform base class
        QTreeView::keyPressEvent(event);
    }
}
