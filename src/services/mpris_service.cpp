#include "mpris_service.hpp"
#include "../dbus/proxy.hpp"
#include "../util/log.hpp"
#include <algorithm>
#include <format>
#include <vector>

namespace bar::services {

MprisService::MprisService() {
    this->bus = dbus::proxy("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
    if (!this->bus)
        return;

    this->bus->signal_signal("NameOwnerChanged")
        .connect([this](const Glib::ustring &, const Glib::ustring &, const Glib::VariantContainerBase &args) {
            Glib::VariantBase name = args.get_child(0);
            if (g_str_has_prefix(g_variant_get_string(name.gobj(), nullptr), "org.mpris.MediaPlayer2."))
                this->refresh_players();
        });

    this->refresh_players();
}

void MprisService::refresh_players() {
    Glib::VariantContainerBase result;
    try {
        result = this->bus->call_sync("ListNames");
    } catch (const Glib::Error &e) {
        lg::warn(std::format("could not list dbus names: {}", e.what()));
        return;
    }

    Glib::VariantBase names_variant = result.get_child(0);
    gsize count = 0;
    gchar **names = g_variant_dup_strv(names_variant.gobj(), &count);

    std::vector<Glib::ustring> players;
    for (gsize i = 0; i < count; i++) {
        if (g_str_has_prefix(names[i], "org.mpris.MediaPlayer2."))
            players.emplace_back(names[i]);
    }
    g_strfreev(names);

    Glib::ustring selected;
    if (std::find(players.begin(), players.end(), this->player_name) != players.end())
        selected = this->player_name;
    else if (!players.empty())
        selected = players.front();

    if (selected != this->player_name || !this->player) {
        this->player_name = selected;
        this->player.reset();
        if (!selected.empty()) {
            this->player = dbus::proxy(selected, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player",
                                       Gio::DBus::ProxyFlags::DO_NOT_AUTO_START);
            if (this->player) {
                this->player->signal_properties_changed().connect(
                    [this](const auto &, const auto &) { this->update(); });
            }
        }
    }

    this->update();
}

void MprisService::update() {
    this->current = {};

    if (this->player) {
        this->current.available = true;
        this->current.playing = this->get_string("PlaybackStatus") == "Playing";
        this->current.can_next = this->get_bool("CanGoNext");
        this->current.can_previous = this->get_bool("CanGoPrevious");

        Glib::VariantBase metadata;
        this->player->get_cached_property(metadata, "Metadata");
        if (metadata.gobj()) {
            const gchar *title = nullptr;
            if (g_variant_lookup(metadata.gobj(), "xesam:title", "&s", &title) && title)
                this->current.title = title;

            GVariant *artists = g_variant_lookup_value(metadata.gobj(), "xesam:artist", G_VARIANT_TYPE_STRING_ARRAY);
            if (artists) {
                gsize count = 0;
                gchar **names = g_variant_dup_strv(artists, &count);
                for (gsize i = 0; i < count; i++) {
                    if (i)
                        this->current.artist += ", ";
                    this->current.artist += names[i];
                }
                g_strfreev(names);
                g_variant_unref(artists);
            }
        }
    }

    this->changed.emit();
}

void MprisService::call(const char *method) {
    if (!this->player)
        return;

    try {
        this->player->call_sync(method);
    } catch (const Glib::Error &e) {
        lg::warn(std::format("mpris '{}' failed: {}", method, e.what()));
    }
}

Glib::ustring MprisService::get_string(const char *property) const {
    Glib::VariantBase value;
    this->player->get_cached_property(value, property);
    if (!value.gobj() || !g_variant_is_of_type(value.gobj(), G_VARIANT_TYPE_STRING))
        return {};
    return g_variant_get_string(value.gobj(), nullptr);
}

bool MprisService::get_bool(const char *property) const {
    Glib::VariantBase value;
    this->player->get_cached_property(value, property);
    if (!value.gobj() || !g_variant_is_of_type(value.gobj(), G_VARIANT_TYPE_BOOLEAN))
        return false;
    return g_variant_get_boolean(value.gobj());
}

void MprisService::play_pause() {
    this->call("PlayPause");
}

void MprisService::next() {
    this->call("Next");
}

void MprisService::previous() {
    this->call("Previous");
}

} // namespace bar::services
