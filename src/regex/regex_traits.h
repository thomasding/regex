#ifndef __REGEX_TRAITS_H__
#define __REGEX_TRAITS_H__

#include <locale>
#include <utility>

namespace regex {

/*! \brief The type traits for the regex classes.
 */
template <class Char_>
class RegexTraits {
 public:
  typedef Char_ CharType;
  typedef std::ctype<CharType> CtypeType;
  typedef std::locale LocaleType;

  /*! \brief Translate the equivalent characters into the same value in the
   * imbued locale.
   */
  CharType translate(CharType c) const { return c; }

  /*! \brief Translate the case-insensitive equivalent characters into the same
   * value in the imbued locale.
   */
  CharType translate_nocase(CharType c) const { return ctype_.to_lower(c); }

  /*! \brief Imbue a locale.
   */
  LocaleType imbue(LocaleType loc) {
    using std::swap;
    swap(loc, loc_);
    return loc;
  }

  /*! \brief Get the current locale.
   */
  LocaleType getloc() const { return loc_; }

 private:
  LocaleType loc_;
  CtypeType ctype_;
};
}

#endif
