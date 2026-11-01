#include "volume_window.hpp"
#include "../../../services/volume_service.hpp"

namespace bar::modules {

VolumeWindow::VolumeWindow(std::shared_ptr<bar::services::VolumeService> service)
    : service(service) {
    // TODO: don't accentally forget that this is always supposed to be horizontal when u finally get to the part where
    // you make the vertical bar work
    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 4);
    vbox->set_margin(4);

    this->slider = Gtk::make_managed<Gtk::Scale>(Gtk::Adjustment::create(50.0, 0.0, 100.0, 1.0, 10.0, 0.0));
    this->slider->set_digits(0);
    this->slider->set_draw_value(false);
    this->slider->set_size_request(140, -1);

    this->mute_btn = Gtk::make_managed<Gtk::ToggleButton>("Mute");

    vbox->append(*this->slider);
    vbox->append(*this->mute_btn);
    set_child(*vbox);

    slider->signal_value_changed().connect([this]() {
        if (!this->updating)
            this->service->set_volume(this->slider->get_value());
    });

    mute_btn->signal_toggled().connect([this]() {
        if (!this->updating)
            this->service->set_muted(this->mute_btn->get_active());
    });

    this->service->changed.connect(sigc::mem_fun(*this, &VolumeWindow::update));
    this->update(this->service->state());

    this->add_css_class("volume_window");
}

void VolumeWindow::update(bar::services::VolumeState state) {
    this->updating = true;
    this->slider->set_value(state.volume);
    this->mute_btn->set_active(state.muted);
    this->updating = false;

    this->update_mute_label(state.volume, state.muted);
}

void VolumeWindow::update_mute_label(double volume, bool muted) {
    if (muted)
        this->mute_btn->set_label("󰖁");
    else if (volume <= 0)
        this->mute_btn->set_label("󰝟");
    else if (volume <= 33)
        this->mute_btn->set_label("󰕿");
    else if (volume <= 66)
        this->mute_btn->set_label("󰖀");
    else if (volume <= 100)
        this->mute_btn->set_label("󰕾");
}

} // namespace bar::modules
