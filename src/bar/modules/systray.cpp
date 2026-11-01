#include "systray.hpp"
#include "../../services/tray_service.hpp"

namespace bar::modules {

Systray::Systray(std::shared_ptr<bar::services::TrayService> service)
    : service(service) {
    this->service->changed.connect(sigc::mem_fun(*this, &Systray::rebuild));
    this->rebuild();
}

void Systray::rebuild() {
    while (auto *child = this->get_first_child())
        this->remove(*child);

    for (const auto &item : this->service->items()) {
        auto *button = Gtk::make_managed<Gtk::Button>();
        button->add_css_class("image_btn");
        button->set_has_frame(false);
        button->set_tooltip_text(item->title().empty() ? item->id() : item->title());

        auto *image = Gtk::make_managed<Gtk::Image>();
        image->set_pixel_size(16);
        if (auto texture = item->icon())
            image->set(texture);
        else {
            auto name = item->icon_name();
            image->set_from_icon_name(name.empty() ? "image-missing" : name);
        }
        button->set_child(*image);

        auto *raw = item.get();
        button->signal_clicked().connect([raw]() { raw->activate(); });

        auto right_click = Gtk::GestureClick::create();
        right_click->set_button(GDK_BUTTON_SECONDARY);
        right_click->signal_pressed().connect([raw](int, double, double) { raw->context_menu(); });
        button->add_controller(right_click);

        this->append(*button);
    }
}

} // namespace bar::modules
