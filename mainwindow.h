#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QDockWidget;
class QMdiArea;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addClientAction();
    void navigationClicked(const QString &mime);

private:
    void setupDocks();
    void setupMenus();
    QDockWidget *m_pClientsDock;
    QMdiArea *m_pMdiArea;
    QMenuBar *m_MainMenuBar;
    QMenu *m_DictionaryMenu;
};

#endif // MAINWINDOW_H
