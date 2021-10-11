#ifndef JSONTABLEMODEL_H
#define JSONTABLEMODEL_H

#include <QAbstractItemModel>
#include <optional>

class JsonTableColumn;
class JsonTableModelData;
class JsonTableModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    typedef std::optional<quint16> ColumnWidthType;

    enum ColumnType
    {
        ColumnInvalid = 0,
        ColumnAutoInc,
        ColumnInteger,
        ColumnString,
        ColumnReal
    };
    JsonTableModel(QObject *parent = nullptr);
    virtual ~JsonTableModel();

    bool open(const QString &filename);

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    ColumnWidthType columnWidth(const quint16 &column) const;
    ColumnType columnType(const quint16 &column) const;
    QSizeF columnMinMax(const quint16 &column) const;

private:
    void readElements(const QJsonArray &arr);
    QVector<JsonTableColumn*> m_Columns;
    QScopedPointer<JsonTableModelData> m_Data;
};

#endif // JSONTABLEMODEL_H
