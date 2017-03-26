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
enum Token {
  kEOF,         //!< The scanner has reached the end
  kStar,        //!< Kleene-star "*" quantifier
  kPlus,        //!< Kleene-plus "+" quantifier
  kOptional,    //!< Optional "?" quantifier
  kOr,          //!< "|" operator
  kLeftGroup,   //!< "(" operator
  kRightGroup,  //!< ")" operator
  kCharacter,   //!< An ordinary character
};

/*! \brief The scanner of the regex.
 */
template <class Iterator_>
class RegexScanner {
 public:
  typedef Iterator_ IteratorType;
  typedef typename std::iterator_traits<IteratorType>::value_type CharType;
  typedef const std::ctype<CharType> CtypeType;
  typedef std::locale LocaleType;

  RegexScanner(IteratorType first, IteratorType last, const LocaleType& loc)
      : first_(first),
        last_(last),
        pos_(0),
        ctype_(std::use_facet<CtypeType>(loc)) {
    advance();
  }

  /*! \brief Return the current token.
   */
  Token cur_token() const noexcept { return cur_token_; }

  /*! \brief Return the current character.
   *
   * The current character is valid only if the current token is kCharacter.
   */
  CharType cur_char() const noexcept { return cur_char_; }

  /*! \brief Return the current position.
   */
  int cur_pos() const noexcept { return pos_; }

  /*! \brief Advance the scanner and get the next token.
   */
  void advance() {
    if (first_ == last_) {
      cur_token_ = kEOF;
    } else if (*first_ == ctype_.widen('*')) {
      cur_token_ = kStar;
      advance_char();
    } else if (*first_ == ctype_.widen('+')) {
      cur_token_ = kPlus;
      advance_char();
    } else if (*first_ == ctype_.widen('?')) {
      cur_token_ = kOptional;
      advance_char();
    } else if (*first_ == ctype_.widen('(')) {
      cur_token_ = kLeftGroup;
      advance_char();
    } else if (*first_ == ctype_.widen(')')) {
      cur_token_ = kRightGroup;
      advance_char();
    } else if (*first_ == ctype_.widen('|')) {
      cur_token_ = kOr;
      advance_char();
    } else if (*first_ == ctype_.widen('\\')) {
      eat_escape();
    } else {
      cur_token_ = kCharacter;
      cur_char_ = *first_;
      advance_char();
    }
  }

 private:
  IteratorType first_;
  IteratorType last_;
  int pos_;
  CtypeType& ctype_;

  Token cur_token_;
  CharType cur_char_;

  /*! \brief Eat the escaped character.
   *
   * Currently, the backslash turns a special character into an ordinary one.
   */
  void eat_escape() {
    assert(*first_ == ctype_.widen('\\'));
    advance_char();

    if (first_ == last_) {
      REGEX_THROW(kEscapeEOF, pos_);
    }

    auto c = *first_;
    if (c == ctype_.widen('*') || c == ctype_.widen('+') ||
        c == ctype_.widen('?') || c == ctype_.widen('(') ||
        c == ctype_.widen(')') || c == ctype_.widen('\\') ||
        c == ctype_.widen('|')) {
      cur_token_ = kCharacter;
      cur_char_ = c;
      advance_char();
    } else {
      REGEX_THROW(kEscapeBadChar, pos_);
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
