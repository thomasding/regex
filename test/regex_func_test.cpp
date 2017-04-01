#include <locale>
#include <string>

#include "gtest/gtest.h"
#include "regex/regex.h"
#include "regex/regex_match_results.h"
#include "regex/regex_func.h"

using namespace regex;

typedef basic_regex<char> Regex;
typedef match_results<typename std::string::iterator> MatchResults;
typedef regex_matcher<Regex, typename std::string::iterator, MatchResults>
RegexMatcher;

TEST(RegexMatchTest, Match) {
  Regex re("a(b)c");
  MatchResults what;
  std::string s("abcd");

  regex_match(s.begin(), s.end(), what, re);

  ASSERT_TRUE(what.ready());
  EXPECT_EQ(2u, what.size());
  EXPECT_EQ("abc", what[0].str());
  EXPECT_EQ("b", what[1].str());
}

TEST(RegexMatchTest, MatchNone) {
  Regex re("a(b)c");
  MatchResults what;
  std::string s("acd");

  regex_match(s.begin(), s.end(), what, re);
  EXPECT_FALSE(what.ready());
}

TEST(RegexSearchTest, Search) {
  Regex re("ab+c");
  MatchResults what;
  std::string s("acaabcdabbcabbbc");

  regex_search(s.begin(), s.end(), what, re);

  ASSERT_TRUE(what.ready());
  ASSERT_EQ(1u, what.size());
  EXPECT_EQ("abc", what[0].str());
}

TEST(RegexSearchTest, SearchNone) {
  Regex re("ab+c");
  MatchResults what;
  std::string s("acaabdabbabbb");

  regex_search(s.begin(), s.end(), what, re);

  EXPECT_FALSE(what.ready());
}
