#pragma once
#include "../util/log.hpp"
#include <format>
#include <giomm.h>

namespace dbus {
inline Glib::RefPtr<Gio::DBus::Proxy> proxy(const Glib::ustring &name, const Glib::ustring &path,
                                            const Glib::ustring &iface,
                                            Gio::DBus::ProxyFlags flags = Gio::DBus::ProxyFlags::NONE) {
    try {
        return Gio::DBus::Proxy::create_for_bus_sync(Gio::DBus::BusType::SESSION, name, path, iface,
                                                     Glib::RefPtr<Gio::DBus::InterfaceInfo>(), flags);
    } catch (const Glib::Error &e) {
        lg::warn(std::format("dbus proxy '{}' failed: {}", name, e.what()));
        return {};
    }
}
} // namespace dbus
