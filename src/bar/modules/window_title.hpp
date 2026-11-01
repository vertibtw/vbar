#pragma once
#include "../../ipc/hyprland/ipc.hpp"
#include <gtkmm.h>
#include <memory>
#include <string>

namespace bar {
namespace modules {
class WindowTitle : public Gtk::Box {
    private:
    Gtk::Image *icon;
    Gtk::Label *title;

    std::shared_ptr<hyprland::Ipc> ipc;

    std::unordered_map<std::string, Glib::RefPtr<Gio::Icon>> app_icons;
    [[nodiscard]] std::unordered_map<std::string, Glib::RefPtr<Gio::Icon>> get_app_icons();

    public:
    void on_window_title_change(std::string); // yes great name

    WindowTitle(std::shared_ptr<hyprland::Ipc>);
    ~WindowTitle() = default;
};
} // namespace modules
} // namespace bar
