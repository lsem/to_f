#include <functional>
#include <type_traits>


namespace details {
template <class L, class R, class... Args> static auto impl_impl(L l) {
  static_assert(!std::is_same<L, std::function<R(Args...)>>::value,
                "Only lambdas are supported, it is unsafe to use "
                "std::function or other non-lambda callables");
  static L lambda_s = std::move(l);
  return +[](Args... args) -> R { return lambda_s(args...); };
}

template <class L>
struct to_f_impl : public to_f_impl<decltype(&L::operator())> {};
template <class ClassType, class R, class... Args>
struct to_f_impl<R (ClassType::*)(Args...) const> {
  template <class L> static auto impl(L l) {
    return impl_impl<L, R, Args...>(std::move(l));
  }
};
template <class ClassType, class R, class... Args>
struct to_f_impl<R (ClassType::*)(Args...)> {
  template <class L> static auto impl(L l) {

    return impl_impl<L, R, Args...>(std::move(l));
  }
};
} // namespace details

// Converts lambda to corresponding function address.
// Note, this also tend to work for other callable objects like std::function
// but it would be better if it didn't work because unlike lambdas
// std::function like objects do not generate unique type so inner template
// and its inner static will be reused for/shared by all functions with
// same signature which most probably is not what we want from it. I've
// specifically disallowed std::function but there are exist more which I don't
// know how to disallow in generic way.
template <class L> auto to_f(L l) {
  return details::to_f_impl<L>::impl(std::move(l));
}
