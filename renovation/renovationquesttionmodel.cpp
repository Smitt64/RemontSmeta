#include "renovationquesttionmodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QApplication>
#include <QFile>
#include <QStyle>
#include "tables/renovation.h"
#include "jsclasses/jsclassexecuter.h"
#include "globals.h"
#include "exceptionbase.h"
#include "models/jsontablemodel.h"

#define SCRIPT_NAME "renovationquestionmodel.js"

class RenovationQuestionModelElementModel : public QAbstractTableModel
{
    typedef struct
    {
        qint32 _id;
        QString _name;
    }Element;
public:
    RenovationQuestionModelElementModel(QObject *parent = nullptr) :
        QAbstractTableModel(parent)
    {

    }

    virtual ~RenovationQuestionModelElementModel()
    {
        qDeleteAll(m_Items);
    }

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(parent);
        return 2;
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(parent);
        return m_Items.size();
    }

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE
    {
        Q_UNUSED(index);
        Q_UNUSED(value);
        Q_UNUSED(role);
        return false;
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        if (role == Qt::DisplayRole)
        {
            if (index.column() == 0)
                return m_Items[index.row()]->_id;
            else
                return m_Items[index.row()]->_name;
        }
        return QVariant();
    }

    void add(const qint32 &_id, const QString &name)
    {
        Element *element = new Element();
        element->_id = _id;
        element->_name = name;
        m_Items.append(element);
    }

private:
    QVector<Element*> m_Items;
};

// ------------------------------------------------

RenovationQuestionModelElement::RenovationQuestionModelElement(QObject *parent) :
    QObject(parent),
    m_CurrentTitle(0),
    m_Type(TypeValue),
    m_IsEditable(true)
{

}

RenovationQuestionModelElement::~RenovationQuestionModelElement()
{

}

const QString &RenovationQuestionModelElement::title() const
{
    return m_Title[m_CurrentTitle];
}

void RenovationQuestionModelElement::setTitle(const QString &value)
{
    m_Title[m_CurrentTitle] = value;
}

const QString &RenovationQuestionModelElement::column() const
{
    return m_Column;
}

void RenovationQuestionModelElement::setColumn(const QString &value)
{
    m_Column = value;
}

void RenovationQuestionModelElement::setCurrentTitle(const quint8 &id)
{
    m_CurrentTitle = id;
    emit titleChanged();
}

bool RenovationQuestionModelElement::isGroup() const
{
    return m_Type == TypeGroup;
}

const RenovationQuestionModelElement::Type &RenovationQuestionModelElement::type() const
{
    return m_Type;
}

void RenovationQuestionModelElement::fromJson(RenovationQuestionModelElement **element, const QJsonObject &obj)
{
    *element = new RenovationQuestionModelElement();

    (*element)->m_Column = obj["column"].toString();

    QJsonArray titles = obj["titles"].toArray();
    for (const auto &item : titles)
        (*element)->m_Title.append(item.toString());

    if (obj.contains("editable"))
        (*element)->m_IsEditable = obj["editable"].toBool();

    if (obj.contains("group"))
    {
        if (obj["group"].toBool())
        {
            (*element)->m_Type = TypeGroup;
            (*element)->m_IsEditable = false;
        }
    }

    if (obj.contains("items"))
    {
        (*element)->m_Items.reset(new RenovationQuestionModelElementModel());
        (*element)->m_Type = TypeComboBox;

        QJsonArray comboItems = obj["items"].toArray();
        for (const auto &item : comboItems)
        {
            QJsonObject comboItem = item.toObject();
            (*element)->m_Items->add(comboItem["id"].toInt(), comboItem["name"].toString());
        }
    }

    if (obj.contains("jsonmodel"))
    {
        (*element)->m_Type = TypeModel;

        QJsonObject jsonmodel = obj["jsonmodel"].toObject();
        (*element)->m_JsonModelName = jsonmodel["name"].toString();
        (*element)->m_JsonModelDisplay = jsonmodel["display"].toString();
        (*element)->m_JsonModelKey = jsonmodel["key"].toString();
    }
}

