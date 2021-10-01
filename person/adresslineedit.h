#ifndef ADRESSLINEEDIT_H
#define ADRESSLINEEDIT_H

#include <QLineEdit>

class AdressLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    AdressLineEdit(QWidget *parent = nullptr);
    virtual ~AdressLineEdit();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void insertCompletion(QString);

private:
    QString cursorWord(const QString& sentence) const;

    QScopedPointer<QCompleter> m_pCompleter;
};

#endif // ADRESSLINEEDIT_H
