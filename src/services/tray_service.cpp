#include "tray_service.hpp"
#include "../dbus/proxy.hpp"
#include "../util/log.hpp"
#include <algorithm>
#include <format>
#include <unistd.h>

namespace bar::services {

TrayItem::TrayItem(const Glib::ustring &identifier)
    : identifier(identifier) {
    auto slash = this->identifier.find('/');
    Glib::ustring service = slash == Glib::ustring::npos ? this->identifier : this->identifier.substr(0, slash);
    Glib::ustring path = slash == Glib::ustring::npos ? "/StatusNotifierItem" : this->identifier.substr(slash);

    this->proxy = dbus::proxy(service, path, "org.kde.StatusNotifierItem", Gio::DBus::ProxyFlags::DO_NOT_AUTO_START);
    if (!this->proxy)
        return;

    this->proxy->signal_properties_changed().connect([this](const auto &, const auto &) { this->changed.emit(); });

    for (const char *signal : {"NewIcon", "NewAttentionIcon", "NewStatus", "NewTitle", "NewToolTip", "NewMenu"}) {
        this->proxy->signal_signal(signal).connect(
            [this](const Glib::ustring &, const Glib::ustring &, const Glib::VariantContainerBase &) {
                this->changed.emit();
            });
    }
}

Glib::ustring TrayItem::title() const {
    return this->get_string("Title");
}

Glib::ustring TrayItem::icon_name() const {
    return this->get_string("IconName");
}

Glib::RefPtr<Gdk::Texture> TrayItem::icon() const {
    Glib::VariantBase value;
    this->proxy->get_cached_property(value, "IconPixmap");
    if (!value.gobj() || !g_variant_is_of_type(value.gobj(), G_VARIANT_TYPE("a(iiay)")))
        return {};

    GVariantIter iter;
    g_variant_iter_init(&iter, value.gobj());

    gint32 width = 0, height = 0;
    GVariant *pixels = nullptr;
    while (g_variant_iter_next(&iter, "(ii@ay)", &width, &height, &pixels)) {
        gsize size = g_variant_get_size(pixels);
        if (width <= 0 || height <= 0 || size < static_cast<gsize>(width) * height * 4) {
            g_variant_unref(pixels);
            continue;
        }

        const auto *argb = static_cast<const guint8 *>(g_variant_get_data(pixels));
        std::vector<guint8> rgba(static_cast<size_t>(width) * height * 4);
        for (gsize i = 0; i < rgba.size(); i += 4) {
            // StatusNotifierItem sends ARGB32 in network byte order for some reason
            rgba[i] = argb[i + 1];
            rgba[i + 1] = argb[i + 2];
            rgba[i + 2] = argb[i + 3];
            rgba[i + 3] = argb[i];
        }
        g_variant_unref(pixels);

        auto bytes = Glib::Bytes::create(rgba.data(), rgba.size());
        return Gdk::MemoryTexture::create(width, height, Gdk::MemoryTexture::Format::R8G8B8A8, bytes, width * 4);
    }

    return {};
}

void TrayItem::activate() {
    this->call("Activate");
}

void TrayItem::context_menu() {
    // TODO
}

Glib::ustring TrayItem::get_string(const char *property) const {
    Glib::VariantBase value;
    this->proxy->get_cached_property(value, property);
    if (!value.gobj() || !g_variant_is_of_type(value.gobj(), G_VARIANT_TYPE_STRING))
        return {};
    return g_variant_get_string(value.gobj(), nullptr);
}

bool TrayItem::item_is_menu() const {
    Glib::VariantBase value;
    this->proxy->get_cached_property(value, "ItemIsMenu");
    return value.gobj() && g_variant_is_of_type(value.gobj(), G_VARIANT_TYPE_BOOLEAN) &&
           g_variant_get_boolean(value.gobj());
}

void TrayItem::call(const char *method) {
    std::vector<Glib::VariantBase> args{Glib::Variant<int>::create(0), Glib::Variant<int>::create(0)};
    try {
        this->proxy->call_sync(method, Glib::VariantContainerBase::create_tuple(args));
    } catch (const Glib::Error &e) {
        lg::warn(std::format("statusnotifier '{}' failed: {}", method, e.what()));
    }
}

TrayService::TrayService() {
    // I didn't want to implement my own watcher, so use something like
    // https://github.com/NBonaparte/gdbus-sni-watcher
    this->watcher =
        dbus::proxy("org.kde.StatusNotifierWatcher", "/StatusNotifierWatcher", "org.kde.StatusNotifierWatcher");
    if (!this->watcher || this->watcher->get_name_owner().empty()) {
        lg::warn("no StatusNotifierWatcher found, systray will be empty");
        lg::note(
            "vbar does not implement its own status notifier watcher, find and use your own idk"); // yes I just wanted
                                                                                                   // another log level.

        this->watcher.reset();
        return;
    }

    try {
        this->watcher->call_sync("RegisterStatusNotifierHost",
                                 Glib::VariantContainerBase::create_tuple(Glib::Variant<Glib::ustring>::create(
                                     "org.kde.StatusNotifierHost-" + std::to_string(getpid()))));
    } catch (const Glib::Error &e) {
        lg::warn(std::format("could not register systray host: {}", e.what()));
    }

    this->watcher->signal_signal("StatusNotifierItemRegistered")
        .connect([this](const Glib::ustring &, const Glib::ustring &, const Glib::VariantContainerBase &args) {
            Glib::VariantBase item = args.get_child(0);
            this->add(g_variant_get_string(item.gobj(), nullptr));
        });

    this->watcher->signal_signal("StatusNotifierItemUnregistered")
        .connect([this](const Glib::ustring &, const Glib::ustring &, const Glib::VariantContainerBase &args) {
            Glib::VariantBase item = args.get_child(0);
            this->remove(g_variant_get_string(item.gobj(), nullptr));
        });

    Glib::VariantBase items;
    this->watcher->get_cached_property(items, "RegisteredStatusNotifierItems");
    if (items.gobj()) {
        gchar **list = g_variant_dup_strv(items.gobj(), nullptr);
        for (gsize i = 0; list[i]; i++)
            this->add(list[i]);
        g_strfreev(list);
    }
}

void TrayService::add(const Glib::ustring &identifier) {
    if (std::any_of(this->tray_items.begin(), this->tray_items.end(),
                    [&](const auto &item) { return item->id() == identifier; }))
        return;

    auto item = std::make_unique<TrayItem>(identifier);
    if (!item->valid())
        return;

    item->changed.connect([this]() { this->changed.emit(); });
    this->tray_items.push_back(std::move(item));
    this->changed.emit();
}

void TrayService::remove(const Glib::ustring &identifier) {
    auto it = std::find_if(this->tray_items.begin(), this->tray_items.end(),
                           [&](const auto &item) { return item->id() == identifier; });
    if (it == this->tray_items.end())
        return;

    this->tray_items.erase(it);
    this->changed.emit();
}

} // namespace bar::services
