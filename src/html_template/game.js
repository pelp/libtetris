Module = {
    print: console.log
};

const generate_html = (width, height) => {
    const elements = [];
    const shape_elements = [];
    const game = document.querySelector("#game");
    const wrapper = document.querySelector("#wrapper");

    const info_wrapper = document.createElement("div");
    const lines_display = document.createElement("div");
    const restart_button = document.createElement("div");

    const lb_wrapper = document.createElement("div");
    const leaderboard = document.createElement("div");
    const name_input = document.createElement("input");
    name_input.value = window.localStorage.getItem("name") || "anonymous";
    name_input.onchange = event => {
        window.localStorage.setItem("name", name_input.value);
    };

    const updateLeaderboard = (scores) => {
        leaderboard.innerHTML = "";
        scores.forEach((person, _) => {
            const span = document.createElement("span");
            const n = document.createElement("span");
            const t = document.createElement("span");
            span.appendChild(n);
            span.appendChild(t);
            n.innerHTML = person[0];
            t.innerHTML = person[1];
            leaderboard.appendChild(span);
        });
    };

    // Setup WebSocket for leaderboard
    const ws_link = (window.location.protocol === "http:") ?
        `ws://${window.location.hostname}:8763` :
        `wss://${window.location.hostname}/ws`;
    const socket = new WebSocket(ws_link);
    socket.onmessage = event => {
        updateLeaderboard(JSON.parse(event.data));
    };

    const shape = document.createElement("div");
    // shape.classList.add("grid_wrapper");
    shape.classList.add("shape");

    game.classList.add("game");
    lb_wrapper.classList.add("leaderboard");
    lb_wrapper.appendChild(shape);
    lb_wrapper.appendChild(name_input);
    lb_wrapper.appendChild(leaderboard);

    restart_button.classList.add("restart");
    lines_display.classList.add("text_info");
    const lines_span = document.createElement("div");
    lines_display.appendChild(lines_span);

    const shape_wrapper = document.createElement("div");
    shape.appendChild(shape_wrapper);

    let old_w = 0;
    let old_h = 0;

    const render = () => {
        const ptr = Module._js_get();
        lines_span.innerHTML = Module._js_lines().toString().padStart(3, '0');
        elements.forEach((e, i) => {
            e.dataset.tile = Module.HEAP8[i + ptr];
        });

        const shape_width = Module._js_next_width();
        const shape_height = Module._js_next_height();
        if (!(old_w == shape_width && old_h == shape_height)) {
            old_w = shape_width;
            old_h = shape_height;
            shape_wrapper.innerHTML = "";
            shape_wrapper.style.gridTemplateColumns = "auto ".repeat(shape_width);
            shape_elements.length = 0;
            for (let i = 0; i < shape_height; i++) {
                const line = document.createElement("div");
                for (let j = 0; j < shape_width; j++) {
                    const e = document.createElement("div");
                    e.classList.add("square");
                    shape_wrapper.appendChild(e);
                    shape_elements.push(e);
                }
            }
        }
        const shape_ptr = Module._js_next();

        shape_elements.forEach((e, i) => {
            e.dataset.tile = Module.HEAP8[i + shape_ptr];
        });
    };

    const handle = rc => {
        switch (rc) {
            case 0:
                return 0;
            case 1:
                playing = false;
                postScore();
                return 1;
            case 2:
                return 1;
        }
    };
    const timeout = () => {
        if (playing) {
            handle(Module._js_step());
            render();
        }
        setTimeout(timeout, 1000 - 10 * Module._js_lines());
    };
    setTimeout(timeout, 1000);

    const restart = () => {
        playing = true;
        Module._js_init(width, height);
        restart_button.innerHTML = "Restart";
        render();
    };

    restart();

    wrapper.classList.add("wrapper");

    const postScore = () => {
        const lines = Module._js_lines();
        const name = name_input.value.toLowerCase();
        socket.send(JSON.stringify([name, lines]));
    };

    // Generate info
    restart_button.onclick = event => {
        restart();
    };

    info_wrapper.appendChild(lines_display);
    info_wrapper.appendChild(restart_button);

    info_wrapper.classList.add("info_wrapper");
    wrapper.appendChild(info_wrapper);

    // Generate grid
    const grid_wrapper = document.createElement("div");
    grid_wrapper.classList.add("grid_wrapper");

    const generate_grid = () => {
        grid_wrapper.innerHTML = "";
        grid_wrapper.style.gridTemplateColumns = "auto ".repeat(width);
        elements.length = 0;

        for (let i = 0; i < height; i++) {
            const line = document.createElement("div");
            for (let j = 0; j < width; j++) {
                const e = document.createElement("div");
                e.classList.add("square");
                grid_wrapper.appendChild(e);
                elements.push(e);
            }
        }
        wrapper.appendChild(grid_wrapper);
        game.appendChild(wrapper);
        game.appendChild(lb_wrapper);
    };

    generate_grid();

    render();
    socket.onopen = event => {
        socket.send(JSON.stringify(['default']));
    };
    return [render, handle];
};

const width = 10;
const height = 20;
let playing = true;

Module.onRuntimeInitialized = event => {
    const [render, handle] = generate_html(width, height);
    document.onkeydown = event => {
        if (!playing) return; // Guard against not playing
        switch (event.code) {
            case "Space": // Space
                while (handle(Module._js_step()) === 0) {
                }
                render();
                break;
            case "ArrowLeft": //Left arrow
                Module._js_left();
                render();
                break;
            case "ControlLeft": // Up arrow
                Module._js_rotate_ccw();
                render();
                break;
            case "ArrowUp": // Up arrow
                Module._js_rotate_cw();
                render();
                break;
            case "ArrowRight": // Right arrow
                Module._js_right();
                render();
                break;
            case "ArrowDown":
                handle(Module._js_step());
                render();
                break;
            default:
                console.log(event.code);
                break;
        }
    };
};
