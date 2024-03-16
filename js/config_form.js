function restart() {
    window.module._app_restart();
}
window.restart = restart;

function set_custom_preset() {
    document.getElementById("preset").value = "(custom)";
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

function input_config(param_name) {
    set_config(param_name);
}
window.input_config = input_config;

function input_algorithm_config(param_name) {
    set_custom_preset();
    set_config(param_name);
}
window.input_algorithm_config = input_algorithm_config;

function set_algorithm() {
    const algorithms_params = {
        "larger-than-life": ["survival_range_field", "birth_range_field"],
        "primordia":        ["timestamp_field", "survival_range_field", "birth_range_field"],
        "lenia":            ["timestamp_field", "kernel_gauss_field", "growth_gauss_field"],
    };

    let algorithm = document.getElementById("algorithm").value;
    let params_to_show = algorithms_params[algorithm];
    let params = ["timestamp_field", "survival_range_field", "birth_range_field", "kernel_gauss_field", "growth_gauss_field"];
    params.forEach((param) => {
        document.getElementById(param).hidden = !params_to_show.includes(param);
    });
    set_config("algorithm");
}

function input_algorithm() {
    set_custom_preset();
    set_algorithm();
}
window.input_algorithm = input_algorithm;

function set_preset() {
    const preset_params = {
        "conway-life": {
            algorithm: "larger-than-life",
            params: {
                "range": 1,
                "survival_range_min": 2,
                "survival_range_max": 3,
                "birth_range_min": 3,
                "birth_range_max": 3,
            },
        },
        "bosco-rule": {
            algorithm: "larger-than-life",
            params: {
                "range": 5,
                "survival_range_min": 33,
                "survival_range_max": 57,
                "birth_range_min": 34,
                "birth_range_max": 45,
            },
        },
        "primordia": {
            algorithm: "primordia",
            params: {
                "range": 1,
                "timestamp": 10,
                "survival_range_min": 0.19,
                "survival_range_max": 0.33,
                "birth_range_min": 0.20,
                "birth_range_max": 0.25,
            },
        },
        "primordia-proto-lenia": {
            algorithm: "primordia",
            params: {
                "range": 5,
                "timestamp": 10,
                "survival_range_min": 0.12,
                "survival_range_max": 0.15,
                "birth_range_min": 0.12,
                "birth_range_max": 0.15,
            },
        },
        "lenia": {
            algorithm: "lenia",
            params: {
                "range": 15,
                "timestamp": 10,
                "kernel_center": 0.5,
                "kernel_width": 0.15,
                "growth_center": 0.135,
                "growth_width": 0.014,
            },
        },
    };

    let preset_name = document.getElementById("preset").value;
    let preset = preset_params[preset_name];
    document.getElementById("algorithm").value = preset.algorithm;
    set_algorithm(preset.algorithm);
    for (const param in preset.params) {
        let value = preset.params[param];
        document.getElementById(param).value = value;
        set_config(param);
    }
}
window.set_preset = set_preset;
