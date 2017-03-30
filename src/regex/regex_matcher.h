#ifndef __REGEX_MATCHER_H__
#define __REGEX_MATCHER_H__

#include <cassert>
#include <iterator>
#include <set>
#include <stack>

#include "regex_nfa.h"

namespace regex {

template <class Regex, class BidirIt, class MatchResults>
class regex_matcher {
 public:
  typedef Regex regex_type;
  typedef BidirIt iterator;
  typedef MatchResults match_results_type;
  typedef typename Regex::nfa_type::instruction_type instruction_type;

  regex_matcher(BidirIt first, BidirIt last, const Regex& regex,
                MatchResults& match_results)
      : cur_(first),
        last_(last),
        regex_(regex),
        results_(match_results),
        cur_closure_() {
    add_to_closure(cur_closure_, regex_.nfa().start_id(), match_results_type());
    do_match();
  }

 private:
  BidirIt cur_;
  BidirIt last_;

  struct candidate {
    int pc;
    match_results_type capture;
  };

  /*! \brief A e-closure of a NFA-state.
   */
  struct closure {
    /*! \brief The candidates of regex matching.
     */
    std::stack<candidate> candidates;
    /*! \brief The included NFA states.
     */
    std::set<int> nfa_states;
  };

  const Regex& regex_;
  MatchResults& results_;
  closure cur_closure_;

  /*! \brief Recursively add the e closure of pc to c.
   */
  void add_to_closure(closure& c, int pc, match_results_type&& capture) {
    if (c.nfa_states.find(pc) != c.nfa_states.end()) return;
    c.nfa_states.insert(pc);

    auto& insn = regex_.nfa().at(pc);
    switch (insn.opcode) {
      case k_match_char_category:
      case k_accept:
        c.candidates.emplace(pc, std::move(capture));
        break;
      case k_goto:
      case k_advance:
        add_to_closure(c, insn.next, std::move(capture));
        break;
      case k_fork:
        add_to_closure(c, insn.next, match_results_type(capture));
        add_to_closure(c, insn.next2, std::move(capture));
        break;
      case k_mark_group_start:
        capture.set_sub_start(insn.group_id, cur_);
        add_to_closure(c, insn.next, std::move(capture));
        break;
      case k_mark_group_end:
        capture.set_sub_end(insn.group_id, cur_);
        add_to_closure(c, insn.next, std::move(capture));
        break;
      default:
        assert(false);
    }
  }

  /*! \brief Match a character.
   */
  void advance() {
    closure next_closure;
    bool discard_others = false;
    for (auto& cand : cur_closure_.candidates) {
      if (discard_others) break;

      auto& insn = regex_.nfa().at(cand.pc);
      switch (insn.opcode) {
        case k_match_char_category:
          if (insn.cc.match(*cur_)) {
            add_to_closure(next_closure, insn.next, std::move(cand.capture));
          }
          break;
        case k_accept:
          // Remove all the lower-priority candidates but keeps the higher
          // priority candidates.
          results_ = std::move(cand.capture);
          results_.set_ready();
          discard_others = true;
          break;
      }
    }

    cur_closure_ = std::move(next_closure);
  }

  /*! \brief Match the string.
   */
  void do_match() {
    while (!cur_closure_.candidates.empty()) {
      advance();
    }
  }
};
}

#endif
