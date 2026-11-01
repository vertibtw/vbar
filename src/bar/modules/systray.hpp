#pragma once
#include <gtkmm.h>
#include <memory>

namespace bar::services {
class TrayService;
}

namespace bar::modules {
class Systray : public Gtk::Box {
    public:
    explicit Systray(std::shared_ptr<bar::services::TrayService>);
    ~Systray() = default;

    private:
    std::shared_ptr<bar::services::TrayService> service;
    void rebuild();
};
} // namespace bar::modules
