#pragma once
#include <gtkmm.h>
#include <memory>

namespace bar::services {
class VolumeService;
struct VolumeState;
} // namespace bar::services

namespace bar::modules {
class VolumeButton : public Gtk::MenuButton {
    public:
    Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
    VolumeButton(Gtk::Popover *, std::shared_ptr<bar::services::VolumeService>);
    void update_label(double volume, bool muted);
    ~VolumeButton() = default;

    private:
    std::shared_ptr<bar::services::VolumeService> service;

    void update(bar::services::VolumeState);
};
} // namespace bar::modules
