#ifndef RENOVATIONCONTENTWIDGET_H
#define RENOVATIONCONTENTWIDGET_H

#include <QMainWindow>

namespace Ui {
class RenovationContentWidget;
}

class JsonTableModel;
class QAction;
class QToolButton;
class DbTable;
class TRenovation;
class TRenovationService;
class RenovationQuestionModel;
class RenovationQuestItemDelegate;
class QuestionsTreeView;
class RenovationContentWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit RenovationContentWidget(const quint16 &renovation, QWidget *parent = nullptr);
    ~RenovationContentWidget();

private slots:
    void onClientsClick();
    void onAddRoom();

private:
    void addRoom(const QString &text);
    void setupToolBar();
    void setupComboBoxEvents();
    Ui::RenovationContentWidget *ui;

    QSharedPointer<DbTable> m_Table;
    QSharedPointer<DbTable> m_Rooms;
    QScopedPointer<TRenovationService> m_Service;

    QScopedPointer<QuestionsTreeView> m_RoomQuestions;
    QScopedPointer<RenovationQuestionModel> m_RoomsQuestionModel;
    QScopedPointer<RenovationQuestItemDelegate> m_QuestItemDelegate;

    JsonTableModel *m_pTypeModel, *m_PromotionsModel, *m_pHouseModel;
    QAction *m_pSave;
    QToolButton *m_pAddRoom;
    QMenu *m_pRoomMenu;
};

#endif // RENOVATIONCONTENTWIDGET_H
