#pragma once

#include <string_view>
namespace themes::catppuccin {
inline constexpr std::string_view mocha =
    R"CSS(:root {
        --background_main: #11111b;
        --background_secondary: #1e1e2e;
        --background_light: #313244;
        --foreground_main: #cdd6f4;
        --foreground_secondary: #7f849c;
        --red: #f38ba8;
        --orange: #fab387;
        --green: #a6e3a1;
        --blue: #89b4fa;
        --purple: #cba6f7;
    })CSS";
} // namespace themes::catppuccin
