Module = {
    print: console.log
};

const GRID_WIDTH = 10;
const GRID_HEIGHT = 20;

Module.onRuntimeInitialized = () => {
    let playing = true;

    const keys = {
        down: false,
        left: false,
        right: false,
        rotate_cw: false,
        rotate_ccw: false,
        space: false
    };

    const generate_html = () => {
        const elements = [];
        const shape_elements = [];

        const restart_button = document.querySelector("#restart_button");
        const leaderboard = document.querySelector("#leaderboard");

        const name_input = document.querySelector("#name_input");
        name_input.value = window.localStorage.getItem("name") || "anonymous";
        name_input.onchange = event => {
            window.localStorage.setItem("name", name_input.value);
        };

        const updateLeaderboard = (scores) => {
            leaderboard.innerHTML = "";
            scores.forEach((person, _) => {
                const span = document.createElement("span");
                const name = document.createElement("span");
                const score = document.createElement("span");
                span.appendChild(name);
                span.appendChild(score);
                name.innerHTML = person[0];
                score.innerHTML = person[1];
                leaderboard.appendChild(span);
            });
        };

        // Setup WebSocket for leaderboard
        const ws_link = (window.location.protocol === "http:")
            ? `ws://${window.location.hostname}:8763`
            : `wss://${window.location.hostname}/ws`;
        const socket = new WebSocket(ws_link);
        socket.onmessage = event => {
            updateLeaderboard(JSON.parse(event.data));
        };
        socket.onopen = event => {
            socket.send(JSON.stringify(['default']));
        };
        const postScore = () => {
            const lines = Module._js_lines();
            const name = name_input.value.toLowerCase();
            socket.send(JSON.stringify([name, lines]));
        };

        const lines_span = document.querySelector("#lines_span");
        const shape_wrapper = document.querySelector("#shape_wrapper");

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
            if (!(old_w === shape_width && old_h === shape_height)) {
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
                case 1:
                    playing = false;
                    postScore();
                    return 1;
                case 2:
                    return 1;
                default:
                    return rc;
            }
        };

        let prev;
        const tick = (timeStamp) => {
            // TODO: Get more accurate clock
            const diff = timeStamp - prev
            prev = timeStamp
            const rc = handle(Module._js_tick(
                keys.space,
                keys.down,
                keys.left,
                keys.right,
                keys.rotate_cw,
                keys.rotate_ccw,
                keys.hold,
                diff * 1000));
            if(rc !== -1){
                Module._js_set_fall_interval(1000 * (1000 - 10 * Module._js_lines()))
                render();
            }
            window.requestAnimationFrame(tick)
        }
        window.requestAnimationFrame(tick)

        // Restart button
        const restart = () => {
            playing = true;
            Module._js_init(
                GRID_WIDTH,
                GRID_HEIGHT,
                Math.floor(1000000),
                166667,
                33000);
            render();
        };
        restart();
        restart_button.onclick = event => {
            restart();
        };

        // Generate grid
        {
            const grid_wrapper = document.querySelector("#grid_wrapper");
            grid_wrapper.innerHTML = "";
            grid_wrapper.style.gridTemplateColumns = "auto ".repeat(GRID_WIDTH);
            elements.length = 0;

            for (let i = 0; i < GRID_HEIGHT; i++) {
                const line = document.createElement("div");
                for (let j = 0; j < GRID_WIDTH; j++) {
                    const e = document.createElement("div");
                    e.classList.add("square");
                    grid_wrapper.appendChild(e);
                    elements.push(e);
                }
            }
        }

        render();
    };

    generate_html();

    const handle_key = (event, state) => {
        if (!playing) return; // Guard against not playing
        switch (event.code) {
            case "Space":
                keys.space = state;
                break;
            case "ArrowLeft":
                keys.left = state;
                break;
            case "ControlLeft":
                keys.rotate_ccw = state;
                break;
            case "ShiftLeft":
                keys.hold = state;
                break;
            case "ArrowUp":
                keys.rotate_cw = state;
                break;
            case "ArrowRight":
                keys.right = state;
                break;
            case "ArrowDown":
                keys.down = state;
                break;
            default:
                console.log(event.code);
                break;
        }
    }
    document.onkeydown = event => handle_key(event, true)
    document.onkeyup = event => handle_key(event, false)
};
