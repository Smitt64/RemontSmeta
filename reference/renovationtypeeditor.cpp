#include "renovationtypeeditor.h"

RenovationTypeEditor::RenovationTypeEditor(QWidget *parent) :
    JsonTableModelEditor(parent)
{

}

RenovationTypeEditor::~RenovationTypeEditor()
{

}

void RenovationTypeEditor::onInitModel()
{
    setJsonFile("renovation_type.json");
}
