#ifndef RENOVATIONPANEL_H
#define RENOVATIONPANEL_H

#include <QDialog>

namespace Ui {
class RenovationPanel;
}

class JsonTableModel;
class RenovationPanel : public QDialog
{
    Q_OBJECT

public:
    explicit RenovationPanel(QWidget *parent = nullptr);
    ~RenovationPanel();

private slots:
    void onClientsClick();

private:
    Ui::RenovationPanel *ui;

    JsonTableModel *m_pTypeModel, *m_PromotionsModel, *m_pHouseModel;
};

#endif // RENOVATIONPANEL_H
