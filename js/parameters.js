const defaultParameters = {
    "width": 200,
    "height": 150,
    "framerate": 60,
    "startup": "",
    "pause": false,

    "algorithm": "lenia",
    "range": 15,
    "timestamp": 10,
    "survival_range_min": 2,
    "survival_range_max": 3,
    "birth_range_min": 3,
    "birth_range_max": 3,
    "kernel_center": 0.5,
    "kernel_width": 0.15,
    "kernel_1_growth_center": 0.135,
    "kernel_1_growth_width": 0.014,
    "kernel_1_ring_1_weight": 1.0,
    "kernel_1_ring_2_weight": 1.0,
    "kernel_1_ring_3_weight": 1.0,
};

const Parameters = class {
    constructor() {
        this.searchParams = new URLSearchParams(window.location.search)
        this.id = 0;
    }

    keys() {
        return Object.keys(defaultParameters);
    }

    get(name) {
        let param = this.searchParams.get(name);
        if (param === null) {
            param = defaultParameters[name];
        }
        return param.toString();
    }

    set(name, value) {
        this.searchParams.set(name, value);
    }

    save_in_url() {
        let newUrl = location.pathname + "?" + this.searchParams.toString();
        window.history.pushState(this.id, "Restart", newUrl);
        this.id++;
    }
};

export default Parameters;