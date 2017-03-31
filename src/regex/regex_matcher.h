#ifndef __REGEX_MATCHER_H__
#define __REGEX_MATCHER_H__

#include <cassert>
#include <deque>
#include <iterator>
#include <set>

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
    add_to_closure(cur_closure_, regex_.nfa().start_id(), first,
                   match_results_type());
    do_match();
    results_.resize(regex_.mark_count());
  }

 private:
  BidirIt cur_;
  BidirIt last_;

  /*! \brief The matching candidates.
   *
   * Only the instructions of k_match_char_category and k_accept can be a
   * matching candidate.
   */
  struct candidate {
    int pc;
    match_results_type capture;
  };

  /*! \brief A e-closure of a NFA-state.
   */
  struct closure {
    /*! \brief The candidates of the next char matching.
     */
    std::deque<candidate> candidates;

    /*! \brief The included NFA states, including the candidates and the
     * passing-by instructions.
     */
    std::set<int> nfa_states;
  };

  const Regex& regex_;
  MatchResults& results_;
  closure cur_closure_;

  /*! \brief Recursively add the e closure of pc to c.
   */
  void add_to_closure(closure& c, int pc, iterator sp,
                      match_results_type&& capture) {
    if (c.nfa_states.find(pc) != c.nfa_states.end()) return;
    c.nfa_states.insert(pc);

    auto& insn = regex_.nfa().at(pc);
    switch (insn.opcode) {
      case k_match_char_category:
      case k_accept:
        c.candidates.push_back(candidate{pc, std::move(capture)});
        break;
      case k_goto:
      case k_advance:
        add_to_closure(c, insn.next, sp, std::move(capture));
        break;
      case k_fork:
        add_to_closure(c, insn.next, sp, match_results_type(capture));
        add_to_closure(c, insn.next2, sp, std::move(capture));
        break;
      case k_mark_group_start:
        capture.set_sub_start(insn.group_id, sp);
        add_to_closure(c, insn.next, sp, std::move(capture));
        break;
      case k_mark_group_end:
        capture.set_sub_end(insn.group_id, sp);
        add_to_closure(c, insn.next, sp, std::move(capture));
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
          if (cur_ != last_ && insn.cc.match(*cur_)) {
            add_to_closure(next_closure, insn.next, std::next(cur_),
                           std::move(cand.capture));
          }
          break;
        case k_accept:
          // Remove all the lower-priority candidates but keeps the higher
          // priority candidates.
          results_ = std::move(cand.capture);
          results_.set_ready();
          discard_others = true;
          break;
        default:
          assert(false);
      }
    }

    cur_closure_ = std::move(next_closure);
    ++cur_;
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
