function restart() {
    window.module._app_restart();
}

function set_config(param_name) {
    const get_input_value = (elem_name) => {
        let element = document.getElementById(elem_name);
        if (element.type == "checkbox") {
            return element.checked;
        }
        return element.value;
    };

    const is_string = (value) => {
        return value != false && value != true && isNaN(parseFloat(value));
    };

    let value = get_input_value(param_name);
    if (is_string(value)) {
        value = window.module.stringToNewUTF8(value);
    }
    let function_name = "_app_config_" + param_name;
    window.module[function_name](value);
}
