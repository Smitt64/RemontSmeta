#ifndef SELECTPARTYDLG_H
#define SELECTPARTYDLG_H

#include <QDialog>
#include <QHash>

namespace Ui {
class SelectPartyDlg;
}

typedef QHash<QString,QVariant> SelectPartyResult;

class QMainWindow;
class QListView;
class QToolBar;
class QLineEdit;
class ClientsViewModel;
class QSortFilterProxyModel;
class SelectPartyDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SelectPartyDlg(QWidget *parent = nullptr);
    ~SelectPartyDlg();

    const SelectPartyResult &result() const;

private:
    Ui::SelectPartyDlg *ui;

    QScopedPointer<QMainWindow> m_pWindow;
    QScopedPointer<QListView> m_ClientsView;
    QScopedPointer<QToolBar> m_MainToolBar;
    QScopedPointer<QLineEdit> m_FilterEdit;

    QScopedPointer<QSortFilterProxyModel> m_FilterModel;
    QScopedPointer<ClientsViewModel> m_ClientsModel;

    SelectPartyResult m_Result;

    QAction *m_pNewParty, *m_pViewParty;
};

#endif // SELECTPARTYDLG_H
