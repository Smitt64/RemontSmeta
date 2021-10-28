"use strict";
var PersonPanel = /** @class */ (function () {
    function PersonPanel(person) {
        this._person = person;
        console.log('constructor ' + this._person.name());
    }
    PersonPanel.prototype.checkSave = function () {
        console.log(this._person.name());
        console.log('checkSave');
        return true;
    };
    return PersonPanel;
}());
