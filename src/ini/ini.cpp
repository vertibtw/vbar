#include "ini.hpp"

#define NEXT(i, bs, errormsg)                                                                                          \
    do {                                                                                                               \
        if ((i) >= (bs)) {                                                                                             \
            lg::err(errormsg);                                                                                         \
            std::exit(EXIT_FAILURE);                                                                                   \
        }                                                                                                              \
        (i)++;                                                                                                         \
    } while (0)

namespace {
std::string trim(const std::string &s) {
    size_t start = 0;
    size_t end = s.size();

    while (start < end && isspace(s[start]))
        start++;

    while (end > start && isspace(s[end - 1]))
        end--;

    return s.substr(start, end - start);
}
} // namespace

ini ini::parse(std::string content) {
    ini r;
    std::istringstream c(content);
    std::string buffer, section, key = "";
    int64_t line_count = 0;

    while (std::getline(c, buffer)) {
        line_count++;
        int64_t bs = buffer.size();
        for (int64_t i = 0; i < bs; i++) {
            if (std::isspace(buffer[i]))
                continue;
            else if (buffer[i] == '#' || buffer[i] == ';')
                break;
            else if (buffer[i] == '[') {

                std::string errormsg = "no closing '] line (" + std::to_string(line_count) + ")";
                section.clear();
                NEXT(i, bs, errormsg);
                while (buffer[i] != ']') {
                    section += buffer[i];
                    if (i >= bs) {
                        lg::err(std::format("{} (line {})", errormsg, line_count));
                        std::exit(1);
                    }
                    NEXT(i, bs, errormsg);
                }
                // TODO: don't allow junk after section declarations
                break;

            } else if (key == "") {

                std::string errormsg = "no key provided (line " + std::to_string(line_count) + ")";
                while (buffer[i] != '=') {
                    key += buffer[i];
                    NEXT(i, bs, errormsg);
                }

                key = trim(key);

                if (key == "") {
                    lg::err(std::format("{} (line {})", errormsg, line_count));
                    std::exit(1);
                }

                i--; // so that we also parse the =
            } else if (buffer[i] == '=') {

                std::string errormsg = "no value for key" + key;
                std::string value = "";

                NEXT(i, bs, errormsg);

                while (i < bs && buffer[i] != '#') {
                    value += buffer[i++];
                }
                value = trim(value);
                lg::info(std::format("[{}] '{}' = '{}'", section, key, value));

                r.conf[section][key] = value;
                key.clear();

                break;
            } else {
                lg::err(std::format("unexpected EOF, on line {}", line_count));
                std::exit(EXIT_FAILURE);
            }
        }
    }
    return r;
}
