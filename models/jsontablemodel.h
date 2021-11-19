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
    typedef QVector<JsonTableColumn*> JsonTableColumnVector;

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

    bool open(const QString &filename, bool logopenmsg = true);

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
    int columnNumber(const QString &column) const;

    QVariant getValue(const QString &key, const QVariant &keyValue, const QString &valueFld, const QVariant &defaultValue = QVariant()) const;
    QModelIndex getValueIndex(const QString &key, const QVariant &keyValue) const;

    bool hasChanges() const;

    qint16 columnAutoinc() const;
    quint32 nextAutoinc() const;

    const QString &metaTitle() const;

protected:
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

public slots:
    void revertChanges();
    void applyChanges();
    bool save();

signals:
    void editStateChanged();

private:
    void readElements(const QJsonArray &arr);
    void readMetaData(const QJsonValue &obj);
    JsonTableColumnVector m_Columns;
    QScopedPointer<JsonTableModelData> m_Data;

    QScopedPointer<QJsonValue> m_JsonColumns, m_MetaData;
    QString m_Title;
};

#endif // JSONTABLEMODEL_H
