#include "navigationdockwidget.h"
#include "models/treemodel.h"
#include "models/treeitem.h"
#include <QTreeView>
#include <QFile>
#include <QSqlQueryModel>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QMenu>

typedef struct
{
    QString title, icon, mime;
}ItemActionItem;
typedef QList<ItemActionItem> ItemActionItemList;

class NavigationModel : public TreeModel
{ 
public:
    enum CustomRoles
    {
        RoleMime = Qt::UserRole + 1,
        RoleActions
    };

    NavigationModel(QObject *parent = nullptr) :
        TreeModel({tr("Элементы")}, parent)
    {
    }

    void setupModelData() Q_DECL_OVERRIDE
    {
        TreeItem *rootItem = root();

        QFile file(":/tools/json/navigation.json");
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray data = file.readAll();

            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(data, &error);
            QJsonObject rootObject = doc.object();

            addElements(rootObject["elements"].toArray(), rootItem);
        }
    }

private:
    void addElements(const QJsonArray &elements, TreeItem *parent)
    {
        for (const auto &elem : elements)
            addItem(elem.toObject(), parent);
    }

    void addItem(const QJsonObject &obj, TreeItem *parent)
    {
        QString title = obj["title"].toString();
        QString icon = obj["icon"].toString();
        QString mime = obj["mime"].toString();

        TreeItem *item = new TreeItem({title}, parent);
        parent->appendChild(item);

        if (!icon.isEmpty())
            item->setRoleData(0, Qt::DecorationRole, QIcon::fromTheme(icon));

        if (!mime.isEmpty())
            item->setRoleData(0, NavigationModel::RoleMime, mime);

        if (obj.contains("actions"))
        {
            ItemActionItemList list;
            readActions(obj["actions"].toArray(), list);

            if (!list.isEmpty())
            {
                QVariant val;
                val.setValue(list);
                item->setRoleData(0, NavigationModel::RoleActions, val);
            }
        }

        if (obj.contains("elements"))
        {
            QJsonArray elements = obj["elements"].toArray();
            addElements(elements, item);
        }
    }

    void addActionToList(const QJsonObject &obj, ItemActionItemList &list)
    {
        ItemActionItem item;
        item.title = obj["title"].toString();
        item.icon = obj["icon"].toString();
        item.mime = obj["mime"].toString();

        list.append(item);
    }

    void readActions(const QJsonArray &elements, ItemActionItemList &list)
    {
        for (const auto &elem : elements)
            addActionToList(elem.toObject(), list);
    }
};

Q_DECLARE_METATYPE(ItemActionItem)
Q_DECLARE_METATYPE(ItemActionItemList)

// --------------------------------------------------------------------------

NavigationDockWidgetArea::NavigationDockWidgetArea(QWidget *parent) :
    QMainWindow(parent)
{
    m_pView = new QTreeView(this);
    m_pView->setIconSize(QSize(16, 16));
    m_pView->setUniformRowHeights(true);
    m_pModel = new NavigationModel(this);
    m_pModel->setupModelData();
    m_pView->setModel(m_pModel);
    m_pView->setContextMenuPolicy(Qt::CustomContextMenu);
    //m_pView->setRootIsDecorated(false);
    m_pView->expandAll();

    setWindowFlag(Qt::SubWindow, true);
    setCentralWidget(m_pView);

    connect(m_pView, &QTreeView::customContextMenuRequested, this, &NavigationDockWidgetArea::customContextMenuRequested);
    connect(m_pView, &QTreeView::doubleClicked, this, &NavigationDockWidgetArea::doubleClicked);
}

NavigationDockWidgetArea::~NavigationDockWidgetArea()
{

}

void NavigationDockWidgetArea::customContextMenuRequested(const QPoint &pos)
{
    QPoint world = mapToGlobal(pos);
    QModelIndex index = m_pView->indexAt(pos);

    if (index.isValid())
    {
        QList<QAction*> actions;
        const ItemActionItemList &list = m_pModel->data(index, NavigationModel::RoleActions).value<ItemActionItemList>();

        if (!list.isEmpty())
        {
            for (const auto &item : list)
            {
                QAction *action = new QAction(item.title);
                action->setIcon(QIcon::fromTheme(item.icon));
                action->setData(item.mime);

                actions.append(action);
            }
            QMenu::exec(actions, world, actions[0], this);
        }

        qDeleteAll(actions);
    }
}

void NavigationDockWidgetArea::doubleClicked(const QModelIndex &index)
{
    QString mime = m_pModel->data(index, NavigationModel::RoleMime).toString();
    itemClicked(mime);

}
// --------------------------------------------------------------------------

NavigationDockWidget::NavigationDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pClientArea(nullptr)
{
    m_pClientArea = new NavigationDockWidgetArea;
    setWindowTitle(tr("Навигация"));
    setWindowIcon(QIcon::fromTheme("NavigateMenu"));
    setWidget(m_pClientArea);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    connect(m_pClientArea, SIGNAL(itemClicked(QString)), this, SIGNAL(itemClicked(QString)));
}

NavigationDockWidget::~NavigationDockWidget()
{
    if (m_pClientArea)
        delete m_pClientArea;
}
