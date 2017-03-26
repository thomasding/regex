#ifndef __REGEX_CHAR_CATEGORY_H__
#define __REGEX_CHAR_CATEGORY_H__

#include <cassert>

namespace regex {

/*! \brief The character category.
 *
 * A character category can be a single character, a range of character or a
 * named category like [:num:].
 */
template <class Char_>
class CharCategory {
 public:
  typedef Char_ CharType;

  /*! \brief The type of the character category.
   */
  enum CategoryType {
    kEmpty,       //!< Does not match any character
    kSingleChar,  //!< A single character
  };

  /*! \brief Create a category of no character.
   */
  CharCategory() : type_(kEmpty) {}

  /*! \brief Create a category of a single character.
   */
  explicit CharCategory(CharType ch) : type_(kSingleChar), ch_(ch) {}

  /*! \brief Return true if ch is in the category.
   */
  bool match(CharType ch) const {
    if (type_ == kSingleChar) {
      return ch == ch_;
    } if (type_ == kEmpty) {
      assert(false);
    } else {
      assert(false);
    }
  }

  /*! \brief Assert the category is not empty.
   */
  void assert_not_empty() const {
    assert(type_ != kEmpty);
  }

 private:
  CategoryType type_;
  CharType ch_;
};
}

#endif
