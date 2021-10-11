#include "jsontablemodel.h"
#include "loggingcategories.h"
#include "exceptionbase.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <limits>
#include <QSizeF>
#include <QDebug>

#define ERR_NO_TAG(name) QObject::tr("Не верное описание документа. Отсутствует элемент %1").arg(name)

class JsonTableColumn
{
public:
    JsonTableColumn()
    {
        _editable = true;
        _min = std::numeric_limits<decltype (_min)>::min();
        _max = std::numeric_limits<decltype (_max)>::max();
    }

    JsonTableColumn(JsonTableColumn &other) :
        _name(other._name),
        _title(other._title),
        _type(other._type),
        _editable(other._editable),
        _default(other._editable),
        _width(other._width),
        _min(other._min),
        _max(other._max)
    {

    }

    JsonTableColumn(JsonTableColumn &&other) :
        _name(std::move(other._name)),
        _title(std::move(other._title)),
        _type(std::move(other._type)),
        _editable(std::exchange(other._editable, false)),
        _default(std::move(other._editable)),
        _width(std::move(other._width)),
        _min(std::exchange(other._min, 0)),
        _max(std::exchange(other._max, 0))
    {

    }

    const QString &title() const
    {
        if (!_title.isEmpty())
            return _title;
        return _name;
    }

    const QString &name() const
    {
        return _name;
    }

    const JsonTableModel::ColumnType &type() const
    {
        return _type;
    }

    const bool &editable() const
    {
        return _editable;
    }

    const auto &min() const
    {
        return _min;
    }

    const auto &max() const
    {
        return _max;
    }

    JsonTableModel::ColumnWidthType width() const
    {
        return _width;
    }

    static JsonTableModel::ColumnType typeFromString(const QString &type)
    {
        JsonTableModel::ColumnType t = JsonTableModel::ColumnInvalid;

        if (type == "autoinc")
            t = JsonTableModel::ColumnAutoInc;
        else if (type == "integer")
            t = JsonTableModel::ColumnInteger;
        else if (type == "string")
            t = JsonTableModel::ColumnString;
        else if (type == "real")
            t = JsonTableModel::ColumnReal;

        return t;
    }

    bool isNumber() const
    {
        bool hr = false;

        if (_type == JsonTableModel::ColumnAutoInc ||
                _type == JsonTableModel::ColumnInteger ||
                _type == JsonTableModel::ColumnReal)
        {
            hr = true;
        }

        return hr;
    }

    QVariant valueFromJsonObject(const QJsonObject &obj) const
    {
        QVariant result;

        if (obj.contains(_name))
        {
            QJsonValue jsvalue = obj[_name];
            if (_type == JsonTableModel::ColumnAutoInc || _type == JsonTableModel::ColumnInteger)
            {
                if (jsvalue.isString())
                    result = jsvalue.toString().toInt();
                else
                    result = jsvalue.toInt();
            }
            else if (_type == JsonTableModel::ColumnString)
                result = jsvalue.toString();
            else if (_type == JsonTableModel::ColumnReal)
                result = jsvalue.toDouble();
        }
        else
            result = _default;

        return result;
    }

    static JsonTableColumn fromJsonObject(const QJsonObject &obj)
    {
        JsonTableColumn column;
        if (!obj.contains("name"))
            throw ExceptionBase(ERR_NO_TAG("name"));
        else
            column._name = obj["name"].toString();

        if (!obj.contains("type"))
            throw ExceptionBase(ERR_NO_TAG("type"));
        else
        {
            column._type = typeFromString(obj["type"].toString());

            if (column._type == JsonTableModel::ColumnAutoInc || column._type == JsonTableModel::ColumnInteger)
                column._default = 0;
            else if (column._type == JsonTableModel::ColumnString)
                column._default = QString();
            else if (column._type == JsonTableModel::ColumnReal)
                column._default.setValue<qreal>(0.0);

            if (column._type == JsonTableModel::ColumnAutoInc || column._type == JsonTableModel::ColumnString)
                column._min = 0;
            else
            {
                if (obj.contains("min"))
                    column._min = obj["min"].toVariant().value<decltype (_min)>();

                if (obj.contains("max"))
                    column._max = obj["max"].toVariant().value<decltype (_max)>();
            }

            if (obj.contains("editable"))
                column._editable = obj["editable"].toBool();

            if (obj.contains("title"))
                column._title = obj["title"].toString();

            if (obj.contains("width"))
            {
                if (obj["width"].isString())
                    column._width = obj["width"].toString().toInt();
                else
                    column._width = obj["width"].toInt();
            }
        }

        return column;
    }

private:
    QString _name, _title;
    JsonTableModel::ColumnType _type;
    bool _editable;
    QVariant _default;
    JsonTableModel::ColumnWidthType _width;
    qreal _min, _max;
};

// ------------------------------------------------

class JsonTableModelData
{
    friend class JsonTableModel;
public:
    typedef QMap<QString,QVariant> JsonDataListElement;
    typedef QList<JsonDataListElement> JsonDataList;
    typedef QList<JsonDataList> JsonDataStorage;

    JsonTableModelData() :
        m_isChanged(false)
    {
    }

    void append(const JsonDataListElement &value)
    {
        if (!m_isChanged)
            push();

        m_isChanged = true;
        top().append(value);
    }