QVariant RenovationQuestionModelElement::getComboValue(const qint32 &value) const
{
    QModelIndexList lst = m_Items->match(m_Items->index(0, 0), Qt::DisplayRole, value, 1, Qt::MatchExactly);

    if (lst.isEmpty())
        return QVariant();

    return m_Items->data(m_Items->index(lst.first().row(), 1));
}

QAbstractItemModel *RenovationQuestionModelElement::comboModel() const
{
    return m_Items.data();
}

const bool &RenovationQuestionModelElement::isEditable() const
{
    return m_IsEditable;
}

const QString &RenovationQuestionModelElement::jsonModelName() const
{
    return m_JsonModelName;
}

const QString &RenovationQuestionModelElement::jsonModelDisplay() const
{
    return m_JsonModelDisplay;
}

const QString &RenovationQuestionModelElement::jsonModelKey() const
{
    return m_JsonModelKey;
}

// ----------------------------------------

RenovationQuestionModel::RenovationQuestionModel(TRoom *table, QObject *parent) :
    QAbstractTableModel(parent),
    m_pRoom(table)
{
    loadFromJson();

    QFileInfo fi = Globals::inst()->scriptFile(SCRIPT_NAME);

    m_JsExecuter.reset(new JsClassExecuter());
    if (m_JsExecuter->open(fi.absoluteFilePath()))
    {
        JsClassExecuter::setObjectOwner(this);
        JsClassExecuter::setObjectOwner(table);

        m_JsClass = m_JsExecuter->createClass("RenovationQuestionModelHelper", this);
    }
    else
    {
        throw ExceptionBase(tr("Ошибка выполнения сценария [%1]").arg(SCRIPT_NAME));
    }

    connect(m_pRoom, &DbTable::positionChanged, this, &RenovationQuestionModel::onTablePositionChanged);
    connect(m_pRoom, &DbTable::columnValueChanged, [=](const QString &column)
    {
        int idx = m_ColumnsNames.indexOf(column);

        if (idx != -1)
            emit dataChanged(index(idx, FldValue), index(idx, FldValue));
    });
}

RenovationQuestionModel::~RenovationQuestionModel()
{
    qDeleteAll(m_Elements);
}

int RenovationQuestionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

int RenovationQuestionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_Elements.size();
}

void RenovationQuestionModel::onTablePositionChanged()
{
    beginResetModel();
    endResetModel();
}

