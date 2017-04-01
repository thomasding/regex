#ifndef __REGEX_CHAR_CATEGORY_H__
#define __REGEX_CHAR_CATEGORY_H__

#include <cassert>

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

  /*! \brief The type of the character category.
   */
  enum category_type {
    k_empty,        //!< Does not match any character
    k_single_char,  //!< A single character
    k_any_char,     //!< Any character
  };

  /*! \brief Create a category of no character.
   */
  char_category() : type_(k_empty) {}

  /*! \brief Create a category of a single character.
   */
  explicit char_category(char_type ch) : type_(k_single_char), ch_(ch) {}

  /*! \brief Make a char category that matches any character.
   */
  static char_category any_char() {
    char_category c;
    c.type_ = k_any_char;
    return c;
  }

  /*! \brief Return true if ch is in the category.
   */
  bool match(char_type ch) const {
    switch (type_) {
      case k_single_char:
        return ch == ch_;
      case k_any_char:
        return true;
      default:
        assert(false);
    }
  }

  /*! \brief Assert the category is not empty.
   */
  void assert_not_empty() const { assert(type_ != k_empty); }

 private:
  category_type type_;
  char_type ch_;
};
}

#endif
