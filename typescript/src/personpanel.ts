class PersonPanel {
    private _person: DbTable;

    constructor (person : DbTable) {
        this._person = person;
        console.log('constructor ' + this._person.name());
    }

    public checkSave() : boolean {
        console.log(this._person.name());
        console.log('checkSave');
        return true;
    }
}