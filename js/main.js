"use strict"

import Module from './bin/lenia-toybox.js'
import Parameters from './parameters.js'

const onGlobalPageLoad = async () => {

    const log = (text) => {
        let textArea = document.querySelector("#loggerOutput");
        textArea.value += text + '\n';
        // force focus on last line
        textArea.scrollTop = textArea.scrollHeight;
    };

    log("page loaded");

    const onBasicGlobalPageError = (event) => {
        log(`[ERROR] exception, event=${event.message}`);
    };
    window.addEventListener("error", onBasicGlobalPageError);

    const canvas = document.querySelector("#canvas");


    const onContextCreationError = (event) => {

        event.preventDefault();

        const statusMessage = event.statusMessage || "Unknown error";
        log(`[ERROR] could not create a WebGL context, statusMessage="${statusMessage}".`);
    };
    canvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);

    const onWebGlContextLost = (event) => {

        event.preventDefault();

        log("[ERROR] WebGL context lost. You will need to reload the page.");
    };
    canvas.addEventListener("webglcontextlost", onWebGlContextLost, false);

    // this prevent the contextual menu to appear on a right click
    const onContextMenu = (event) => {
        event.preventDefault();
    };
    canvas.addEventListener("contextmenu", onContextMenu, false);

    let webglCtx;

    try {
        const wasmSupported = (() => {
            try {
                if (typeof(WebAssembly) === "object" && typeof(WebAssembly.instantiate) === "function") {

                    const module = new WebAssembly.Module(Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00));

                    if (module instanceof WebAssembly.Module)
                        return new WebAssembly.Instance(module) instanceof WebAssembly.Instance;
                }
            } catch (err) {
                log("[ERROR] Cannot instanciate WebAssembly module.");
            }
            return false;
        })();

        if (!wasmSupported)
            throw new Error("missing WebAssembly feature (unsuported)");

        log("WebAssembly feature => supported");


        if (!window.WebGLRenderingContext)
            throw new Error("missing WebGL feature (unsuported)");

        log("WebGL feature => supported");

        const renderingContextAttribs = {
            alpha: false,
            antialias: false,
            depth: false,
            failIfMajorPerformanceCaveat: false,
            premultipliedAlpha: false,
            preserveDrawingBuffer: false,
            stencil: false,

            // A hint to the user agent indicating what configuration of GPU is
            // suitable for the WebGL context. Possible values are:
            // "default":
            //     Let the user agent decide which GPU configuration is most
            //     suitable. This is the default value.
            // "high-performance":
            //     Prioritizes rendering performance over power consumption.
            // "low-power":
            //     Prioritizes power saving over rendering performance.
            powerPreference: "high-performance",
        };

        webglCtx = canvas.getContext("webgl2", renderingContextAttribs);

        if (!webglCtx)
            throw new Error("WebGL context failed (initialisation)");

        log("WebGL context => initialised");
    }
    catch (err) {

        log(`[ERROR] dependencies check failed: message="${err.message}"`);
        return;
    }

    let parameters = new Parameters();

    let argv = [
        "-w", parameters.get("width"),
        "-h", parameters.get("height"),
        "-f", parameters.get("framerate"),
        "-s", parameters.get("startup"),

        "-a", parameters.get("algorithm"),
        "-r", parameters.get("range"),
        "-t", parameters.get("timestamp"),
        "--survival-min", parameters.get("survival_range_min"),
        "--survival-max", parameters.get("survival_range_max"),
        "--birth-min", parameters.get("birth_range_min"),
        "--birth-max", parameters.get("birth_range_max"),
        "--kernel-center", parameters.get("kernel_center"),
        "--kernel-width", parameters.get("kernel_width"),
        "--growth-center", parameters.get("growth_center"),
        "--growth-width", parameters.get("growth_width"),
    ];
    if (parameters.get("pause") == "true") {
        argv.push("-p");
    }

    const options = {
        print: (text) => { log(`${text}`); },
        printErr: (text) => { log(`[ERROR] ${text}`); },

        locateFile: (path, prefix) => { return "js/bin/" + path; },

        canvas: canvas,
        arguments: argv,
    };

    log("Loading main script...")
    try {

        Module(options).then(function(mymod) {
            window.module = mymod;
        });

        log("wasm script: loading successful");
    }
    catch (err) {

        log(`[ERROR] wasm script: loading failed, err=${err.message}`);
    }
};

window.addEventListener("load", onGlobalPageLoad);
