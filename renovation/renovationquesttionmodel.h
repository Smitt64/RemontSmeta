#ifndef RENOVATIONQUESTTIONMODEL_H
#define RENOVATIONQUESTTIONMODEL_H

#include <QAbstractTableModel>
#include "db/dbtable.h"

class QStandardItemModel;
class QJsonObject;
class RenovationQuestionModelElementModel;
class RenovationQuestionModelElement : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString column READ column WRITE setTitle)
public:
    enum Type
    {
        TypeValue = 0,
        TypeGroup,
        TypeComboBox,
        TypeModel
    };
    Q_ENUM(Type);
    RenovationQuestionModelElement(QObject *parent = nullptr);
    virtual ~RenovationQuestionModelElement();

    const QString &title() const;
    void setTitle(const QString &value);

    const QString &column() const;
    void setColumn(const QString &value);

    Q_INVOKABLE void setCurrentTitle(const quint8 &id);
    Q_INVOKABLE bool isGroup() const;

    Q_INVOKABLE const Type &type() const;
    Q_INVOKABLE QVariant getComboValue(const qint32 &value) const;
    Q_INVOKABLE QAbstractItemModel *comboModel() const;
    Q_INVOKABLE const bool &isEditable() const;

    Q_INVOKABLE const QString &jsonModelName() const;
    Q_INVOKABLE const QString &jsonModelDisplay() const;
    Q_INVOKABLE const QString &jsonModelKey() const;

    static void fromJson(RenovationQuestionModelElement **element, const QJsonObject &obj);

signals:
    void titleChanged();

private:
    QStringList m_Title;
    QString m_Column;
    quint8 m_CurrentTitle;
    Type m_Type;
    bool m_IsEditable;

    QString m_JsonModelName, m_JsonModelDisplay, m_JsonModelKey;

    QScopedPointer<RenovationQuestionModelElementModel> m_Items;
};

// ------------------------------------------------

class TRoom;
class DbTable;
class JsClassWrapper;
class JsClassExecuter;
class RenovationQuestionModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Field
    {
        FldName = 0,
        FldValue
    };
    RenovationQuestionModel(TRoom *table, QObject *parent = nullptr);
    virtual ~RenovationQuestionModel();

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    Q_INVOKABLE RenovationQuestionModelElement *element(const quint16 &id) const;
    Q_INVOKABLE RenovationQuestionModelElement *element(const QString &id) const;

    Q_INVOKABLE DbTable *table();
    DbFieldBase *field(const QModelIndex &index) const;

private slots:
    void onTablePositionChanged();
    void onTitleChanged();

private:
    void loadFromJson();

    QScopedPointer<JsClassExecuter> m_JsExecuter;
    QSharedPointer<JsClassWrapper> m_JsClass;

    QVector<RenovationQuestionModelElement*> m_Elements;
    QHash<QString,RenovationQuestionModelElement*> m_ColumnElements;
    QStringList m_ColumnsNames;

    TRoom *m_pRoom;
};

#endif // RENOVATIONQUESTTIONMODEL_H
