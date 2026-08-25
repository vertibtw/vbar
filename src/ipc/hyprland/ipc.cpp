#include "ipc.hpp"

namespace hyprland {
Ipc::Ipc() {
    this->his = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    this->runtimedir = std::getenv("XDG_RUNTIME_DIR");

    if (this->his.empty() || this->runtimedir.empty()) {
        lg::err("hyprland env not set (is hyprland running?)");
        std::exit(1);
    }

    this->event_thread = std::thread(&Ipc::socket2, this);
}

Ipc::~Ipc() {
    this->running = false;
    if (this->s2 >= 0)
        close(this->s2);
    if (this->event_thread.joinable())
        this->event_thread.join();
}

std::string Ipc::socket1(std::string cmd) {
    std::string socket_path = this->runtimedir + "/hypr/" + this->his + "/.socket.sock";

    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s < 0) {
        // idk what to put here, so I'll just have different exit statuses for now
        lg::err("couldn't connect to hyprland's ipc");
        std::exit(1);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path.c_str());

    if (connect(s, (sockaddr *)&addr, sizeof(addr)) < 0) {
        lg::err("couldn't connect to hyprland's ipc");
        std::exit(2);
    }

    send(s, cmd.c_str(), cmd.size(), 0);

    char buffer[4096]; // probably doesn't have to be larger idk
    ssize_t received;
    std::string response;

    while ((received = recv(s, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[received] = '\0';
        response.append(buffer, received);
    }
    if (received < 0) {
        lg::err(std::format("recv failed: {}", strerror(errno)));
    }
    close(s);
    return response;
}

void Ipc::socket2() {
    std::string socket_path = this->runtimedir + "/hypr/" + this->his + "/.socket2.sock";

    this->s2 = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path.c_str());

    if (connect(this->s2, (sockaddr *)&addr, sizeof(addr)) < 0) {
        lg::err("couldn't connect to hyprland's ipc");
        std::exit(3);
    }

    char buf[4096];
    while (this->running) {
        ssize_t n = read(this->s2, buf, sizeof(buf) - 1);
        if (n <= 0)
            break;
        buf[n] = '\0';

        std::istringstream iss(buf);
        std::string line;
        while (std::getline(iss, line)) {
            auto pos = line.find(">>");
            if (pos == std::string::npos)
                continue;
            std::string event = line.substr(0, pos);
            std::string arg = line.substr(pos + 2);
            if (on_event)
                on_event(event, arg);
        }
    }
}

std::vector<Workspace *> Ipc::get_initial_workspaces() {
    std::string raw_ws = socket1("j/workspaces");
    std::string raw_active_ws = socket1("j/activeworkspace");

    // TODO: check if the output is valid
    nlohmann::json j_ws = nlohmann::json::parse(raw_ws);
    nlohmann::json j_a_ws = nlohmann::json::parse(raw_active_ws);

    int active_ws_id = -1;
    if (j_a_ws.contains("id")) {
        active_ws_id = j_a_ws["id"].get<int>();
    } else {
        lg::err("ipc did not provide active workspace id");
    }

    std::vector<int> ws_ids;
    for (const auto &obj : j_ws) {
        if (obj.contains("id")) {
            int id = obj["id"].get<int>();
            if (id >= 0)
                ws_ids.push_back(id);
        } else {
            lg::err("workspace doesn't contain id");
        }
    }

    std::sort(ws_ids.begin(), ws_ids.end());

    std::vector<Workspace *> workspaces;

    for (int i = 1; i <= 10; i++) {
        auto ws = Gtk::make_managed<hyprland::Workspace>();
        ws->id = i;
        ws->add_css_class("workspace");
        ws->add_css_class("empty");
        workspaces.push_back(ws);
    }

    for (int id : ws_ids) {
        if (id >= 1 && id <= 10) {
            Workspace *ws = workspaces[id - 1];
            ws->exists = true;
            if (id == active_ws_id) {
                ws->active = true;
                ws->add_css_class("active");
                ws->add_css_class("occupied");
            } else {
                ws->add_css_class("occupied");
            }
        } else if (id > 10) { // workspace buttons with ids > 10 are separate
            auto ws = Gtk::make_managed<hyprland::Workspace>();
            ws->id = id;
            ws->exists = true;
            if (id == active_ws_id) {
                ws->active = true;
                ws->add_css_class("active");
                ws->add_css_class("occupied");
            } else {
                ws->add_css_class("occupied");
            }

            workspaces.push_back(ws);
        }
    }

    return workspaces;
}
} // namespace hyprland
