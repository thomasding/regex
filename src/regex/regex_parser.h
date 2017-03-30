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
template <class Scanner, class CharCategory, class NFA>
class regex_parser {
 public:
  typedef Scanner scanner_type;
  typedef typename scanner_type::char_type char_type;
  typedef CharCategory char_category_type;
  typedef NFA nfa_type;
  typedef typename nfa_type::allocator_type allocator_type;

  /*! \brief Parse a regex.
   */
  explicit regex_parser(scanner_type s) : scanner_(std::move(s)) {
    parse_regex();
  }

  /*! \brief Parse a regex.
   */
  explicit regex_parser(scanner_type s, const allocator_type& alloc)
      : scanner_(std::move(s)), nfa_(alloc) {
    parse_regex();
  }

  /*! \brief Get the NFA.
   */
  nfa_type& nfa() { return nfa_; }

  /*! \brief Get the NFA.
   */
  const nfa_type& nfa() const { return nfa_; }

 private:
  /*! \brief A fragment of the uncompleted NFA.
   */
  struct fragment {
    int start;         //!< The start instruction id
    int end;           //!< The end instruction id
    bool maybe_empty;  //!< May match empty string.
  };

  scanner_type scanner_;
  nfa_type nfa_;

  /*! \brief Parse nonterminal Regex.
   */
  void parse_regex() {
    fragment f = parse_sub();
    if (scanner_.cur_token() != k_eof) {
      regex_throw(k_unexpected_token, scanner_.cur_pos());
    }

    int sid = nfa_.append_accept();
    link_dangled_pointer(f.end, sid);
    nfa_.set_start_id(f.start);
    nfa_.assert_complete();
  }

  /*! \brief Parse nonterminal Sub and RestSub.
   */
  fragment parse_sub() {
    unsigned group_id = nfa_.alloc_group_id();
    fragment prev = parse_seq();

    while (scanner_.cur_token() == k_or) {
      scanner_.advance();

      fragment seq2 = parse_seq();

      int start = nfa_.append_fork(prev.start, seq2.start);
      int end = nfa_.append_goto(k_dangled);

      link_dangled_pointer(prev.end, end);
      link_dangled_pointer(seq2.end, end);

      prev.start = start;
      prev.end = end;
      prev.maybe_empty = prev.maybe_empty || seq2.maybe_empty;
    }

    int group_start = nfa_.append_mark_group_start(prev.start, group_id);
    int group_end = nfa_.append_mark_group_end(k_dangled, group_id);
    link_dangled_pointer(prev.end, group_end);

    return {group_start, group_end, prev.maybe_empty};
  }

  /*! \brief Parse nonterminal Seq and RestSeq.
   */
  fragment parse_seq() {
    if (is_atom_head()) {
      fragment prev = parse_term();
      while (is_atom_head()) {
        fragment term2 = parse_term();
        link_dangled_pointer(prev.end, term2.start);
        prev.end = term2.end;
        prev.maybe_empty = prev.maybe_empty && term2.maybe_empty;
      }
      return prev;
    } else {
      int sid = nfa_.append_goto(k_dangled);
      return {sid, sid, true};
    }
  }

  /*! \brief Parse nonterminal Term and RestTerm.
   */
  fragment parse_term() {
    fragment prev = parse_atom();

    // The following is the parser for RestTerm.
    while (true) {
      if (scanner_.cur_token() == k_star) {
        prev = parse_star(prev);
      } else if (scanner_.cur_token() == k_plus) {
        prev = parse_plus(prev);
      } else if (scanner_.cur_token() == k_optional) {
        prev = parse_optional(prev);
      } else {
        break;
      }
    }

    return prev;
  }

  /*! \brief Parse star quantifier.
   */
  fragment parse_star(fragment cur_frag) {
    scanner_.advance();

    int start = cur_frag.start;
    if (cur_frag.maybe_empty) {
      start = nfa_.append_advance(cur_frag.start);
    }

    int loop = nfa_.append_fork(start, k_dangled);
    link_dangled_pointer(cur_frag.end, loop);

    return {loop, loop, true};
  }

  /*! \brief Parse plus quantifier.
   */
  fragment parse_plus(fragment cur_frag) {
    scanner_.advance();

    int start = cur_frag.start;
    if (cur_frag.maybe_empty) {
      start = nfa_.append_advance(cur_frag.start);
    }

    int loop = nfa_.append_fork(start, k_dangled);
    link_dangled_pointer(cur_frag.end, loop);

    return {start, loop, cur_frag.maybe_empty};
  }

  /*! \brief Parse optional quantifier.
   */
  fragment parse_optional(fragment cur_frag) {
    scanner_.advance();

    int mergenode = nfa_.append_goto(k_dangled);
    int forknode = nfa_.append_fork(cur_frag.start, mergenode);
    link_dangled_pointer(cur_frag.end, mergenode);

    return {forknode, mergenode, true};
  }

  /*! \brief Parse nonterminal Atom.
   */
  fragment parse_atom() {
    if (scanner_.cur_token() == k_character) {
      int sid = nfa_.append_match_char_category(
          char_category_type(scanner_.cur_char()), k_dangled);
      scanner_.advance();
      return {sid, sid, false};
    } else if (scanner_.cur_token() == k_left_group) {
      scanner_.advance();
      fragment s = parse_sub();
      if (scanner_.cur_token() != k_right_group) {
        regex_throw(k_missing_right_group, scanner_.cur_pos());
      }
      scanner_.advance();
      return s;
    } else {
      regex_throw(k_missing_atom, scanner_.cur_pos());
    }
  }

  /*! \brief Link dangled pointers of nfa_[end] to next.
   */
  void link_dangled_pointer(int end, int next) {
    auto& insn = nfa_.at(end);
    if (insn.next == k_dangled) insn.next = next;
    if (insn.next2 == k_dangled) insn.next2 = next;
  }

  /*! \brief Return true of the lookahead token is the head of an Atom.
   */
  bool is_atom_head() {
    auto t = scanner_.cur_token();
    return t == k_character || t == k_left_group;
  }
};
}

#endif