    QVariant get(const quint32 &row, const QString &fld)
    {
        if (row >= size())
            return QVariant();

        const JsonDataListElement &element = top().at(row);
        if (!element.contains(fld))
            return QVariant();

        return element[fld];
    }

    void set(const quint32 &row, const QString &fld, const QVariant &val)
    {
        if (row >= size())
            return;

        if (!m_isChanged)
            push();

        JsonDataListElement &element = top()[row];

        element[fld] = val;
        m_isChanged = true;
    }

    const bool &isChanged() const
    {
        return m_isChanged;
    }

    quint32 size()
    {
        if (m_Data.isEmpty())
            return 0;

        return top().size();
    }

private:
    void resetChangedFlag()
    {
        m_isChanged = false;
    }

    JsonDataList &top()
    {
        return m_Data.last();
    }

    void push()
    {
        if (m_Data.size())
            m_Data.append(JsonDataList(m_Data.first()));
        else
            m_Data.append(JsonDataList());
    }

    bool m_isChanged;
    JsonDataStorage m_Data;
};

// ------------------------------------------------

JsonTableModel::JsonTableModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_Data.reset(new JsonTableModelData());
}

JsonTableModel::~JsonTableModel()
{
    qDeleteAll(m_Columns);
}

bool JsonTableModel::open(const QString &filename)
{
    bool hr = true;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
        qInfo(logCore()) << QString("JsonTableModel::open(%1)").arg(filename) << (hr = false);
    else
    {
        qInfo(logCore()) << QString("JsonTableModel::open(%1)").arg(filename) << true;

        QJsonParseError parseResult;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseResult);

        if (parseResult.error != QJsonParseError::NoError)
        {
            qInfo(logCore()) << "Parse error:" << parseResult.errorString();
            hr = false;
        }
        else
        {
            QJsonObject root = doc.object();

            if (!root.contains("columns"))
            {
                qInfo(logCore()) << "Parse error:" << ERR_NO_TAG("columns");
                hr = false;
            }
            else
            {
                try {
                    QJsonArray arr = root["columns"].toArray();

                    for (auto column : arr)
                    {
                        JsonTableColumn tmp = JsonTableColumn::fromJsonObject(column.toObject());

                        JsonTableColumn *col = new JsonTableColumn(tmp);
                        m_Columns.append(col);
                    }
                }
                catch (ExceptionBase &e)
                {
                    qInfo(logCore()) << "Parse error:" << e.what();
                    hr = false;
                }

                if (root.contains("elements"))
                {
                    QJsonArray arr = root["elements"].toArray();
                    readElements(arr);
                }

                m_Data->resetChangedFlag();
            }
        }
    }

    return hr;
}

void JsonTableModel::readElements(const QJsonArray &arr)
{
    for (auto column : arr)
    {
        QJsonObject fldelement = column.toObject();
        JsonTableModelData::JsonDataListElement element;

        for (const JsonTableColumn * const fld : m_Columns)
        {
            QString name = fld->name();
            element[name] = fld->valueFromJsonObject(fldelement);
        }

        m_Data->append(element);
    }
}

QVariant JsonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_Columns[section]->title();

    return QAbstractItemModel::headerData(section, orientation, role);
}

int JsonTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Columns.size();
}

QModelIndex JsonTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex JsonTableModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int JsonTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Data->size();
}

Qt::ItemFlags JsonTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;

    Q_ASSERT_X(index.column() >= 0 && index.column() < m_Columns.size(), "JsonTableModel::flags", "invalid column index");

    const JsonTableColumn * const column = m_Columns[index.column()];
    Qt::ItemFlags f = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled;
    if (column->editable())
        f |= Qt::ItemIsEditable;

    return f;
}

JsonTableModel::ColumnWidthType JsonTableModel::columnWidth(const quint16 &column) const
{
    Q_ASSERT_X(column >= 0 && column < m_Columns.size(), "JsonTableModel::columnWidth", "invalid column index");
    return m_Columns[column]->width();
}

JsonTableModel::ColumnType JsonTableModel::columnType(const quint16 &column) const
{
    Q_ASSERT_X(column >= 0 && column < m_Columns.size(), "JsonTableModel::columnType", "invalid column index");
    return m_Columns[column]->type();
}

QSizeF JsonTableModel::columnMinMax(const quint16 &column) const
{
    Q_ASSERT_X(column >= 0 && column < m_Columns.size(), "JsonTableModel::columnMinMax", "invalid column index");
    return QSize(m_Columns[column]->min(), m_Columns[column]->max());
}

QVariant JsonTableModel::data(const QModelIndex &index, int role) const
{
    const JsonTableColumn * const column = m_Columns[index.column()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return m_Data->get(index.row(), column->name());
    else if (role == Qt::TextAlignmentRole)
    {
        if (column->isNumber())
            return Qt::AlignRight;
    }

    return QVariant();
}

bool JsonTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_ASSERT_X(index.column() >= 0 && index.column() < m_Columns.size(), "JsonTableModel::setData", "invalid column index");

    const JsonTableColumn * const column = m_Columns[index.column()];
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        QVariant oldValue = m_Data->get(index.row(), column->name());
        if (oldValue != value)
        {
            m_Data->set(index.row(), column->name(), value);
            return false;
        }
    }

    return false;
}
