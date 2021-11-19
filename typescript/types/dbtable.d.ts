interface DbTable extends QAbstractItemModel
{
    constructor(name : string);

    name() : string;
    find(index : number, keys : Array<any>) : boolean;
    selectAll() : boolean;
    fieldNum(name : string) : number;

    primaryIndex() : number;
    rowCount() : number;
    columnCount() : number;

    value(id : number | string) : any;
    setValue(id : number | string, val : any);

    next() : boolean;
    previous() : boolean;
    first() : boolean;
    last() : boolean;
    last(index : number) : boolean;

    newRec() : boolean;
    update() : boolean;
    insert() : boolean;
    delete_() : boolean;
    delete_(index : number, keys : Array<any>) : boolean;

    lastInsertRowId() : number;

    submit() : boolean;
    insertRows(row : number, count : number) : boolean;
}