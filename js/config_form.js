function restart() {
    window.module._app_restart();
}

function set_config(param_name) {

    let value = document.getElementById(param_name).value;
    if (isNaN(parseFloat(value))) {
        value = window.module.stringToNewUTF8(value);
    }
    let function_name = "_app_config_" + param_name;
    window.module[function_name](value);
}
