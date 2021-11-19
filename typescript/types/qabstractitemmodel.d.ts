interface QModelIndex{}

interface QAbstractItemModel {
    constructor();

    hasIndex(row : number, column : number, parent ?: QModelIndex) : boolean;
    index(row : number, column : number, parent ?: QModelIndex) : QModelIndex;
    parent(child ?: QModelIndex) : QModelIndex;

    sibling(row : number, column : number, idx ?: QModelIndex) : QModelIndex;

    rowCount(parent ?: QModelIndex) : number;
    columnCount(parent ?: QModelIndex) : number;
    hasChildren(parent ?: QModelIndex) : boolean;

    data(index : QModelIndex, role : number) : any;
    setData(index : QModelIndex, value : any, role : number);
    headerData(section : number, orientation : number, role : number) : any;

    fetchMore(parent ?: QModelIndex) : boolean;
    canFetchMore(parent ?: QModelIndex) : boolean;
    match(start : QModelIndex, role : number, value: any, hits : number = -1) : Array<QModelIndex>;
}