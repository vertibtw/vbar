#include "volume_service.hpp"
#include "../util/log.hpp"
#include <algorithm>
#include <format>

namespace bar::services {

VolumeService::VolumeService() {
    wp_init(WP_INIT_ALL);
    // Connect to both the manager and the daemon, the same way wpctl does.
    this->core =
        wp_core_new(nullptr, nullptr, wp_properties_new("remote.name", "[pipewire-0-manager,pipewire-0]", NULL));

    wp_core_load_component(core, "libwireplumber-module-default-nodes-api", "module", nullptr, nullptr, nullptr,
                           on_component_loaded, this);
    wp_core_load_component(core, "libwireplumber-module-mixer-api", "module", nullptr, nullptr, nullptr,
                           on_component_loaded, this);

    if (!wp_core_connect(core))
        lg::err("could not connect to pipewire");
}

VolumeService::~VolumeService() {
    if (this->mixer_handler)
        g_signal_handler_disconnect(this->mixer, this->mixer_handler);
    if (this->nodes_handler)
        g_signal_handler_disconnect(this->default_nodes, this->nodes_handler);

    if (this->mixer)
        g_object_unref(this->mixer);
    if (this->default_nodes)
        g_object_unref(this->default_nodes);

    if (this->core) {
        wp_core_disconnect(this->core);
        g_object_unref(this->core);
    }
}

void VolumeService::on_component_loaded(GObject *source, GAsyncResult *res, gpointer data) {
    auto *self = static_cast<VolumeService *>(data);
    auto *core = WP_CORE(source);
    GError *error = nullptr;
    if (!wp_core_load_component_finish(core, res, &error)) {
        lg::err(std::format("could not load wireplumber component: {}", error ? error->message : "unknown error"));
        g_clear_error(&error);
        return;
    }

    if (--self->pending_plugins > 0)
        return;

    self->mixer = wp_plugin_find(core, "mixer-api");
    self->default_nodes = wp_plugin_find(core, "default-nodes-api");
    if (!self->mixer || !self->default_nodes) {
        lg::err("could not load the wireplumber mixer/default-nodes apis");
        return;
    }

    g_object_set(self->mixer, "scale", 1, NULL); // cubic, same as wpctl
    self->mixer_handler = g_signal_connect(self->mixer, "changed", G_CALLBACK(on_mixer_changed), self);
    self->nodes_handler = g_signal_connect(self->default_nodes, "changed", G_CALLBACK(on_nodes_changed), self);
    self->ready = true;
    self->refresh();
}

void VolumeService::on_mixer_changed(WpPlugin *, guint, gpointer data) {
    static_cast<VolumeService *>(data)->refresh();
}

void VolumeService::on_nodes_changed(WpPlugin *, gpointer data) {
    static_cast<VolumeService *>(data)->refresh();
}

guint VolumeService::default_sink_id() {
    guint id = 0;
    g_signal_emit_by_name(this->default_nodes, "get-default-node", "Audio/Sink", &id);
    return id;
}

void VolumeService::refresh() {
    if (!this->ready)
        return;

    guint id = this->default_sink_id();
    GVariant *dict = nullptr;
    g_signal_emit_by_name(this->mixer, "get-volume", id, &dict);
    if (!dict)
        return;

    double volume = 0.0;
    gboolean muted = FALSE;
    g_variant_lookup(dict, "volume", "d", &volume);
    g_variant_lookup(dict, "mute", "b", &muted);
    g_variant_unref(dict);

    this->current = {volume * 100.0, muted != FALSE};
    this->changed.emit(this->current);
}

void VolumeService::set_volume(double pct) {
    if (!this->ready)
        return;

    GVariantBuilder b;
    g_variant_builder_init(&b, G_VARIANT_TYPE_VARDICT);
    g_variant_builder_add(&b, "{sv}", "volume", g_variant_new_double(std::clamp(pct, 0.0, 100.0) / 100.0));
    GVariant *variant = g_variant_builder_end(&b);

    gboolean ok = false;
    g_signal_emit_by_name(this->mixer, "set-volume", this->default_sink_id(), variant, &ok);
}

void VolumeService::set_muted(bool muted) {
    if (!this->ready)
        return;

    GVariantBuilder b;
    g_variant_builder_init(&b, G_VARIANT_TYPE_VARDICT);
    g_variant_builder_add(&b, "{sv}", "mute", g_variant_new_boolean(muted));
    GVariant *variant = g_variant_builder_end(&b);

    gboolean ok = false;
    g_signal_emit_by_name(this->mixer, "set-volume", this->default_sink_id(), variant, &ok);
}

} // namespace bar::services