QVariant RenovationQuestionModel::data(const QModelIndex &index, int role) const
{
    RenovationQuestionModelElement *element = m_Elements[index.row()];
    if (role == Qt::DisplayRole)
    {
        if (index.column() == FldName)
            return element->title();
        else if (index.column() == FldValue)
        {
            if (!m_pRoom->isValidPosition())
                return QVariant();

            QVariant value = m_pRoom->value(element->column());
            if (element->type() == RenovationQuestionModelElement::TypeValue)
            {
                if (m_pRoom->isReal(element->column()))
                    return QLocale::system().toString(value.toDouble(), 'f', 2);
                else
                {
                    const DbFieldBase &fld = m_pRoom->field(element->column());

                    if (fld.type() == DbFieldBase::Boolean)
                    {
                        if (value.toBool())
                            return tr("да");
                        else
                            return tr("нет");
                    }
                    return value;
                }
            }
            else if (element->type() == RenovationQuestionModelElement::TypeComboBox)
            {
                QVariant val = element->getComboValue(value.toInt());
                return val;
            }
            else if (element->type() == RenovationQuestionModelElement::TypeModel)
            {
                JsonTableModel *model = Globals::inst()->model(element->jsonModelName());

                QVariant value = model->getValue(element->jsonModelKey(), m_pRoom->value(element->column()), element->jsonModelDisplay(), tr("нет"));
                return value;
            }
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        QPalette palette = qApp->palette();

        if (element->isEditable())
        {
            if (index.row() % 2 != 0)
                return palette.color(QPalette::ToolTipBase);
        }
        else
            return palette.color(QPalette::Window);
    }
    else if (role == Qt::EditRole)
    {
        if (index.column() == FldValue)
        {
            QVariant value = m_pRoom->value(element->column());
            if (element->type() == RenovationQuestionModelElement::TypeValue)
                return value;
            else if (element->type() == RenovationQuestionModelElement::TypeComboBox)
                return value;
            else if (element->type() == RenovationQuestionModelElement::TypeModel)
                return value;
        }
    }
    else if (role == Qt::SizeHintRole)
        return QSize(30, 25);

    return QVariant();
}

bool RenovationQuestionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() != FldValue || role != Qt::EditRole)
        return false;

    bool isChanged = false;
    RenovationQuestionModelElement *element = m_Elements[index.row()];
    if (element->type() == RenovationQuestionModelElement::TypeValue)
    {
        m_pRoom->setValue(element->column(), value);
        isChanged = true;
    }
    else if (element->type() == RenovationQuestionModelElement::TypeComboBox)
    {
        m_pRoom->setValue(element->column(), value);
        isChanged = true;
    }
    else if (element->type() == RenovationQuestionModelElement::TypeModel)
    {
        m_pRoom->setValue(element->column(), value);
        isChanged = true;
    }

    if (isChanged)
    {
        m_JsClass->call("afterEditRoleSet", index.row(), index.column(), value);
        return true;
    }

    return false;
}

Qt::ItemFlags RenovationQuestionModel::flags(const QModelIndex &index) const
{
    RenovationQuestionModelElement *element = m_Elements[index.row()];
    Qt::ItemFlags flgs = QAbstractTableModel::flags(index);

    if (index.column() == FldValue && m_pRoom->isValidPosition())
    {
        if (element->isEditable())
            flgs |= Qt::ItemIsEditable;
    }

    return flgs;
}

QVariant RenovationQuestionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (role != Qt::DisplayRole)
        return QAbstractTableModel::headerData(section, orientation, role);

    QVariant val;
    switch(section)
    {
    case FldName:
        val = tr("Вопрос");
        break;
    case FldValue:
        val = tr("Ответ");
        break;
    }

    return val;
}

void RenovationQuestionModel::loadFromJson()
{
    QFile file(":/tools/json/renovationquestionmodel.json");

    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    if (doc["elements"].isArray())
    {
        QJsonArray elements = doc["elements"].toArray();

        for (const auto &item : elements)
        {
            RenovationQuestionModelElement *element = nullptr;
            RenovationQuestionModelElement::fromJson(&element, item.toObject());
            m_Elements.append(element);
            m_ColumnElements.insert(element->column(), element);
            m_ColumnsNames.append(element->column());

            connect(element, &RenovationQuestionModelElement::titleChanged, this, &RenovationQuestionModel::onTitleChanged);
        }
    }
}

RenovationQuestionModelElement *RenovationQuestionModel::element(const quint16 &id) const
{
    return m_Elements[id];
}

RenovationQuestionModelElement *RenovationQuestionModel::element(const QString &id) const
{
    return m_ColumnElements[id];
}

DbTable *RenovationQuestionModel::table()
{
    return m_pRoom;
}

void RenovationQuestionModel::onTitleChanged()
{
    RenovationQuestionModelElement *element = qobject_cast<RenovationQuestionModelElement*>(sender());

    if (!element)
        return;

    int idx = m_Elements.indexOf(element);
    if (idx >= 0)
        emit dataChanged(index(idx, FldName), index(idx, FldName));
}

DbFieldBase *RenovationQuestionModel::field(const QModelIndex &index) const
{
    RenovationQuestionModelElement *element = m_Elements[index.row()];
    return const_cast<DbFieldBase*>(&m_pRoom->field(element->column()));
}
