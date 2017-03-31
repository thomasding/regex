#include <locale>
#include <string>

#include "gtest/gtest.h"
#include "regex/regex.h"
#include "regex/regex_match_results.h"
#include "regex/regex_matcher.h"

using namespace regex;

typedef basic_regex<char> Regex;
typedef match_results<typename std::string::iterator> MatchResults;
typedef regex_matcher<Regex, typename std::string::iterator, MatchResults>
    RegexMatcher;

TEST(RegexMatcherTest, MatchEmpty) {
  Regex re("");
  MatchResults what;
  std::string s("a");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("", what[0].str());
}

TEST(RegexMatcherTest, MatchChar) {
  Regex re("a");
  MatchResults what;
  std::string s("ab");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("a", what[0].str());
}
