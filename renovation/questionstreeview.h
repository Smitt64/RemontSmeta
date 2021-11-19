#ifndef QUESTIONSTREEVIEW_H
#define QUESTIONSTREEVIEW_H

#include <QTreeView>

class QKeyEvent;
class QuestionsTreeView : public QTreeView
{
    Q_OBJECT
public:
    QuestionsTreeView(QWidget *parent = nullptr);
    virtual ~QuestionsTreeView();

protected:
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
};

#endif // QUESTIONSTREEVIEW_H
