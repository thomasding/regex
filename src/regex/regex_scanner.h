#ifndef __REGEX_SCANNER_H__
#define __REGEX_SCANNER_H__

#include <cassert>
#include <iterator>
#include <locale>

#include "regex_char_category.h"
#include "regex_except.h"

namespace regex {

/*! \brief The type of the tokens.
 */
enum token {
  k_eof,            //!< The scanner has reached the end
  k_star,           //!< Kleene-star "*" quantifier
  k_plus,           //!< Kleene-plus "+" quantifier
  k_optional,       //!< Optional "?" quantifier
  k_or,             //!< "|" operator
  k_left_group,     //!< "(" operator
  k_right_group,    //!< ")" operator
  k_character,      //!< An ordinary character
  k_left_bracket,   //!< "[" operator
  k_right_bracket,  //!< "]" operator
  k_negation,       //!< "^" as the first character after a left bracket
  k_char_class,     //!< a character class
};

/*! \brief Character classes.
 */
enum character_class {
  k_cc_alnum,     //!< [:alnum:] = [A-Za-z0-9]
  k_cc_word,      //!< [:word] = \w = [A-Za-z0-9_]
  k_cc_not_word,  //!< \W = [^A-Za-z0-9_]
  k_cc_
};

/*! \brief The scanner of the regex.
 */
template <class Iterator>
class regex_scanner {
 public:
  typedef Iterator iterator;
  typedef typename std::iterator_traits<iterator>::value_type char_type;
  typedef const std::ctype<char_type> ctype_type;
  typedef std::locale locale_type;
  typedef char_category<char_type> char_category_type;

  regex_scanner(iterator first, iterator last, const locale_type& loc)
      : first_(first),
        last_(last),
        pos_(0),
        ctype_(std::use_facet<ctype_type>(loc)) {
    advance();
  }

  /*! \brief Return the current token.
   */
  token cur_token() const noexcept { return cur_token_; }

  /*! \brief Return the current character category.
   *
   * The current character category is valid only if the current token is
   * k_character.
   */
  char_category_type cur_cc() const noexcept { return cur_cc_; }

  /*! \brief Return the current position.
   */
  int cur_pos() const noexcept { return pos_; }

  /*! \brief Advance the scanner and get the next token.
   */
  void advance() {
    if (first_ == last_) {
      cur_token_ = k_eof;
    } else if (*first_ == ctype_.widen('*')) {
      cur_token_ = k_star;
      advance_char();
    } else if (*first_ == ctype_.widen('+')) {
      cur_token_ = k_plus;
      advance_char();
    } else if (*first_ == ctype_.widen('?')) {
      cur_token_ = k_optional;
      advance_char();
    } else if (*first_ == ctype_.widen('(')) {
      cur_token_ = k_left_group;
      advance_char();
    } else if (*first_ == ctype_.widen(')')) {
      cur_token_ = k_right_group;
      advance_char();
    } else if (*first_ == ctype_.widen('|')) {
      cur_token_ = k_or;
      advance_char();
    } else if (*first_ == ctype_.widen('\\')) {
      eat_escape();
    } else {
      cur_token_ = k_character;
      cur_cc_ = char_category_type::ordinary_char(*first_);
      advance_char();
    }
  }

 private:
  iterator first_;
  iterator last_;
  int pos_;
  ctype_type& ctype_;

  token cur_token_;
  char_category_type cur_cc_;

  /*! \brief Eat the escaped character.
   *
   * Currently, the backslash turns a special character into an ordinary one.
   */
  void eat_escape() {
    assert(*first_ == ctype_.widen('\\'));
    advance_char();

    if (first_ == last_) {
      regex_throw(k_escape_eof, pos_);
    }

    auto c = *first_;
    if (c == ctype_.widen('*') || c == ctype_.widen('+') ||
        c == ctype_.widen('?') || c == ctype_.widen('(') ||
        c == ctype_.widen(')') || c == ctype_.widen('\\') ||
        c == ctype_.widen('|')) {
      cur_token_ = k_character;
      cur_cc_ = char_category_type::ordinary_char(c);
      advance_char();
    } else {
      regex_throw(k_escape_bad_char, pos_);
    }
  }

  /*! \brief Advance a character.
   */
  void advance_char() {
    ++first_;
    ++pos_;
  }
};
}

#endif
