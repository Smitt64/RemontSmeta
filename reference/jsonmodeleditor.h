#ifndef JSONMODELEDITOR_H
#define JSONMODELEDITOR_H

#include <subwindowbase.h>

class JsonTableModel;
class QTableView;
class QToolBar;
class QMainWindow;
class FilteredTableWidget;
class JsonTableModelFilterController;
class JsonTableModelEditor : public SubWindowBase
{
    Q_OBJECT
public:
    JsonTableModelEditor(QWidget *parent);
    virtual ~JsonTableModelEditor();

protected:
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void setJsonFile(const QString &filename);
    virtual void onInitModel();

private:
    void setupWindow();

    QAction *m_GoToFirst, *m_GoToPrevious, *m_GoToNext, *m_GoToLast;
    QAction *m_NewRow, *m_RemoveRow, *m_SaveTable;

    QScopedPointer<QMainWindow> m_CentralWindow;
    QScopedPointer<FilteredTableWidget> m_FilteredTable;
    QScopedPointer<JsonTableModelFilterController> m_FilterController;
    QScopedPointer<QTableView> m_Table;
    QScopedPointer<JsonTableModel> m_Model;
    QScopedPointer<QToolBar> m_ToolBar;
};

#endif // JSONMODELEDITOR_H
