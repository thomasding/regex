#ifndef __REGEX_FUNC_H__
#define __REGEX_FUNC_H__

#include "regex.h"
#include "regex_match_results.h"
#include "regex_matcher.h"

namespace regex {

template <class BidirIt, class Alloc, class CharT, class Traits>
bool regex_match(BidirIt first, BidirIt last, match_results<BidirIt, Alloc>& m,
                 const basic_regex<CharT, Traits>& e) {
  regex_matcher<basic_regex<CharT, Traits>, BidirIt,
                match_results<BidirIt, Alloc>>
      matcher(first, last, e, m);
  return m.ready();
}

template <class BidirIt, class Alloc, class CharT, class Traits>
bool regex_search(BidirIt first, BidirIt last, match_results<BidirIt, Alloc>& m,
                  const basic_regex<CharT, Traits>& e) {
  typedef char_category<CharT> CharCategory;
  // Transform the regex E to .*?E
  auto e_ = e;
  int any_char_id =
      e_.nfa().append_match_char_category(CharCategory::any_char(), k_dangled);
  int loop_id = e_.nfa().append_fork(e_.nfa().start_id(), any_char_id);
  e_.nfa()[any_char_id].next = loop_id;
  e_.nfa().set_start_id(loop_id);
  e_.nfa().assert_complete();

  regex_matcher<basic_regex<CharT, Traits>, BidirIt,
                match_results<BidirIt, Alloc>>
      matcher(first, last, e_, m);
  return m.ready();
}
}

#endif
