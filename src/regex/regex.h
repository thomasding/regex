#ifndef __REGEX_H__
#define __REGEX_H__

#include <initializer_list>
#include <string>

#include "regex_nfa.h"
#include "regex_parser.h"
#include "regex_traits.h"

namespace regex {

template <class CharT, class Traits = regex_traits<CharT>>
class basic_regex {
 public:
  typedef CharT char_type;
  typedef std::basic_string<char_type> string_type;
  typedef nfa<instruction<char_type>, std::allocator<instruction<char_type>>>
      nfa_type;
  typedef regex_parser<char_type, char_category<char_type>, nfa_type>
      parser_type;
  typedef Traits traits_type;

  basic_regex() = default;
  explicit basic_regex(const CharT* s)
      : parser_(make_scanner(s, s + traits_type::length(s))),
        nfa_(std::move(parser_.nfa())) {}

  basic_regex(const CharT* s, std::size_t count)
      : parser_(make_scanner(s, s + count)),
        nfa_(std::move(parser_.nfa())) {}

  template <class ST, class SA>
  basic_regex(const std::basic_string<CharT, ST, SA>& str)
      : parser_(make_scanner(str.begin(), str.end())),
        nfa_(std::move(parser_.nfa())) {}

  template <class ForwardIt>
  basic_regex(ForwardIt first, ForwardIt last):
      parser_(make_scanner(first, last)),
      nfa_(std::move(parser_.nfa())) {}

  basic_regex(std::initializer_list<CharT> init):
      parser_(make_scanner(init.begin(), init.end())),
      nfa_(std::move(parser_.nfa())) {}

  std::locale getloc() const { return loc_; }

  std::locale imbue(const std::locale& loc) {
    using std::swap;
    swap(loc, loc_);
    return loc;
  }

  void swap(basic_regex& other) {
    using std::swap;
    swap(parser_. other.parser_);
    swap(nfa_, other.nfa_);
    swap(loc_, other.loc_);
  }

  nfa_type& nfa() { return nfa_; }

  const nfa_type& nfa() const { return nfa_; }

 private:
  parser_type parser_;
  nfa_type nfa_;
  std::locale loc_;

  template <class ForwardIt>
  static regex_scanner<ForwardIt> make_scanner(ForwardIt first,
                                               ForwardIt last) {
    return regex_scanner<ForwardIt>(first, last);
  }
};
}

#endif
