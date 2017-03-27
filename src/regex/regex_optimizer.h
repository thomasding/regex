#ifndef __REGEX_OPTIMIZER_H__
#define __REGEX_OPTIMIZER_H__

#include <cassert>

#include "regex_nfa.h"
#include "regex_except.h"

namespace regex {

/*! \brief This optimizer removes all the GOTO instructions.
 *
 * If the next instruction of Instruction A is GOTO B, its next instruction will
 * be replaced to B instead.
 */
template <class NFA, class Allocator>
class redundant_insn_optimizer {
 public:
  typedef NFA nfa_type;
  typedef Allocator allocator_type;

  /*! \brief Optimize the given NFA.
   */
  explicit redundant_insn_optimizer(nfa_type nfa)
      : nfa_(std::move(nfa)), next_(nfa_.size(), -1) {
    init_next();
    reduce_gotos();
    remove_gotos();
  }

  /*! \brief Optimize the given NFA.
   */
  explicit redundant_insn_optimizer(nfa_type nfa, const allocator_type& alloc)
      : nfa_(std::move(nfa)), next_(nfa_.size(), -1, alloc) {
    init_next();
    reduce_gotos();
    remove_gotos();
  }

  /*! \brief Get the optimized NFA.
   */
  const nfa_type& nfa() const { return nfa_; }

  /*! \brief Get the optimized NFA.
   */
  nfa_type& nfa() { return nfa_; }

  /*! \brief Get the number of removed GOTO instructions.
   */
  int removed_insn_count() const { return goto_insn_count_; }

 private:
  nfa_type nfa_;

  /*! \brief The next instruction of each GOTO instruction.
   *
   * The next instruction of instruction i is next_[i]. If instruction i is not
   * GOTO, next_[i] == -1.
   */
  std::vector<int, allocator_type> next_;

  int goto_insn_count_ = 0;

  /*! \brief Initialize next_.
   */
  void init_next() {
    assert(next_.size() == nfa_.size());

    for (size_t i = 0; i < nfa_.size(); ++i) {
      if (nfa_[i].opcode == k_goto) {
        next_[i] = nfa_[i].next;
        ++goto_insn_count_;
      }
    }
  }

  /*! \brief Reduce the consecutive GOTO instructions into one GOTO instruction.
   */
  void reduce_gotos() {
    assert(next_.size() == nfa_.size());

    bool changed;
    do {
      changed = false;

      for (int i = 0; i < int(next_.size()); ++i) {
        auto& ni = next_[i];
        if (ni != -1 && nfa_[ni].opcode == k_goto) {
          ni = next_[ni];
          changed = true;
          assert (ni != i);
        }
      }
    } while (changed);

    for (auto ni : next_) assert(nfa_[ni].opcode != k_goto);
  }

  /*! \brief Remove the GOTO instructions.
   */
  void remove_gotos() {
    assert(next_.size() == nfa_.size());

    auto replace = [&](int& ni) {
      if (ni >= 0 && next_[ni] != -1) ni = next_[ni];
    };

    for (auto& insn : nfa_) {
      replace(insn.next);
      replace(insn.next2);
    }
  }
};
}

#endif
