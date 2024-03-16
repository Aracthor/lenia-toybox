const defaultParameters = {
    "width": 800,
    "height": 600,
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
        if (!param) {
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