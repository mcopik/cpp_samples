#ifndef CPPSPRINTF_HPP
#define CPPSPRINTF_HPP

#include <string>
#include <cstring>
#include <memory>
#include <type_traits>


#if defined(HAVE_CXX17_OPTIONAL)
  #include <optional>
  #define OPTIONAL std::optional
#elif defined(HAVE_LLVM_OPTIONAL)
  #include <llvm/ADT/Optional.h>
  #define OPTIONAL llvm::Optional
#endif

#define HAVE_OPTIONAL (defined(HAVE_CXX17_OPTIONAL) || defined(HAVE_LLVM_OPTIONAL))

namespace {

  const char * to_str(std::string && t)
  {
    return t.c_str();
  }

  const char * to_str(const std::string & t)
  {
    return t.c_str();
  }

#if defined(HAVE_LLVM_OPTIONAL)
  // result_of won't work since it requires function to be a type
  template<typename T>
  auto to_str(const llvm::Optional<T> & t) -> decltype( to_str(std::declval<T&>()) )
  {
    return to_str(t.getValue());
  }
#elif defined(HAVE_CXX17_OPTIONAL)
  // result_of won't work since it requires function to be a type
  template<typename T>
  auto to_str(const std::optional<T> & t) -> decltype( to_str(std::declval<T&>()) )
  {
    return to_str(t.value());
  }
#endif

  // universal reference here would be always selected, including std::string
  template<typename T>
  T to_str(const T & t)
  {
    return t;
  }

  template<typename T>
  bool has_value(const T & t)
  {
    return true;
  }

#if defined(HAVE_LLVM_OPTIONAL)
  template<typename T>
  bool has_value(const llvm::Optional<T> & t)
  {
    return t.hasValue();
  }
#elif defined(HAVE_CXX17_OPTIONAL)
  template<typename T>
  bool has_value(const std::optional<T> & t)
  {
    return t.has_value();
  }
#endif

  bool all_true()
  {
    return true;
  }

  template<typename Arg, typename... Args>
  bool all_true(const Arg & a, Args &&... args)
  {
   return has_value(a) && all_true(args...);
  }

  // Is an object of type A
  template <typename T, template<typename...> class A>
  struct is_instance_of: std::false_type{};

  template <template <typename...> class A, typename... T>
  struct is_instance_of<A<T...>, A> : std::true_type{};

#if HAVE_OPTIONAL
  template<typename Arg, typename... Args>
  struct contains_optional
  {
    static constexpr bool value =
        is_instance_of<Arg, OPTIONAL>::value ||
        contains_optional<Args...>::value;
  };

  template<typename Arg>
  struct contains_optional<Arg>
  {
    static constexpr bool value = is_instance_of<Arg, OPTIONAL>::value;
  };
#endif
  template<typename ... Args>
  std::string cppsprintf_impl(const std::string& format, Args &&... args)
  {
    size_t size = snprintf( nullptr, 0, format.c_str(), to_str(args)...) + 1;
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), to_str(args)...);
    return std::string(buf.get(), buf.get() + size - 1);
  }
}

// FIXME: single function with C++17
#if HAVE_OPTIONAL
template<typename ... Args>
auto cppsprintf(const std::string& format, Args &&... args)
  -> typename std::enable_if< contains_optional<Args...>::value, OPTIONAL<std::string>>::type
{
  if(!all_true(args...))
    return OPTIONAL<std::string>();
  return cppsprintf_impl(format, std::forward<Args>(args)...);
}
#endif

template<typename ... Args>
auto cppsprintf(const std::string& format, Args &&... args)
#if HAVE_OPTIONAL
  -> typename std::enable_if< !contains_optional<Args...>::value, std::string >::type
#else
  -> std::string
#endif
{
  return cppsprintf_impl(format, std::forward<Args>(args)...);
}

#endif

