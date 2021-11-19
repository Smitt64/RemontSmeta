
enum Type
{
    TypeValue = 0,
    TypeGroup,
    TypeComboBox,
    TypeModel
}

enum Shape
{
    ShapeSquare = 0,
    ShapePolygon
}

interface RenovationQuestionModelElement {
    title : string;
    column : string;

    setCurrentTitle(id : number) : void;
    isGroup() : boolean;
    type() : Type;
    getComboValue(value : number) : any;
    comboModel() : QAbstractItemModel;
    isEditable() : boolean;
}

interface RenovationQuestionModel extends QAbstractItemModel {
    element(id : number | string) : RenovationQuestionModelElement;
    table() : DbTable;
}

class RenovationQuestionModelHelper {
    private _model: RenovationQuestionModel;
    private _room : DbTable;

    private static onShapeChangeTitles : string[] = [
        "t_length", "t_width", "t_height"
    ];

    private static onSizeChangeTitles : string[] = [
        "t_length", "t_width", "t_height",
        "t_door_width", "t_door_height", "t_door_depth",
        "t_window_width", "t_window_height", "t_window_depth"
    ];

    constructor (model : RenovationQuestionModel) {
        this._model = model;
        this._room = model.table();
    }

    public afterEditRoleSet(row : number, column : number, value : any) {
        var element : RenovationQuestionModelElement = this._model.element(row);

        if (element.column === "t_shape") {
            for (let col of RenovationQuestionModelHelper.onShapeChangeTitles) {
                this._model.element(col).setCurrentTitle(value);
            }
        }

        if (RenovationQuestionModelHelper.onSizeChangeTitles.indexOf(element.column) > -1) {
            var t_shape : Shape = <Shape>this._room.value("t_shape");
            var t_length : number = this._room.value("t_length");
            var t_width : number = this._room.value("t_width");
            var t_height : number = this._room.value("t_height");
            var t_door_width : number = this._room.value("t_door_width");
            var t_door_height : number = this._room.value("t_door_height");
            var t_window_width : number = this._room.value("t_window_width");
            var t_window_height : number = this._room.value("t_window_height");

            var t_wall_area_bopens : number = 0.0;
            if (t_shape === Shape.ShapeSquare) {
                t_wall_area_bopens = (t_length + t_width) * 2 * t_height;
                var t_floor : number = t_length * t_width;

                this._room.setValue("t_floor_area", t_floor); // Площадь пола
                this._room.setValue("t_celing_area", t_floor); // Площадь потолка
                this._room.setValue("t_wall_area_bopens", t_wall_area_bopens); // Площадь стен до вычета проемов
                this._room.setValue("t_floor_perimeter", t_floor); // Периметр пола
            }
            else {
                t_wall_area_bopens = t_width;
                this._room.setValue("t_floor_area", t_length); // Площадь пола
                this._room.setValue("t_celing_area", t_length); // Площадь потолка
                this._room.setValue("t_wall_area_bopens", t_wall_area_bopens); // Площадь стен до вычета проемов
                this._room.setValue("t_floor_perimeter", t_height); // Периметр пола
            }

            var t_door_area : number = t_door_width * t_door_height;
            var t_window_area : number = t_window_width * t_window_height;

            this._room.setValue("t_door_area", t_door_area);
            this._room.setValue("t_window_area", t_window_area);

            var t_window_slopes : number = t_window_width + t_window_height * 2;
            var t_wall_area_aopens : number = t_wall_area_bopens - t_door_area - t_window_area;

            if (t_wall_area_aopens < 0)
                t_wall_area_aopens = 0;

            this._room.setValue("t_wall_area_aopens", t_wall_area_aopens);
            this._room.setValue("t_window_slopes", t_window_slopes);
        }
    }
}