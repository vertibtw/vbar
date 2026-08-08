#include "window_title.hpp"
#include "../util/extern/json.hpp"
#include "bar.hpp"

namespace bar::modules {
WindowTitle::WindowTitle(std::shared_ptr<hyprland::Ipc> Ipc)
    : ipc(Ipc) {
    this->set_orientation(bar::orientation);
    this->set_spacing(4);
    this->add_css_class("window_title");

    this->app_icons = this->get_app_icons();
    std::string app, win_title, raw = ipc->socket1("j/activewindow");
    nlohmann::json j = nlohmann::json::parse(raw);

    if (j.empty()) {
        app = "";
        win_title = "";
    } else {
        if (j.contains("class"))
            app = j["class"];
        if (j.contains("title"))
            win_title = j["title"];
    }

    this->title = Gtk::make_managed<Gtk::Label>();
    this->title->set_max_width_chars(30);
    this->title->set_ellipsize(Pango::EllipsizeMode::END);

    if (bar::orientation == Gtk::Orientation::VERTICAL) {
        this->title->add_css_class("rotated");
    }

    this->icon = Gtk::make_managed<Gtk::Image>();

    this->append(*this->icon);
    this->append(*this->title);

    this->on_window_title_change(app + "," + win_title);
}

void WindowTitle::on_window_title_change(std::string new_title) {
    auto pos = new_title.find(",");
    if (pos == std::string::npos)
        return;
    std::string app_name = new_title.substr(0, pos);
    std::string win_title = new_title.substr(pos + 1);

    if (app_name.empty()) {
        this->icon->clear();
        this->title->set_text("un sentimiento de vacío"); // so espaniol muchacho
        return;
    }

    for (auto &c : app_name)
        c = std::tolower(c);
    this->icon->set(app_icons[app_name]);
    this->title->set_text(win_title);
}

std::unordered_map<std::string, Glib::RefPtr<Gio::Icon>> WindowTitle::get_app_icons() {
    // so c++
    auto lower = [](std::string s) {
        for (auto &c : s)
            c = std::tolower(c);
        return s;
    };

    std::unordered_map<std::string, Glib::RefPtr<Gio::Icon>> r;
    for (auto &app : Gio::AppInfo::get_all()) {
        auto id = app->get_id();
        if (id.size() > 8 && id.ends_with(".desktop"))
            id.resize(id.size() - 8);
        r[lower(id)] = app->get_icon();

        auto exec = app->get_executable();
        // the get_executable returns something like /usr/bin/something something so have to do ts
        auto slash = exec.rfind('/');
        if (slash != std::string::npos)
            exec = exec.substr(slash + 1);
        r[lower(exec)] = app->get_icon();
    }
    return r;
}
} // namespace bar::modules
