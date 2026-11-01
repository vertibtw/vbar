#pragma once
#include <gtkmm.h>
#include <memory>

namespace bar::services {
class VolumeService;
struct VolumeState;
} // namespace bar::services

namespace bar::modules {

class VolumeWindow : public Gtk::Popover {
    public:
    explicit VolumeWindow(std::shared_ptr<bar::services::VolumeService>);
    ~VolumeWindow() = default;

    private:
    std::shared_ptr<bar::services::VolumeService> service;
    Gtk::Scale *slider = nullptr;
    Gtk::ToggleButton *mute_btn = nullptr;
    bool updating = false;

    void update(bar::services::VolumeState);
    void update_mute_label(double, bool);
};

} // namespace bar::modules
