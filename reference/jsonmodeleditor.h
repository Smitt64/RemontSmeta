#ifndef JSONMODELEDITOR_H
#define JSONMODELEDITOR_H

#include <subwindowbase.h>
#include <QSortFilterProxyModel>

class JsonTableModel;
class QTableView;
class QToolBar;
class QMainWindow;
class FilteredTableWidget;
class JsonTableModelFilterController;

class JsonTableModelFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    JsonTableModelFilter(QObject *parent = nullptr);
    virtual ~JsonTableModelFilter();

    void setValue(const quint16 &column, const QVariant &value);
    bool isFiltered() const;

    bool isLastIndex(const QModelIndex &index) const;
    bool isFirstIndex(const QModelIndex &index) const;

public slots:
    void clearFilter();

signals:
    void filterChanged();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

private:
    QMap<quint16, QVariant> m_Filters;
};

// ------------------------------------------------

template<class T>
class OwnerPointer
{
public:
    OwnerPointer() :
        m_isCreated(false),
        pPtr(nullptr)
    {

    }

    virtual ~OwnerPointer()
    {
        if (m_isCreated && pPtr)
            delete pPtr;
    }

    operator bool()
    {
        return pPtr;
    }

    T *operator ->()
    {
        return pPtr;
    }

    void set(T *ptr)
    {
        pPtr = ptr;
    }

    void create(T *ptr)
    {
        pPtr = ptr;
        m_isCreated = true;
    }

    void destroy()
    {
        if (pPtr)
        {
            delete pPtr;
            pPtr = nullptr;
        }

        m_isCreated = false;
    }

    T *data()
    {
        return pPtr;
    }

private:
    bool m_isCreated;
    T *pPtr;
};
// ------------------------------------------------

class JsonTableModelEditor : public SubWindowBase
{
    Q_OBJECT
public:
    JsonTableModelEditor(QWidget *parent);
    virtual ~JsonTableModelEditor();

private slots:
    void updateActions();
    void insertRow();

protected:
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    bool setJsonFile(const QString &filename);
    virtual bool onInitModel();

private:
    void setupWindow();

    QAction *m_GoToFirst, *m_GoToPrevious, *m_GoToNext, *m_GoToLast;
    QAction *m_NewRow, *m_RemoveRow, *m_SaveTable, *m_ApplyChanges, *m_RevertChanges;
    QAction *m_FilterAction;

    QScopedPointer<QMainWindow> m_CentralWindow;
    QScopedPointer<FilteredTableWidget> m_FilteredTable;
    QScopedPointer<JsonTableModelFilterController> m_FilterController;
    QScopedPointer<QTableView> m_Table;
    QScopedPointer<JsonTableModelFilter> m_Filter;
    OwnerPointer<JsonTableModel> m_Model;
    QScopedPointer<QToolBar> m_ToolBar;

    bool m_IsFirstShow;
};

#endif // JSONMODELEDITOR_H
