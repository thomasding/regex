#include <locale>
#include <string>

#include "gtest/gtest.h"
#include "regex/regex_parser.h"

using namespace regex;

template <class String>
using RegexParserType =
    RegexParser<RegexScanner<typename String::const_iterator>,
                CharCategory<typename String::value_type>,
                NFA<Instruction<typename String::value_type>,
                    std::allocator<Instruction<typename String::value_type>>>>;

template <class String>
RegexParserType<String> make_parser(const String& s) {
  return RegexParserType<String>(RegexScanner<typename String::const_iterator>(
      s.cbegin(), s.cend(), std::locale()));
}

TEST(RegexParserTest, EmptyString) {
  std::string v("");
  auto p = make_parser(v);
  ASSERT_EQ(2u, p.nfa().size());
  EXPECT_EQ(kGoto, p.nfa()[0].opcode);
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(kAccept, p.nfa()[1].opcode);
  EXPECT_EQ(0, p.nfa().start_id());
}

TEST(RegexParserTest, OneChar) {
  std::string v("a");
  auto p = make_parser(v);
  ASSERT_EQ(2u, p.nfa().size());
  EXPECT_EQ(kMatchCharCategory, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(kAccept, p.nfa()[1].opcode);
  EXPECT_EQ(0, p.nfa().start_id());
}

TEST(RegexParserTest, Star) {
  std::string v("a*");
  auto p = make_parser(v);
  ASSERT_EQ(3u, p.nfa().size());
  EXPECT_EQ(kMatchCharCategory, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(kFork, p.nfa()[1].opcode);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(2, p.nfa()[1].next2);
  EXPECT_EQ(kAccept, p.nfa()[2].opcode);
  EXPECT_EQ(1, p.nfa().start_id());
}

TEST(RegexParserTest, Plus) {
  std::string v("a+");
  auto p = make_parser(v);
  ASSERT_EQ(3u, p.nfa().size());
  EXPECT_EQ(kMatchCharCategory, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(kFork, p.nfa()[1].opcode);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(2, p.nfa()[1].next2);
  EXPECT_EQ(kAccept, p.nfa()[2].opcode);
  EXPECT_EQ(0, p.nfa().start_id());
}

TEST(RegexParserTest, Optional) {
  std::string v("a?");
  auto p = make_parser(v);
  ASSERT_EQ(4u, p.nfa().size());
  EXPECT_EQ(kMatchCharCategory, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(kGoto, p.nfa()[1].opcode);
  EXPECT_EQ(3, p.nfa()[1].next);
  EXPECT_EQ(kFork, p.nfa()[2].opcode);
  EXPECT_EQ(0, p.nfa()[2].next);
  EXPECT_EQ(1, p.nfa()[2].next2);
  EXPECT_EQ(kAccept, p.nfa()[3].opcode);
  EXPECT_EQ(2, p.nfa().start_id());
}

TEST(RegexParserTest, Or) {
  std::string v("a|b");
  auto p = make_parser(v);
  ASSERT_EQ(5u, p.nfa().size());
  EXPECT_EQ(kMatchCharCategory, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(3, p.nfa()[0].next);
  EXPECT_EQ(kMatchCharCategory, p.nfa()[1].opcode);
  EXPECT_TRUE(p.nfa()[1].cc.match('b'));
  EXPECT_EQ(3, p.nfa()[1].next);
  EXPECT_EQ(kFork, p.nfa()[2].opcode);
  EXPECT_EQ(0, p.nfa()[2].next);
  EXPECT_EQ(1, p.nfa()[2].next2);
  EXPECT_EQ(kGoto, p.nfa()[3].opcode);
  EXPECT_EQ(4, p.nfa()[3].next);
  EXPECT_EQ(kAccept, p.nfa()[4].opcode);
  EXPECT_EQ(2, p.nfa().start_id());
}
