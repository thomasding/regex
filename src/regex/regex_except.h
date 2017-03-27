#ifndef __REGEX_EXCEPT_H__
#define __REGEX_EXCEPT_H__

#include <exception>
#include <string>

namespace regex {

/*! \brief The regex errors.
 */
enum Error {
  kEscapeEOF,
  kEscapeBadChar,
  kMissingRightGroup,
  kMissingAtom,
  kUnexpectedToken,
};

/*! \brief The error class for regex.
 */
class RegexError final : public std::exception {
 public:
  RegexError(Error err, int pos) : what_() {
    if (pos >= 0)
      what_ = "regex error at " + std::to_string(pos) + ": ";
    else
      what_ = "regex error: ";

    switch (err) {
      case kEscapeEOF:
        what_ += "no character after backslash";
        break;
      case kEscapeBadChar:
        what_ += "character cannot be escaped";
        break;
      default:
        what_ += "unknown error";
        break;
    }
  }

  const char* what() const noexcept { return what_.c_str(); }

 private:
  std::string what_;
};
}

#if REGEX_ENABLE_EXCEPTION
#define REGEX_THROW(err, pos) throw ::regex::RegexError(err, pos)
#else
#define REGEX_THROW(err, pos) ::std::terminate()
#endif

#endif
