// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
#ifndef MEASUREMENT_KIT_COMMON_DELEGATE_HPP
#define MEASUREMENT_KIT_COMMON_DELEGATE_HPP

#include <cstddef>
#include <functional>

namespace mk {

template <typename T> class Delegate {
  public:
    Delegate() {}
    template <typename F> Delegate(F f) : func(f) {}
    Delegate(std::function<T> f) : func(f) {}

    ~Delegate() {}

    void operator=(std::function<T> f) { func = f; }
    template <typename F> void operator=(F f) { func = f; }
    void operator=(std::nullptr_t f) { func = f; }

    // not implementing swap and assign

    operator bool() { return static_cast<bool>(func); }

    template <typename... Args> void operator()(Args &&... args) {
        // Make sure the original closure is not destroyed before end of scope
        auto orig = this->func;
        orig(std::forward<Args>(args)...);
    }

  protected:
    std::function<T> func;
};

} // namespace
#endif