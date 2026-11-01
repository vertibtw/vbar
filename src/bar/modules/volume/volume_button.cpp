#include "volume_button.hpp"
#include "../../../services/volume_service.hpp"

namespace bar::modules {
VolumeButton::VolumeButton(Gtk::Popover *popover, std::shared_ptr<bar::services::VolumeService> service)
    : service(service) {
    this->set_popover(*popover);
    this->set_child(*this->label);
    this->set_has_frame(false);
    this->set_always_show_arrow(false);
    this->add_css_class("volume_button");

    this->service->changed.connect(sigc::mem_fun(*this, &VolumeButton::update));
    this->update(this->service->state());
}

void VolumeButton::update(bar::services::VolumeState state) {
    this->update_label(state.volume, state.muted);
}

void VolumeButton::update_label(double volume, bool muted) {
    if (muted)
        this->label->set_text("󰖁");
    else if (volume <= 0)
        this->label->set_text("󰝟");
    else if (volume <= 33)
        this->label->set_text("󰕿");
    else if (volume <= 66)
        this->label->set_text("󰖀");
    else if (volume <= 100)
        this->label->set_text("󰕾");
}
} // namespace bar::modules
