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

TEST(RegexMatcherTest, MatchStar1) {
  Regex re("a*");
  MatchResults what;
  std::string s("aa");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("aa", what[0].str());
}

TEST(RegexMatcherTest, MatchStar2) {
  Regex re("a*");
  MatchResults what;
  std::string s("");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("", what[0].str());
}

TEST(RegexMatcherTest, MatchPlus1) {
  Regex re("a+");
  MatchResults what;
  std::string s("aa");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("aa", what[0].str());
}

TEST(RegexMatcherTest, MatchPlus2) {
  Regex re("a+");
  MatchResults what;
  std::string s("");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_FALSE(what.ready());
}

TEST(RegexMatcherTest, MatchOptional1) {
  Regex re("a?");
  MatchResults what;
  std::string s("aa");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("a", what[0].str());
}

TEST(RegexMatcherTest, MatchOptional2) {
  Regex re("a*");
  MatchResults what;
  std::string s("b");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("", what[0].str());
}

TEST(RegexMatcherTest, MatchOr1) {
  Regex re("a|b");
  MatchResults what;
  std::string s("aa");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("a", what[0].str());
}

TEST(RegexMatcherTest, MatchOr2) {
  Regex re("a|b");
  MatchResults what;
  std::string s("bc");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("b", what[0].str());
}

TEST(RegexMatcherTest, MatchSequence) {
  Regex re("abc");
  MatchResults what;
  std::string s("abcd");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("abc", what[0].str());
}

TEST(RegexMatcherTest, MatchGroup1) {
  Regex re("a(b)c");
  MatchResults what;
  std::string s("abcd");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(2u, what.size());
  EXPECT_EQ("abc", what[0].str());
  EXPECT_EQ("b", what[1].str());
}

TEST(RegexMatcherTest, MatchGroup2) {
  Regex re("a(b)((c))");
  MatchResults what;
  std::string s("abcd");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(4u, what.size());
  EXPECT_EQ("abc", what[0].str());
  EXPECT_EQ("b", what[1].str());
  EXPECT_EQ("c", what[2].str());
  EXPECT_EQ("c", what[3].str());
}

TEST(RegexMatcherTest, MatchLoop1) {
  Regex re("a**");
  MatchResults what;
  std::string s("b");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("", what[0].str());
}

TEST(RegexMatcherTest, MatchLoop2) {
  Regex re("a?+");
  MatchResults what;
  std::string s("b");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(1u, what.size());
  EXPECT_EQ("", what[0].str());
}

TEST(RegexMatcherTest, MatchComplicated1) {
  Regex re("a+(b*(c|d+)+(e?))*");
  MatchResults what;
  std::string s("aaaabcceddcdc");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(4u, what.size());
  EXPECT_EQ("aaaabcceddcdc", what[0].str());
  EXPECT_EQ("ddcdc", what[1].str());
  EXPECT_EQ("c", what[2].str());
  EXPECT_EQ("", what[3].str());
}

TEST(RegexMatcherTest, MatchComplicated2) {
  Regex re("(a|bc?de+(f*))+");
  MatchResults what;
  std::string s("abdeeeeb");
  RegexMatcher rm(s.begin(), s.end(), re, what);
  EXPECT_TRUE(what.ready());
  EXPECT_EQ(3u, what.size());
  EXPECT_EQ("abdeeee", what[0].str());
  EXPECT_EQ("bdeeee", what[1].str());
  EXPECT_EQ("", what[2].str());
}
