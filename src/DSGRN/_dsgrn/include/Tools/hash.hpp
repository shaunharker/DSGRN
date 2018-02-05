/// hash.hpp
/// Shaun Harker
/// 2018-02-02

/// Add hash support

#pragma once

#include <utility>
#include <set>

namespace dsgrn {

  std::size_t hash_value ( std::size_t t );
  template <class A, class B> std::size_t hash_value(std::pair<A, B> const& v);
  template <class K, class C, class A> std::size_t hash_value ( std::set<K, C, A> const& p );
  template <typename T> void hash_combine (std::size_t & seed, const T& val);

  inline std::size_t
  hash_value ( std::size_t t ) {
    return std::hash<std::size_t>()(t);
  }

  template <class A, class B>
  std::size_t hash_value(std::pair<A, B> const& v) {
      std::size_t seed = 0;
      dsgrn::hash_combine(seed, v.first);
      dsgrn::hash_combine(seed, v.second);
      return seed;
  }

  template <class K, class C, class A>
  std::size_t
  hash_value ( std::set<K, C, A> const& p ) {
    std::size_t seed = 0;
    for ( auto const& item : p ) hash_combine( seed, item);
    return seed;
  }

  template <typename T>
  void hash_combine (std::size_t & seed, const T& val) {
    seed ^= hash_value(val) + 0x9e3779b9 // 7f4a7c16  // magic number hex digits of (\sqrt{5}-1/2)
                    + (seed<<6) + (seed>>2);
  }

  template <class T> 
  struct hash : public std::unary_function<T const&, std::size_t> {
    std::size_t operator()(T const& val) const {
      return hash_value(val);
    }
  };
}
