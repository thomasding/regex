#ifndef __REGEX_CHAR_CATEGORY_H__
#define __REGEX_CHAR_CATEGORY_H__

#include <cassert>

/*! \brief The type of the character category.
 */
enum category_type {
  k_cc_empty,        //!< Does not match any character
  k_cc_ordinary_char,  //!< A single character
  k_cc_any_char,     //!< Any character
};


namespace regex {

/*! \brief The character category.
 *
 * A character category can be a single character, a range of character or a
 * named category like [:num:].
 */
template <class Char>
class char_category {
 public:
  typedef Char char_type;

  /*! \brief Create a category of no character.
   */
  char_category() : type_(k_cc_empty) {}

  /*! \brief Make a char category that matches any character.
   */
  static char_category any_char() {
    char_category c;
    c.type_ = k_cc_any_char;
    return c;
  }

  static char_category ordinary_char(char_type ch) {
    char_category c;
    c.type_ = k_cc_ordinary_char;
    c.ch_ = ch;
    return c;
  }

  /*! \brief Return true if ch is in the category.
   */
  bool match(char_type ch) const {
    switch (type_) {
      case k_cc_ordinary_char:
        return ch == ch_;
      case k_cc_any_char:
        return true;
      default:
        assert(false);
    }
  }

  /*! \brief Assert the category is not empty.
   */
  void assert_not_empty() const { assert(type_ != k_cc_empty); }

  /*! \brief Return the char category type.
   */
  category_type type() const { return type_; }

  /*! \brief Return the ordinary character.
   */
  char_type ch() const { return ch_; }

 private:
  category_type type_;
  char_type ch_;
};
}

#endif
