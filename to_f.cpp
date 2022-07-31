#include <cassert>
#include <iostream>

using namespace std;

template <class F> struct to_f_impl : public to_f_impl<decltype(&F::operator())> {};
template <class ClassType, class R, class... Args>
struct to_f_impl<R (ClassType::*)(Args...) const> {
    template <class L> static auto impl(L l) {
        static L lambda_s = std::move(l);
        return +[](Args... args) -> R { return lambda_s(args...); };
    }
};

// Converts lambda to function pointer. Works per lambda type, if lambda is reused,
// previous address gets invalidated.
template <class L> auto to_f(L l) { return to_f_impl<L>::impl(std::move(l)); }

void tests() {
#undef NDEBUG
    // no args
    {
        bool called = false;
        to_f([&]() { called = true; })();
        assert(called);
    }
    // r-value args
    {
        bool called = false;
        to_f([&](int x, std::string y) {
            called = true;
            assert(x == 10);
            assert(y == "test");
        })(10, "test");
        assert(called);
    }
    // l-value args
    {
        bool called = false;
        int x = 20;
        to_f([&](int &x, std::string y) {
            called = true;
            assert(x == 20);
            assert(y == "test");
            x = 100;
        })(x, "test");
        assert(called);
        assert(x == 100);
    }

    // l-value const args
    {
        bool called = false;
        to_f([&](const int &x, std::string y) {
            called = true;
            assert(x == 20);
            assert(y == "test");
        })(20, "test");
        assert(called);
    }
    // C-style out param
    {
        bool result = false;
        to_f([&](bool *result) { *result = true; })(&result);
        assert(result);
    }
    std::cout << "all passed\n";
}

int main() {
    std::cout << "- tests -\n";
    tests();
}
