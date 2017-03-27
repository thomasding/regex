#ifndef __REGEX_PARSER_H__
#define __REGEX_PARSER_H__

#include "regex_nfa.h"
#include "regex_scanner.h"

namespace regex {

/*! \brief Parse the regex into a nfa.
 *
 * The parser is written in a recursive-descendent way.
 *
 * The start symbol is Regex, the nonterminal symbols start with a capital
 * letter and the terminal symbols are quoted with angle brackets. The
 * empty string is represented with '<>'.
 *
 * Regex    ::= Sub
 *
 * Sub      ::= Seq RestSub
 *
 * RestSub  ::= <kOr> Seq RestSub
 *           |  <>
 *
 * Seq      ::= Term RestSeq
 *           |  <>
 *
 * RestSeq  ::= Term RestSeq
 *           |  <>
 *
 * Term     ::= Atom RestTerm
 *
 * RestTerm ::= Quantifier RestTerm
 *           |  <>
 *
 * Atom     ::= <kCharacter>
 *           |  <kLeftGroup> Sub <kRightGroup>
 *
 * Quantifier  ::= <kStar>
 *              |  <kPlus>
 *              |  <kOptional>
 *
 * The parser functions are named with the snake case of the corresponding
 * nonterminal syntax.
 */
template <class Scanner_, class CharCategory_, class NFA_>
class RegexParser {
 public:
  typedef Scanner_ ScannerType;
  typedef typename Scanner_::CharType CharType;
  typedef CharCategory_ CharCategoryType;
  typedef NFA_ NFAType;
  typedef typename NFAType::AllocatorType AllocatorType;

  /*! \brief Parse a regex.
   */
  explicit RegexParser(ScannerType s) : scanner_(std::move(s)) {
    parse_regex();
  }

  /*! \brief Parse a regex.
   */
  explicit RegexParser(ScannerType s, const AllocatorType& alloc)
      : scanner_(std::move(s)), nfa_(alloc) {
    parse_regex();
  }

  /*! \brief Get the NFA.
   */
  NFAType& nfa() { return nfa_; }

  /*! \brief Get the NFA.
   */
  const NFAType& nfa() const { return nfa_; }

 private:
  /*! \brief A fragment of the uncompleted NFA.
   */
  struct Fragment {
    int start;         //!< The start instruction id
    int end;           //!< The end instruction id
    bool maybe_empty;  //!< May match empty string.
  };

  ScannerType scanner_;
  NFAType nfa_;

  /*! \brief Parse nonterminal Regex.
   */
  void parse_regex() {
    Fragment f = parse_sub();
    if (scanner_.cur_token() != kEOF) {
      REGEX_THROW(kUnexpectedToken, scanner_.cur_pos());
    }

    int sid = nfa_.append_accept();
    link_dangled_pointer(f.end, sid);
    nfa_.set_start_id(f.start);
    nfa_.assert_complete();
  }

  /*! \brief Parse nonterminal Sub and RestSub.
   */
  Fragment parse_sub() {
    Fragment prev = parse_seq();

    while (scanner_.cur_token() == kOr) {
      scanner_.advance();

      Fragment seq2 = parse_seq();

      int start = nfa_.append_fork(prev.start, seq2.start);
      int end = nfa_.append_goto(kDangled);

      link_dangled_pointer(prev.end, end);
      link_dangled_pointer(seq2.end, end);

      prev.start = start;
      prev.end = end;
      prev.maybe_empty = prev.maybe_empty || seq2.maybe_empty;
    }

    return prev;
  }

  /*! \brief Parse nonterminal Seq and RestSeq.
   */
  Fragment parse_seq() {
    if (is_atom_head()) {
      Fragment prev = parse_term();
      while (is_atom_head()) {
        Fragment term2 = parse_term();
        link_dangled_pointer(prev.end, term2.start);
        prev.end = term2.end;
        prev.maybe_empty = prev.maybe_empty && term2.maybe_empty;
      }
      return prev;
    } else {
      int sid = nfa_.append_goto(kDangled);
      return {sid, sid, true};
    }
  }

  /*! \brief Parse nonterminal Term and RestTerm.
   */
  Fragment parse_term() {
    Fragment prev = parse_atom();

    // The following is the parser for RestTerm.
    while (true) {
      if (scanner_.cur_token() == kStar) {
        prev = parse_star(prev);
      } else if (scanner_.cur_token() == kPlus) {
        prev = parse_plus(prev);
      } else if (scanner_.cur_token() == kOptional) {
        prev = parse_optional(prev);
      } else {
        break;
      }
    }

    return prev;
  }

  /*! \brief Parse star quantifier.
   */
  Fragment parse_star(Fragment cur_frag) {
    scanner_.advance();

    int start = cur_frag.start;
    if (cur_frag.maybe_empty) {
      start = nfa_.append_advance(cur_frag.start);
    }

    int loop = nfa_.append_fork(start, kDangled);
    link_dangled_pointer(cur_frag.end, loop);

    return {loop, loop, true};
  }

  /*! \brief Parse plus quantifier.
   */
  Fragment parse_plus(Fragment cur_frag) {
    scanner_.advance();

    int start = cur_frag.start;
    if (cur_frag.maybe_empty) {
      start = nfa_.append_advance(cur_frag.start);
    }

    int loop = nfa_.append_fork(start, kDangled);
    link_dangled_pointer(cur_frag.end, loop);

    return {start, loop, cur_frag.maybe_empty};
  }

  /*! \brief Parse optional quantifier.
   */
  Fragment parse_optional(Fragment cur_frag) {
    scanner_.advance();

    int mergenode = nfa_.append_goto(kDangled);
    int forknode = nfa_.append_fork(cur_frag.start, mergenode);
    link_dangled_pointer(cur_frag.end, mergenode);

    return {forknode, mergenode, true};
  }

  /*! \brief Parse nonterminal Atom.
   */
  Fragment parse_atom() {
    if (scanner_.cur_token() == kCharacter) {
      int sid = nfa_.append_match_char_category(
          CharCategoryType(scanner_.cur_char()), kDangled);
      scanner_.advance();
      return {sid, sid, false};
    } else if (scanner_.cur_token() == kLeftGroup) {
      scanner_.advance();
      Fragment s = parse_sub();
      if (scanner_.cur_token() != kRightGroup) {
        REGEX_THROW(kMissingRightGroup, scanner_.cur_pos());
      }
      scanner_.advance();
      return s;
    } else {
      REGEX_THROW(kMissingAtom, scanner_.cur_pos());
    }
  }

  /*! \brief Link dangled pointers of nfa_[end] to next.
   */
  void link_dangled_pointer(int end, int next) {
    auto& insn = nfa_.at(end);
    if (insn.next == kDangled) insn.next = next;
    if (insn.next2 == kDangled) insn.next2 = next;
  }

  /*! \brief Return true of the lookahead token is the head of an Atom.
   */
  bool is_atom_head() {
    auto t = scanner_.cur_token();
    return t == kCharacter || t == kLeftGroup;
  }
};
}

#endif
