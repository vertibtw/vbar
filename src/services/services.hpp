#pragma once
#include <memory>

namespace bar::services {
class VolumeService;
class MprisService;
class TrayService;
} // namespace bar::services

namespace bar {
struct Services {
    std::shared_ptr<services::VolumeService> volume;
    std::shared_ptr<services::MprisService> mpris;
    std::shared_ptr<services::TrayService> tray;
};
} // namespace bar
