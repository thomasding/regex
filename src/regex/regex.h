#ifndef __REGEX_H__
#define __REGEX_H__

#include <initializer_list>
#include <string>

#include "regex_nfa.h"
#include "regex_parser.h"
#include "regex_scanner.h"
#include "regex_traits.h"

namespace regex {

enum match_flags {
  k_match_longest = 1 << 0,
};

template <class CharT, class Traits = regex_traits<CharT>>
class basic_regex {
 public:
  typedef CharT char_type;
  typedef char_category<CharT> char_category_type;
  typedef std::basic_string<char_type> string_type;
  typedef nfa<instruction<char_type>, std::allocator<instruction<char_type>>>
      nfa_type;
  typedef Traits traits_type;

  basic_regex() = default;
  explicit basic_regex(const CharT* s)
      : nfa_(make_nfa(s, s + traits_type::length(s))) {}

  basic_regex(const CharT* s, std::size_t count)
      : nfa_(make_nfa(s, s + count)) {}

  template <class ST, class SA>
  basic_regex(const std::basic_string<CharT, ST, SA>& str)
      : nfa_(make_nfa(str.begin(), str.end())) {}

  template <class ForwardIt>
  basic_regex(ForwardIt first, ForwardIt last) : nfa_(make_nfa(first, last)) {}

  basic_regex(std::initializer_list<CharT> init)
      : nfa_(make_nfa(init.begin(), init.end())) {}

  std::locale getloc() const { return loc_; }

  std::locale imbue(const std::locale& loc) {
    using std::swap;
    swap(loc, loc_);
    return loc;
  }

  void swap(basic_regex& other) {
    using std::swap;
    swap(nfa_, other.nfa_);
    swap(loc_, other.loc_);
  }

  nfa_type& nfa() { return nfa_; }

  const nfa_type& nfa() const { return nfa_; }

  unsigned mark_count() const { return nfa_.mark_count(); }

 private:
  nfa_type nfa_;
  std::locale loc_;

  template <class ForwardIt>
  static nfa_type make_nfa(ForwardIt first, ForwardIt last) {
    return regex_parser<regex_scanner<ForwardIt>, char_category_type, nfa_type>(
               regex_scanner<ForwardIt>(first, last, std::locale()))
        .nfa();
  }
};
}

#endif
