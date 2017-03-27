#ifndef __REGEX_MATCH_RESULTS_H__
#define __REGEX_MATCH_RESULTS_H__

#include <iterator>
#include <string>
#include <vector>

namespace regex {

/*! \brief Denote sequences of chars matched by a sub-expression.
 */
template <class BidirIt>
class SubMatch {
 public:
  typedef BidirIt IteratorType;
  typedef typename std::iterator_traits<IteratorType>::value_type ValueType;
  typedef typename std::iterator_traits<IteratorType>::difference_type
      DifferenceType;
  typedef std::basic_string<ValueType> StringType;

  constexpr SubMatch() : first_(), last_() {}
  constexpr SubMatch(IteratorType first, IteratorType last)
      : first_(first), last_(last) {}

  /*! \brief Get the length of the sub-match.
   */
  DifferenceType length() const { return last_ - first_; }

  /*! \brief Get the matched string.
   */
  operator StringType() const { return StringType(first_, last_); }

  /*! \brief Get the matched string.
   */
  StringType str() const { return StringType(first_, last_); }

 private:
  IteratorType first_;
  IteratorType last_;
};

template <class BidirIt, class Allocator = std::allocator<SubMatch<BidirIt>>>
class MatchResults : public std::vector<BidirIt, Allocator> {
 public:
};
}

#endif
