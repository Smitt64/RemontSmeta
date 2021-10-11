#ifndef RENOVATIONPANEL_H
#define RENOVATIONPANEL_H

#include <QDialog>

namespace Ui {
class RenovationPanel;
}

class RenovationPanel : public QDialog
{
    Q_OBJECT

public:
    explicit RenovationPanel(QWidget *parent = nullptr);
    ~RenovationPanel();

private:
    Ui::RenovationPanel *ui;
};

#endif // RENOVATIONPANEL_H
