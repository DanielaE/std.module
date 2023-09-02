#include "allstd.hpp" // this is required to work by [std.modules]/4
#include <version>
import std;

#ifdef __GLIBCXX__
static constexpr auto lib = "libstdc++";
#else
static constexpr auto lib = "libc++";
#endif

int main() {
    std::expected<int, bool> exp = 42;
    const auto g = [] -> std::generator<std::string> {
        co_yield "Clang";
        co_yield "import std;";
    };
    std::vector<std::string> vs;
    for (auto && s : g()) {
        vs.push_back(std::move(s));
    }
    std::println("{} {} & {} welcome '{}' !", vs[0], __clang_major__, lib, vs[1]);
    return vs.empty() + *exp;
}