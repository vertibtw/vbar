#include "media.hpp"
#include "../../services/mpris_service.hpp"

namespace bar::modules {

// should probably move this to util later
static Gtk::Button *image_button(Gtk::Image *&image, const char *icon) {
    auto *button = Gtk::make_managed<Gtk::Button>();

    button->add_css_class("image_btn");
    button->set_has_frame(false);
    image = Gtk::make_managed<Gtk::Image>();
    image->set_from_icon_name(icon);
    image->set_pixel_size(16);
    button->set_child(*image);

    return button;
}

Media::Media(std::shared_ptr<bar::services::MprisService> service)
    : service(service) {
    this->set_spacing(2);

    this->label->set_ellipsize(Pango::EllipsizeMode::END);
    this->label->set_max_width_chars(40);
    this->label->set_margin_start(4);
    this->label->set_margin_end(4);

    Gtk::Image *previous_image = nullptr;
    Gtk::Image *next_image = nullptr;
    this->previous_btn = image_button(previous_image, "media-skip-backward-symbolic");
    this->play_btn = image_button(this->play_image, "media-playback-start-symbolic");
    this->next_btn = image_button(next_image, "media-skip-forward-symbolic");

    this->append(*this->previous_btn);
    this->append(*this->play_btn);
    this->append(*this->next_btn);
    this->append(*this->label);

    this->previous_btn->signal_clicked().connect([this]() { this->service->previous(); });
    this->play_btn->signal_clicked().connect([this]() { this->service->play_pause(); });
    this->next_btn->signal_clicked().connect([this]() { this->service->next(); });

    this->service->changed.connect(sigc::mem_fun(*this, &Media::update));
    this->update();
}

void Media::update() {
    const auto &state = this->service->state();
    this->set_visible(state.available);
    if (!state.available)
        return;

    Glib::ustring text = state.title;
    if (!state.artist.empty())
        text = state.artist + " - " + state.title;

    this->label->set_text(text.empty() ? "Unknown" : text);
    this->previous_btn->set_sensitive(state.can_previous);
    this->next_btn->set_sensitive(state.can_next);
    this->play_image->set_from_icon_name(state.playing ? "media-playback-pause-symbolic"
                                                       : "media-playback-start-symbolic");
}

} // namespace bar::modules
