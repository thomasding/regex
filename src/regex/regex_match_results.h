#ifndef __REGEX_MATCH_RESULTS_H__
#define __REGEX_MATCH_RESULTS_H__

#include <iterator>
#include <string>
#include <vector>

namespace regex {

/*! \brief Denote sequences of chars matched by a sub-expression.
 */
template <class BidirIt>
class sub_match {
 public:
  typedef BidirIt iterator;
  typedef typename std::iterator_traits<iterator>::value_type value_type;
  typedef
      typename std::iterator_traits<iterator>::difference_type difference_type;
  typedef std::basic_string<value_type> string_type;

  constexpr sub_match() : first_(), last_(), matched_(false) {}

  /*! \brief Get the length of the sub-match.
   */
  difference_type length() const {
    if (matched_)
      return last_ - first_;
    else
      return 0;
  }

  /*! \brief Get the matched string.
   */
  operator string_type() const {
    if (matched_)
      return StringType(first_, last_);
    else
      return {};
  }

  /*! \brief Get the matched string.
   */
  string_type str() const { return StringType(first_, last_); }

  bool matched() const { return matched_; }

  void set_first(BidirIt first) {
    first_ = first;
    matched_ = false;
  }

  void set_last(BidirIt last) {
    last_ = last;
    matched_ = true;
  }

 private:
  iterator first_;
  iterator last_;
  bool matched_;
};

template <class BidirIt, class Allocator = std::allocator<sub_match<BidirIt>>>
class match_results : public std::vector<sub_match<BidirIt>, Allocator> {
 private:
  typedef std::vector<sub_match<BidirIt>, Allocator> parent_type;

 public:
  typedef typename std::iterator_traits<BidirIt>::value_type char_type;
  typedef std::basic_string<char_type> string_type;
  typedef typename parent_type::difference_type difference_type;
  typedef typename parent_type::size_type size_type;

  using std::vector<sub_match<BidirIt>, Allocator>::vector;

  /*! \brief Check whether the object has valid match results.
   */
  bool ready() const { return ready_; }

  /*! \brief Set the object to a valid state.
   */
  void set_ready() { ready_ = true; }

  /*! \brief Set the start position of a subgroup.
   */
  void set_sub_start(unsigned group_id, BidirIt it) {
    if (this->size() <= group_id) this->resize(group_id + 1);
    this->at(group_id).set_first(it);
  }

  /*! \brief Set the end position of a subgroup.
   */
  void set_sub_end(unsigned group_id, BidirIt it) {
    assert(this->size() > group_id);
    this->at(group_id).set_last(it);
  }

 private:
  bool ready_;
};
}

#endif
