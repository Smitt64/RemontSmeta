#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QDockWidget;
class QMdiArea;
class QStatusBar;
class QComboBox;
class SubWindowsModel;
class SubWindowBase;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class SubWindowsMenu;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addClientAction();
    void subWindowRequestNewWindow(QWidget *widget);
    void navigationClicked(const QString &mime, const QString &param, const QIcon &icon);
    void currentWindowSelected(const int &index);

private:
    void setupDocks();
    void setupMenus();
    void setupStatusBar();
    void setupWindowsList();
    void addWindowPrivate(SubWindowBase *wnd);

    QDockWidget *m_pClientsDock;
    QStatusBar *m_pStatusBar;
    QMdiArea *m_pMdiArea;
    QMenuBar *m_MainMenuBar;
    QMenu *m_DictionaryMenu, *m_ViewMenu;

    QScopedPointer<SubWindowsModel> m_WindowsModel;
    QScopedPointer<QMenu> m_WindowsMenu;
};

#endif // MAINWINDOW_H
