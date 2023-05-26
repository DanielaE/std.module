import std;

int main() {
    std::expected<int, bool> exp = 42;
    std::vector<std::string> vs;
    std::println("Hallöle! Clang 16 welcomes '{}' !", "import std;");
    return vs.empty() + *exp;
}