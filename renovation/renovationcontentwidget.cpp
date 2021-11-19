#include "renovationcontentwidget.h"
#include "ui_renovationcontentwidget.h"
#include "globals.h"
#include "exceptionbase.h"
#include "models/jsontablemodel.h"
#include "widgets/selectpartydlg.h"
#include "tables/renovation.h"
#include "renovation/trenovationservice.h"
#include "renovation/renovationquesttionmodel.h"
#include "renovation/renovationquestitemdelegate.h"
#include "models/jsontablemodel.h"
#include "renovation/questionstreeview.h"
#include <QAction>
#include <QHeaderView>
#include <QKeyEvent>

RenovationContentWidget::RenovationContentWidget(const quint16 &renovation, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RenovationContentWidget)
{
    ui->setupUi(this);

    m_Service.reset(new TRenovationService(renovation));
    m_Service->start();

    m_Table = m_Service->createTableObj();
    m_Rooms = m_Service->roomsTable();

    if (renovation)
    {
        m_Table->find((quint16)0, QVariantList() << renovation);
    }
    else
    {
        if (m_Table->newRec() && m_Table->insert())
        {
            /*setWindowTitle(tr("Ввод нового субъекта"));
            setWindowIcon(QIcon::fromTheme("NewTeam"));*/
        }
    }

    try {
        m_RoomQuestions.reset(new QuestionsTreeView());
        ui->formTab->layout()->addWidget(m_RoomQuestions.data());

        m_RoomsQuestionModel.reset(new RenovationQuestionModel(dynamic_cast<TRoom*>(m_Rooms.data())));
        m_RoomQuestions->setModel(m_RoomsQuestionModel.data());
        m_RoomQuestions->setRootIsDecorated(false);
        m_RoomQuestions->setUniformRowHeights(true);
        m_RoomQuestions->header()->resizeSection(0, 250);

        m_QuestItemDelegate.reset(new RenovationQuestItemDelegate(m_RoomsQuestionModel.data()));
        m_RoomQuestions->setItemDelegate(m_QuestItemDelegate.data());

        m_pTypeModel = Globals::inst()->model(JSON_RENOVATION_TYPE);
        m_PromotionsModel = Globals::inst()->model(JSON_PROMOTIONS);
        m_pHouseModel = Globals::inst()->model(JSON_BUILDTYPE);

        ui->typeComboBox->setModel(m_pTypeModel);
        ui->typeComboBox->setModelColumn(1);

        ui->promotionsBox->setModel(m_PromotionsModel);
        ui->promotionsBox->setModelColumn(1);

        ui->houseComboBox->setModel(m_pHouseModel);
        ui->houseComboBox->setModelColumn(1);

        ui->roomsListView->setModel(m_Rooms.data());
        ui->roomsListView->setModelColumn(2);

        setupToolBar();
        setupComboBoxEvents();

        connect(ui->clientButton, &QToolButton::clicked, this, &RenovationContentWidget::onClientsClick);
        connect(m_pAddRoom, &QToolButton::clicked, [=]()
        {
            JsonTableModel *model = Globals::inst()->model(JSON_ROOMS);

            QModelIndexList lst = model->match(model->index(0, 0), Qt::DisplayRole, 8);

            if (!lst.isEmpty())
            {
                QModelIndex index = lst.first();
                addRoom(model->data(model->index(index.row(), 1)).toString());
            }
        });
    }  catch (ExceptionBase &e) {

    }

}

RenovationContentWidget::~RenovationContentWidget()
{
    delete ui;
}

void RenovationContentWidget::onClientsClick()
{
    SelectPartyDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        const SelectPartyResult &result = dlg.result();

        m_Table->setValue("t_personid", result["id"]);
        ui->clientEdit->setText(result["name"].toString());
    }
}

void RenovationContentWidget::setupToolBar()
{
    m_pAddRoom = new QToolButton(this);
    m_pAddRoom->setIcon(QIcon::fromTheme("AddRoom"));
    m_pAddRoom->setText(tr("Добавить комнату"));
    m_pSave = ui->toolBar->addAction(QIcon::fromTheme("Save"), tr("Сохранить изменения"));
    ui->toolBar->addWidget(m_pAddRoom);

    m_pRoomMenu = new QMenu(this);
    m_pAddRoom->setMenu(m_pRoomMenu);
    m_pAddRoom->setPopupMode(QToolButton::MenuButtonPopup);

    JsonTableModel *roomsModel = Globals::inst()->model(JSON_ROOMS);
    for (int i = 0; i < roomsModel->rowCount(); i++)
    {
        QAction *room = m_pRoomMenu->addAction(roomsModel->data(roomsModel->index(i, 1)).toString());
        room->setData(roomsModel->data(roomsModel->index(i, 1)));

        connect(room, &QAction::triggered, this, &RenovationContentWidget::onAddRoom);
    }
}

void RenovationContentWidget::setupComboBoxEvents()
{
    connect(ui->typeComboBox, &QComboBox::currentIndexChanged, [=](const int &index)
    {
        int type = m_pTypeModel->data(m_pTypeModel->index(index, 0)).toInt();
        m_Table->setValue("t_type", type);
    });

    connect(ui->promotionsBox, &QComboBox::currentIndexChanged, [=](const int &index)
    {
        int type = m_PromotionsModel->data(m_PromotionsModel->index(index, 0)).toInt();
        m_Table->setValue("t_stock", type);
    });

    connect(ui->houseComboBox, &QComboBox::currentIndexChanged, [=](const int &index)
    {
        int type = m_pHouseModel->data(m_pHouseModel->index(index, 0)).toInt();
        m_Table->setValue("t_housetype", type);
    });
}

void RenovationContentWidget::addRoom(const QString &text)
{
    if (m_Rooms->newRec())
    {
        m_Rooms->setValue("t_name", text);

        if (!m_Rooms->insert())
            m_Rooms->revert();
    }
}

void RenovationContentWidget::onAddRoom()
{
    QAction *action = qobject_cast<QAction*>(sender());

    if (action)
        addRoom(action->text());
}
