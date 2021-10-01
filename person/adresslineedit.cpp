#include "adresslineedit.h"
#include "globals.h"
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>

AdressLineEdit::AdressLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    m_pCompleter.reset(new QCompleter());
    m_pCompleter->setWidget(this);

    m_pCompleter->setModel(Globals::inst()->streetsModel());
    m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_pCompleter->setCompletionMode(QCompleter::PopupCompletion);
    m_pCompleter->setFilterMode(Qt::MatchContains);
    m_pCompleter->setWrapAround(true);

    connect(m_pCompleter.data(), SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

AdressLineEdit::~AdressLineEdit()
{

}

void AdressLineEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);

    if (!m_pCompleter)
        return;

    m_pCompleter->setCompletionPrefix(cursorWord(this->text()));
    if (m_pCompleter->completionPrefix().length() < 1)
    {
        m_pCompleter->popup()->hide();
        return;
    }
    /*QRect cr = cursorRect();
    cr.setWidth(m_pCompleter->popup()->sizeHintForColumn(0) + m_pCompleter->popup()->verticalScrollBar()->sizeHint().width());*/

    QRect cr = cursorRect();
    cr.setX(0);
    cr.setWidth(geometry().width());

    m_pCompleter->complete(cr);
}

QString AdressLineEdit::cursorWord(const QString &sentence) const
{
    return sentence.mid(sentence.leftRef(cursorPosition()).lastIndexOf(" ") + 1,
                        cursorPosition() -
                        sentence.leftRef(cursorPosition()).lastIndexOf(" ") - 1);
}

void AdressLineEdit::insertCompletion(QString arg)
{
    setText(text().replace(text().left(cursorPosition()).lastIndexOf(" ") + 1,
                           cursorPosition() -
                           text().left(cursorPosition()).lastIndexOf(" ") - 1,
                           arg));
}
