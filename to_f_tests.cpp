#include "to_f.h"
#include <cassert>
#include <iostream>
#include <memory>


void tests() {
#undef NDEBUG
  // basic test
  {
    int(*f)(char x, char y) = to_f([&](char x, char y) { return x + y; });
  }
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
  // non-copyable lambda
  {
    assert(to_f([x = std::unique_ptr<int>(new int(10))](int y) { return *x + y; })(
               100) == 110);
  }
  // mutable lambda
  {
    assert(to_f([x = std::unique_ptr<int>(new int(10))](int y) mutable {
             auto z = *x;
             x.reset();
             return z + y;
           })(100) == 110);
  }

  std::cout << "all passed\n";
}

int main() {
  std::cout << "- tests -\n";
  tests();
}
