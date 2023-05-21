#ifdef _BUILD_STD_MODULE
#define EXPORT export
#else
#define EXPORT
#endif

#ifndef __cpp_lib_start_lifetime_as
namespace std {

template <typename T>
concept ImplicitLifetime = std::is_trivial_v<T> && std::is_trivially_destructible_v<T>;

EXPORT template <std::ImplicitLifetime T>
auto start_lifetime_as(void * Bytes) noexcept -> T * {
    static_assert(sizeof(T) > 0, "type T is incomplete");
    return new (Bytes) T;
}

EXPORT template <std::ImplicitLifetime T>
auto start_lifetime_as(const void * Bytes) noexcept -> const T * {
    static_assert(sizeof(T) > 0, "type T is incomplete");
    return new (Bytes) const T;
}

} // namespace std
#endif

#ifndef __cpp_lib_print
#ifdef _WIN32
extern "C" {
    __declspec(dllimport) intptr_t __stdcall _get_osfhandle(int);
    __declspec(dllimport) int __stdcall WriteConsoleW(intptr_t, const wchar_t*, uint32_t, void*, void*);
    __declspec(dllimport) int __stdcall MultiByteToWideChar(uint32_t, uint32_t, const char*, int, wchar_t*, int);
}
#endif
namespace std {

EXPORT
template <typename... T>
void println(std::format_string<T...> fmt, T&&... args) {
    auto _Text = std::format(fmt, std::forward<T>(args)...);
    _Text.push_back('\n');
#ifdef _WIN32
    constexpr auto CP_UTF8 = 65001;
    const int32_t _Required =
        ::MultiByteToWideChar(CP_UTF8, 0,
            _Text.data(), static_cast<int>(_Text.size()),
            nullptr, 0);
    std::wstring _WText(_Required, L'\0');
    ::MultiByteToWideChar(CP_UTF8, 0,
        _Text.data(), static_cast<int>(_Text.size()),
        _WText.data(), static_cast<int>(_WText.size()));
    ::WriteConsoleW(_get_osfhandle(_fileno(stdout)),
        _WText.data(), static_cast<uint32_t>(_WText.size()),
        nullptr, nullptr);
#else
    std::fwrite(_Text.data(), sizeof(char), _Text.size(), stdout);
#endif
}

} // namespace std
#endif

#ifndef __cpp_lib_generator
#include "generator.hpp"
#endif
