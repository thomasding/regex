#ifndef __REGEX_NFA_H__
#define __REGEX_NFA_H__

#include <cassert>
#include <vector>

#include "regex_char_category.h"

namespace regex {

/*! \brief The type of the instruction.
 */
enum Opcode {
  /*! \brief Match a character.
   */
  kMatchCharCategory,

  /*! \brief Go to another instruction unconditionally.
   */
  kGoto,

  /*! \brief Fork a new thread at the current position.
   */
  kFork,

  /*! \brief Accept the string.
   */
  kAccept,

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
  kAdvance,
};

/*! \brief Constants for the next and next2 field of an instruction.
 */
enum NextConstants {
  /*! \brief The next instruction is dangled.
   */
  kDangled = -1,

  /*! \brief The next instruction does not exist.
   */
  kNull = -2
};

/*! \brief An instruction in the NFA.
 */
template <class Char_>
struct Instruction {
  typedef Char_ CharType;

  /*! \brief The opcode of the instruction.
   */
  Opcode opcode;

  /*! \brief The character category.
   *
   * Used if opcode == kMatchCharCategory.
   */
  CharCategory<CharType> cc = {};

  /*! \brief The location to jump to.
   *
   * Used if opcode == kMatchCharCategory, kGoto, kFork, or kAdvance.
   */
  int next = kNull;

  /*! \brief The location of the new thread.
   *
   * Used if opcode == kFork.
   */
  int next2 = kNull;
};

template <class Instruction_, class Allocator_>
class NFA : public std::vector<Instruction_, Allocator_> {
 public:
  typedef Instruction_ InstructionType;
  typedef typename InstructionType::CharType CharType;
  typedef CharCategory<CharType> CharCategoryType;
  typedef Allocator_ AllocatorType;

  /*! \brief Use the parent constructors.
   */
  using std::vector<Instruction_, Allocator_>::vector;

  /*! \brief Set the start id of the nfa.
   */
  void set_start_id(int i) { start_id_ = i; }

  /*! \brief Get the start id of the nfa.
   */
  int start_id() const { return start_id_; }

  /*! \brief Append an instruction to match a character category and return the
   * instruction id.
   */
  int append_match_char_category(CharCategoryType cc, int next) {
    InstructionType insn{kMatchCharCategory};
    insn.cc = std::move(cc);
    insn.next = next;
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction to go to another position unconditionally and
   * return the instruction id.
   */
  int append_goto(int next) {
    assert(next >= 0 || next == kDangled);

    InstructionType insn{kGoto};
    insn.next = next;
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction to fork and return its id.
   */
  int append_fork(int next1, int next2) {
    assert(next1 >= 0 || next1 == kDangled);
    assert(next2 >= 0 || next2 == kDangled);

    InstructionType insn{kFork};
    insn.next = next1;
    insn.next2 = next2;
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction of reaching the accept state.
   */
  int append_accept() {
    InstructionType insn{kAccept};
    this->push_back(insn);
    return this->size() - 1;
  }

  /*! \brief Append an instruction of the advance flag.
   */
  int append_advance(int next) {
    InstructionType insn{kAdvance};
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
      if (insn.opcode == kMatchCharCategory) {
        insn.cc.assert_not_empty();
        assert(insn.next >= 0 && insn.next <= max_insn_id);
      } else if (insn.opcode == kGoto) {
        assert(insn.next >= 0 && insn.next <= max_insn_id);
      } else if (insn.opcode == kFork) {
        assert(insn.next >= 0 && insn.next <= max_insn_id);
        assert(insn.next2 >= 0 && insn.next2 <= max_insn_id);
      } else if (insn.opcode == kAdvance) {
        assert(insn.next >= 0 && insn.next <= max_insn_id);
      } else if (insn.opcode == kAccept) {
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
