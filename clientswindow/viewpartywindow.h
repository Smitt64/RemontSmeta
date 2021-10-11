#ifndef VIEWPARTYWINDOW_H
#define VIEWPARTYWINDOW_H

#include "subwindowbase.h"

class QMainWindow;
class QSplitter;
class QTreeView;
class QTableView;
class QToolBar;
class ClientsViewModel;
class GenderColumnDlegate;
class QItemSelection;
class ViewPartyWindow : public SubWindowBase
{
    Q_OBJECT
public:
    enum PartyAction
    {
        PA_Create = 0,
        PA_ViewInTab,
        PA_Edit,
        PA_Remove,
    };
    ViewPartyWindow(QWidget *parent = nullptr);
    virtual ~ViewPartyWindow();

    QSplitter *splitter() const;
    ClientsViewModel *clientsModel();

protected:
    virtual void initModel();
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private slots:
    void addParty();
    void partyDoubleClicked(const QModelIndex &index);
    void editPerson();
    void removePartyAction();
    void openParty();
    void partyContextMenuRequested(const QPoint &pos);
    void partySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void setupClientsList();
    void updateActionsEnable(const bool &val);
    void createActionSeparator(QAction **action);
    //void setupWorsksList();
    QScopedPointer<QSplitter> m_Splitter;
    QScopedPointer<QMainWindow> m_ClientsWindow;

    QScopedPointer<QTableView> m_ClientsList;
    QScopedPointer<QToolBar> m_ClientsToolBar;
    QScopedPointer<ClientsViewModel> m_ClientsModel;
    QScopedPointer<GenderColumnDlegate> m_GenderDelegate;

    QList<QAction*> m_PartyActions; //m_pAddParty, *m_pRemoveParty, *m_pEditParty;
};

class ViewClientsWindow : public ViewPartyWindow
{
public:
    ViewClientsWindow(QWidget *parent = nullptr);
    virtual ~ViewClientsWindow();

protected:
    virtual void initModel() Q_DECL_OVERRIDE;

private:
    void setupWorsksList();

    QScopedPointer<QMainWindow> m_WorksWindow;
    QScopedPointer<QTreeView> m_WorksList;
};

#endif // VIEWPARTYWINDOW_H
