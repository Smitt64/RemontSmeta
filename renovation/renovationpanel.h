#ifndef RENOVATIONPANEL_H
#define RENOVATIONPANEL_H

#include <QDialog>
#include <QMainWindow>
#include "subwindowbase.h"

namespace Ui {
class RenovationPanel;
}

class JsonTableModel;
class RenovationContentWidget;
class RenovationPanel : public QDialog
{
    Q_OBJECT

public:
    explicit RenovationPanel(QWidget *parent = nullptr);
    ~RenovationPanel();

/*private slots:
    void onClientsClick();*/

private:
    Ui::RenovationPanel *ui;

    QScopedPointer<RenovationContentWidget> m_ContentWidget;
    //JsonTableModel *m_pTypeModel, *m_PromotionsModel, *m_pHouseModel;
};

/*class RenovationWindow : public QMainWindow
{
public:
    RenovationWindow();

private:
    QScopedPointer<RenovationPanel> m_pPanel;
};*/

/*class RenovationTabWindow : SubWindowBase
{
public:
    RenovationWindow(QWidget *parent = nullptr);
};*/

#endif // RENOVATIONPANEL_H
