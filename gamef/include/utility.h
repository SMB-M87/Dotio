#ifndef UTILITY_H
#define UTILITY_H

#include <string>

namespace gamef {
    void handle_last_error(const std::string& context);

    void log_error(const std::string& error);

    unsigned long get_core_count();
}  // namespace game

#endif  // UTILITY_H
