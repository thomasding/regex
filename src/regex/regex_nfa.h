#ifndef __REGEX_NFA_H__
#define __REGEX_NFA_H__

#include <cassert>
#include <vector>

#include "regex_char_category.h"

namespace regex {

/*! \brief The type of the instruction.
 */
enum opcode {
  /*! \brief Match a character.
   */
  k_match_char_category,

  /*! \brief Go to another instruction unconditionally.
   */
  k_goto,

  /*! \brief Fork a new thread at the current position.
   */
  k_fork,

  /*! \brief Accept the string.
   */
  k_accept,

  /*! \brief Go to another instruction only if the position in the string to be
   * matched has advanced.
   *
   * This instruction is used in the NFA to avoid infinite loops in the
   * backtracking implementation, like the NFA for a**.
   *
   * The each time the thread meets this instruction, it first check if it has
   * reached the same one before. If has, it goes to the next instruction only
   * if the position in the string has advanced and update the position in the
   * record of the thread. Otherwise, the thread quits. If not, it records the
   * current position in the string to be matched in the thread.
   */
  k_advance,
};

/*! \brief Constants for the next and next2 field of an instruction.
 */
enum next_constants {
  /*! \brief The next instruction is dangled.
   */
  k_dangled = -1,

  /*! \brief The next instruction does not exist.
   */
  k_null = -2
};

/*! \brief An instruction in the NFA.
 */
template <class Char>
struct instruction {
  typedef Char char_type;

  /*! \brief The opcode of the instruction.
   */
  opcode opcode;

  /*! \brief The character category.
   *
   * Used if opcode == k_match_char_category.
   */
  char_category<char_type> cc = {};

  /*! \brief The location to jump to.
   *
   * Used if opcode == k_match_char_category, k_goto, k_fork, or k_advance.
   */
  int next = k_null;

  /*! \brief The location of the new thread.
   *
   * Used if opcode == k_fork.
   */
  int next2 = k_null;
};

template <class Instruction, class Allocator>
class nfa : public std::vector<Instruction, Allocator> {
 public:
  typedef Instruction instruction_type;
  typedef typename Instruction::char_type char_type;
  typedef char_category<char_type> char_category_type;

  /*! \brief Use the parent constructors.
   */
  using std::vector<Instruction, Allocator>::vector;

  /*! \brief Set the start id of the nfa.
   */
  void set_start_id(int i) { start_id_ = i; }

  /*! \brief Get the start id of the nfa.
   */
  int start_id() const { return start_id_; }

  /*! \brief Append an instruction to match a character category and return the
   * instruction id.
   */
  int append_match_char_category(char_category_type cc, int next) {
    instruction_type insn{k_match_char_category};
    insn.cc = std::move(cc);
    insn.next = next;
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction to go to another position unconditionally and
   * return the instruction id.
   */
  int append_goto(int next) {
    assert(next >= 0 || next == k_dangled);

    instruction_type insn{k_goto};
    insn.next = next;
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction to fork and return its id.
   */
  int append_fork(int next1, int next2) {
    assert(next1 >= 0 || next1 == k_dangled);
    assert(next2 >= 0 || next2 == k_dangled);

    instruction_type insn{k_fork};
    insn.next = next1;
    insn.next2 = next2;
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction of reaching the accept state.
   */
  int append_accept() {
    instruction_type insn{k_accept};
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction of the advance flag.
   */
  int append_advance(int next) {
    instruction_type insn{k_advance};
    insn.next = next;
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Assert the NFA is complete.
   *
   * A complete NFA has no dangled or unreachable next positions.
   */
  void assert_complete() {
    int max_insn_id = int(this->size()) - 1;
    for (auto& insn : *this) {
      if (insn.opcode == k_match_char_category) {
        insn.cc.assert_not_empty();
        assert(insn.next >= 0 && insn.next <= max_insn_id);
      } else if (insn.opcode == k_goto) {
        assert(insn.next >= 0 && insn.next <= max_insn_id);
      } else if (insn.opcode == k_fork) {
        assert(insn.next >= 0 && insn.next <= max_insn_id);
        assert(insn.next2 >= 0 && insn.next2 <= max_insn_id);
      } else if (insn.opcode == k_advance) {
        assert(insn.next >= 0 && insn.next <= max_insn_id);
      } else if (insn.opcode == k_accept) {
        /* Empty */
      } else {
        assert(false);
      }
    }
  }

 private:
  int start_id_ = -1;
};
}

#endif
