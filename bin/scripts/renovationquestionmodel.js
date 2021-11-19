"use strict";
var Type;
(function (Type) {
    Type[Type["TypeValue"] = 0] = "TypeValue";
    Type[Type["TypeGroup"] = 1] = "TypeGroup";
    Type[Type["TypeComboBox"] = 2] = "TypeComboBox";
    Type[Type["TypeModel"] = 3] = "TypeModel";
})(Type || (Type = {}));
var Shape;
(function (Shape) {
    Shape[Shape["ShapeSquare"] = 0] = "ShapeSquare";
    Shape[Shape["ShapePolygon"] = 1] = "ShapePolygon";
})(Shape || (Shape = {}));
var RenovationQuestionModelHelper = /** @class */ (function () {
    function RenovationQuestionModelHelper(model) {
        this._model = model;
        this._room = model.table();
    }
    RenovationQuestionModelHelper.prototype.afterEditRoleSet = function (row, column, value) {
        var element = this._model.element(row);
        if (element.column === "t_shape") {
            for (var _i = 0, _a = RenovationQuestionModelHelper.onShapeChangeTitles; _i < _a.length; _i++) {
                var col = _a[_i];
                this._model.element(col).setCurrentTitle(value);
            }
        }
        if (RenovationQuestionModelHelper.onSizeChangeTitles.indexOf(element.column) > -1) {
            var t_shape = this._room.value("t_shape");
            var t_length = this._room.value("t_length");
            var t_width = this._room.value("t_width");
            var t_height = this._room.value("t_height");
            var t_door_width = this._room.value("t_door_width");
            var t_door_height = this._room.value("t_door_height");
            var t_window_width = this._room.value("t_window_width");
            var t_window_height = this._room.value("t_window_height");
            var t_wall_area_bopens = 0.0;
            if (t_shape === Shape.ShapeSquare) {
                t_wall_area_bopens = (t_length + t_width) * 2 * t_height;
                var t_floor = t_length * t_width;
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
            var t_door_area = t_door_width * t_door_height;
            var t_window_area = t_window_width * t_window_height;
            this._room.setValue("t_door_area", t_door_area);
            this._room.setValue("t_window_area", t_window_area);
            var t_window_slopes = t_window_width + t_window_height * 2;
            var t_wall_area_aopens = t_wall_area_bopens - t_door_area - t_window_area;
            if (t_wall_area_aopens < 0)
                t_wall_area_aopens = 0;
            this._room.setValue("t_wall_area_aopens", t_wall_area_aopens);
            this._room.setValue("t_window_slopes", t_window_slopes);
        }
    };
    RenovationQuestionModelHelper.onShapeChangeTitles = [
        "t_length", "t_width", "t_height"
    ];
    RenovationQuestionModelHelper.onSizeChangeTitles = [
        "t_length", "t_width", "t_height",
        "t_door_width", "t_door_height", "t_door_depth",
        "t_window_width", "t_window_height", "t_window_depth"
    ];
    return RenovationQuestionModelHelper;
}());
