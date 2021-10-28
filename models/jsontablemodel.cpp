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

template<class T>
struct is_integer_type : std::integral_constant<bool,
        std::is_integral<T>::value> {};

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

    template<class T>
    bool isSameType() const
    {
        bool result = false;

        if constexpr (std::is_floating_point<T>::value)
        {
            if (_type == JsonTableModel::ColumnReal)
                result = true;
        }
        else if constexpr (is_integer_type<T>::value)
        {
            if (_type == JsonTableModel::ColumnAutoInc || _type == JsonTableModel::ColumnInteger)
                result = true;
        }
        else if constexpr (std::is_same<T, QString>::value)
        {
            if (_type == JsonTableModel::ColumnString)
                result = true;
        }

        return result;
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
    typedef QVector<JsonDataListElement> JsonDataList;
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

    void insert(int row)
    {
        if (!m_isChanged)
            push();

        JsonDataListElement value;
        top().insert(row, value);
        m_isChanged = true;
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

    void revert()
    {
        if (m_isChanged && !m_Data.isEmpty())
        {
            m_isChanged = false;
            m_Data.takeLast();
        }
    }

    void apply()
    {
        if (m_isChanged && !m_Data.isEmpty())
        {
            m_isChanged = false;
            m_Data.takeFirst();
        }
    }

    template<class T>
    T maxValue(const JsonTableColumn &column)
    {
        if (!column.isSameType<T>())
            return 0;

        int sz = top().size();
        T max = column.min();
        for (int row = 0; row < sz; row++)
        {
            T curVal = get(row, column.name()).value<T>();
            max = std::max(max, curVal);
        }

        return max;
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

    const JsonDataList &top() const
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

bool JsonTableModel::open(const QString &filename, bool logopenmsg)
{
    bool hr = true;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
    {
        if (logopenmsg)
            qInfo(logCore()) << QString("JsonTableModel::open(%1)").arg(filename) << (hr = false);
    }
    else
    {
        if (logopenmsg)
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
                    m_JsonColumns.reset(new QJsonValue(root["columns"]));
                    QJsonArray arr = m_JsonColumns->toArray();

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

                if (root.contains("metadata"))
                    readMetaData(root["metadata"].toObject());

                m_Data->resetChangedFlag();
            }
        }

        file.close();
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

void JsonTableModel::readMetaData(const QJsonValue &obj)
{
    m_MetaData.reset(new QJsonValue(obj));

    QJsonObject _obj = m_MetaData->toObject();
    m_Title = _obj.value("title").toString();
}

const QString &JsonTableModel::metaTitle() const
{
    return m_Title;
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

    if (role == Qt::EditRole)
    {
        return m_Data->get(index.row(), column->name());
    }
    else if (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::StatusTipRole)
    {
        QVariant dt = m_Data->get(index.row(), column->name());

        if (column->type() == JsonTableModel::ColumnReal)
            return QString::number(dt.toDouble(), 'f', 2);

        return dt;

    }
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
            emit editStateChanged();
            return true;
        }
    }

    return false;
}

bool JsonTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;

    qint16 fld = columnAutoinc();
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
    {
        m_Data->insert(row);

        if (fld != -1)
        {
            quint32 nextautoinc = nextAutoinc();
            m_Data->set(row + r, m_Columns[fld]->name(), nextautoinc);
        }
    }
    endInsertRows();
    return true;
}

bool JsonTableModel::hasChanges() const
{
    return m_Data->isChanged();
}

void JsonTableModel::revertChanges()
{
    beginResetModel();
    m_Data->revert();
    endResetModel();
    emit editStateChanged();
}

void JsonTableModel::applyChanges()
{
    beginResetModel();
    m_Data->apply();
    endResetModel();
    emit editStateChanged();
}

qint16 JsonTableModel::columnAutoinc() const
{
    qint16 fld = -1;

    for (const auto &item : m_Columns)
    {
        if (item->type() == JsonTableModel::ColumnAutoInc)
        {
            fld = m_Columns.indexOf(item);
            break;
        }
    }

    return fld;
}

quint32 JsonTableModel::nextAutoinc() const
{
    qint16 fld = columnAutoinc();

    if (fld == -1)
        return 0;

    quint32 maxvalue = m_Data->maxValue<quint32>(*m_Columns[fld]);

    return maxvalue + 1;
}

bool JsonTableModel::save()
{
    bool hr = false;

    QJsonDocument doc;
    QJsonObject object;

    if (m_MetaData)
        object.insert("metadata", *m_MetaData);

    if (m_JsonColumns)
        object.insert("columns", *m_JsonColumns);

    doc.setObject(object);

    //qDebug() << doc.toJson();
    QFile f("111.json");
    f.open(QIODevice::WriteOnly);
    f.write(doc.toJson());

    return hr;
}
