#include "utility.h"
#include "pch.h"

namespace gamef {
    void handle_last_error(const std::string& context) {
        const DWORD errorCode = GetLastError();

        if (errorCode == ERROR_SUCCESS)
            return;

        DebugBreak();
        LPWSTR errorMessage = nullptr;

        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&errorMessage),
            0,
            nullptr);

        if (errorMessage != nullptr) {
            std::wcerr << "Error (" << errorCode << ") occurred in ";
            std::cerr << "\'" << context << '\"';
            std::wcerr << ": " << errorMessage << std::endl;
            LocalFree(errorMessage);
        }

        else
            std::cerr << "Error (" << errorCode << ") occurred in \"" << context << '\"' << std::endl;
    }

    void log_error(const std::string& error) {
        DebugBreak();
        std::cerr << error << std::endl;
    }
}  // namespace game
