#pragma once

#ifdef HONEY_BADGER
#include <sol.hpp>
namespace v {
constexpr static const char *kHoneyBadgerScriptName = "honey_badger.lua";
class honey_badger {
 public:
  honey_badger();
  ~honey_badger() = default;

  void hbadger(const char *filename, int line, const char *module,
               const char *func);

  static honey_badger &
  get() {
    static thread_local honey_badger h;
    return h;
  }

 private:
  sol::state lua_;
};
}  // namespace v
#define HBADGER(module, func)                                                  \
  v::honey_badger::get().hbadger(__FILE__, __LINE__, #module, #func)
#else
namespace v {
struct dummy_badger {
  static void d();
};
}  // namespace v
#define HBADGER(module, func) ((void)0)
#endif
