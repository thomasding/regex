#ifndef __REGEX_EXCEPT_H__
#define __REGEX_EXCEPT_H__

#include <exception>
#include <string>

namespace regex {

/*! \brief The regex errors.
 */
enum error_code {
  k_escape_eof,
  k_escape_bad_char,
  k_missing_right_group,
  k_missing_atom,
  k_unexpected_token,
};

/*! \brief The error class for regex.
 */
class regex_error final : public std::exception {
 public:
  regex_error(error_code err, int pos) : what_() {
    if (pos >= 0)
      what_ = "regex error at " + std::to_string(pos) + ": ";
    else
      what_ = "regex error: ";

    switch (err) {
      case k_escape_eof:
        what_ += "no character after backslash";
        break;
      case k_escape_bad_char:
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
#define regex_throw(err, pos) throw ::regex::regex_error(err, pos)
#else
#define regex_throw(err, pos) ::std::terminate()
#endif

#endif
