#ifndef RENOVATIONTYPEEDITOR_H
#define RENOVATIONTYPEEDITOR_H

#include "reference/jsonmodeleditor.h"

class RenovationTypeEditor : public JsonTableModelEditor
{
    Q_OBJECT
public:
    RenovationTypeEditor(QWidget *parent);
    virtual ~RenovationTypeEditor();

protected:
    virtual void onInitModel() Q_DECL_OVERRIDE;
};

#endif // RENOVATIONTYPEEDITOR_H
