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
namespace std {

EXPORT
template <typename... T>
void println(std::format_string<T...> fmt, T&&... args) {
  auto text = std::format(fmt, std::forward<T>(args)...);
  text.push_back('\n');
  std::fwrite(text.data(), sizeof(char), text.size(), stdout);
}

} // namespace std
#endif

#ifndef __cpp_lib_generator
#include "generator.hpp"
#endif
