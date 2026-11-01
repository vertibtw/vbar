#pragma once
#include <giomm.h>
#include <sigc++/sigc++.h>

namespace bar::services {

struct MediaState {
    bool available = false;
    bool playing = false;
    bool can_next = false;
    bool can_previous = false;
    Glib::ustring title;
    Glib::ustring artist;
};

class MprisService {
    public:
    MprisService();

    const MediaState &state() const {
        return this->current;
    }

    void play_pause();
    void next();
    void previous();

    sigc::signal<void()> changed;

    private:
    Glib::RefPtr<Gio::DBus::Proxy> bus;
    Glib::RefPtr<Gio::DBus::Proxy> player;
    Glib::ustring player_name;
    MediaState current;

    void refresh_players();
    void update();
    void call(const char *method);
    Glib::ustring get_string(const char *property) const;
    bool get_bool(const char *property) const;
};

} // namespace bar::services
