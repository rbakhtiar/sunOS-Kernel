
const output = document.getElementById("output");
const command = document.getElementById("command");

function print(text) {
    output.innerHTML +=
        document.createElement("div").textContent = text;
}

function runCommand(value) {
    const cmd = value.trim().toLowerCase();

    output.innerHTML += `<div>root@sunos:~$ ${escapeHTML(value)}</div>`;

    switch (cmd) {
        case "help":
            print("Available commands:");
            print("help - Show commands");
            print("version - Show version");
            print("clear - Clear terminal");
            print("date - Show current date");
            print("about - About sunOS");
            break;

        case "version":
            print("sunOS Developer Preview 0.1");
            break;

        case "date":
            print(new Date().toString());
            break;

        case "about":
            showAbout();
            break;

        case "clear":
            output.innerHTML = "";
            break;

        default:
            print("Unknown command: " + value);
    }

    output.scrollTop = output.scrollHeight;
}

function escapeHTML(value) {
    return value.replace(/[&<>"']/g, char => ({
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#39;"
    })[char]);
}

command.addEventListener("keydown", event => {
    if (event.key === "Enter") {
        runCommand(command.value);
        command.value = "";
    }
});

function openTerminal() {
    document.getElementById("terminal").style.display = "block";
    command.focus();
}

function closeTerminal() {
    document.getElementById("terminal").style.display = "none";
}

function showAbout() {
    print("sunOS — Built from the ground up.");
    print("Architecture: x86_64");
    print("Firmware target: UEFI");
    print("Kernel: Hybrid");
}

function updateClock() {
    document.getElementById("clock").textContent =
        new Date().toLocaleTimeString();
}

updateClock();
setInterval(updateClock, 1000);
