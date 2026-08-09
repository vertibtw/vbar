/*
 * this is a separate header just because I wanted the main.cpp to be a bit shorter, and it can't be a css file, cuz I
 * don't want to deal with more installation than necessary.
 */
#pragma once
#include "./themes/catppuccin/frappe.hpp"
#include "./themes/catppuccin/latte.hpp"
#include "./themes/catppuccin/macchiato.hpp"
#include "./themes/catppuccin/mocha.hpp"
#include <string_view>
namespace themes {
inline constexpr std::string_view core = R"CSS(
* {
    font-family: 'Scientifica';
    font-size: 15px;
    font-weight: bold;
}
.bar {
    background-color: alpha(var(--background_main), 0.85);
/*    background-color: #ffffff;*/
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.4);
}
label.rotated {
    transform: rotate(90deg);
}
.left-box {
    margin-left: 10px;
}
.right-box {
    margin-right: 10px;
}

.horizontal_bar .workspace {
    min-width: 20px;
    min-height: 4px;
    background: var(--background_light);
    margin: 0 2px;
    transition: background 200ms ease, min-width 200ms ease, min-height 200ms ease;
}
.horizontal_bar .workspace.occupied {
    background: var(--foreground_secondary);
}
.horizontal_bar .workspace.active {
    min-width: 28px;
    min-height: 4px;
    background: var(--purple);
}

.vertical_bar .workspace {
    min-height: 20px;
    min-width: 4px;
    background: var(--background_light);
    margin: 2 0px;
}
.vertical_bar .workspace.occupied {
    background: var(--foreground_secondary);
}
.vertical_bar .workspace.active {
    min-height: 28px;
    min-width: 4px;
    background: var(--purple);
}

.clock {
    color: var(--foreground_main);
}
.volume_button button {
    background: none;
    box-shadow: none;
    border: none;
}
.volume_window {
    border-radius: 0;
}
.volume_window contents {
    border-radius: 0;
    background: var(--background_main);
    min-width: 200px;
}
.volume_window arrow {
    background: var(--background_main);
    min-width: 12px;
    min-height: 6px;
}
.volume_window scale trough {
    border: none;
    background: var(--foreground_secondary);
    min-height: 6px;
    border-radius: 0;
}
.volume_window scale highlight {
    background: var(--purple);
}
.volume_window scale slider {
    background: var(--foreground_main);
    border-radius: 0;
    border: none;
}
.volume_button label {
    color: var(--foreground_main);
}
.volume_button arrow {
    min-width: 0;
    min-height: 0;
    opacity: 0;
}
.window_title label {
    color: var(--foreground_main);
}
.battery label {
    color: var(--foreground_main);
}
.battery .low {
    color: var(--orange);
}
.battery .full {
    color: var(--foreground_main);
}
.battery .discharging {
    color: var(--foreground_main);
}
.battery .not_charging {
    color: var(--foreground_main);
}
.battery .charging {
    color: var(--green);
}
.battery .unknown {
    color: var(--purple);
}
)CSS";
}
