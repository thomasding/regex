#ifndef __REGEX_TRAITS_H__
#define __REGEX_TRAITS_H__

#include <locale>
#include <utility>

namespace regex {

/*! \brief The type traits for the regex classes.
 */
template <class Char>
class regex_traits {
 public:
  typedef Char char_type;
  typedef std::ctype<char_type> ctype_type;
  typedef std::locale locale_type;

  /*! \brief Translate the equivalent characters into the same value in the
   * imbued locale.
   */
  char_type translate(char_type c) const { return c; }

  /*! \brief Translate the case-insensitive equivalent characters into the same
   * value in the imbued locale.
   */
  char_type translate_nocase(char_type c) const { return ctype_.to_lower(c); }

  /*! \brief Imbue a locale.
   */
  locale_type imbue(locale_type loc) {
    using std::swap;
    swap(loc, loc_);
    return loc;
  }

  /*! \brief Get the current locale.
   */
  locale_type getloc() const { return loc_; }

 private:
  locale_type loc_;
  ctype_type ctype_;
};
}

#endif
