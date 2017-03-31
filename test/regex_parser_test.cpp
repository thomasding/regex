#include <locale>
#include <string>

#include "gtest/gtest.h"
#include "regex/regex_parser.h"

using namespace regex;

template <class String>
using RegexParser =
    regex_parser<regex_scanner<typename String::const_iterator>,
                 char_category<typename String::value_type>,
                 nfa<instruction<typename String::value_type>,
                     std::allocator<instruction<typename String::value_type>>>>;

template <class String>
RegexParser<String> make_parser(const String& s) {
  return RegexParser<String>(regex_scanner<typename String::const_iterator>(
      s.cbegin(), s.cend(), std::locale()));
}

TEST(RegexParserTest, EmptyString) {
  std::string v("");
  auto p = make_parser(v);
  ASSERT_EQ(4u, p.nfa().size());
  EXPECT_EQ(k_goto, p.nfa()[0].opcode);
  EXPECT_EQ(2, p.nfa()[0].next);
  EXPECT_EQ(k_mark_group_start, p.nfa()[1].opcode);
  EXPECT_EQ(0u, p.nfa()[1].group_id);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[2].opcode);
  EXPECT_EQ(0u, p.nfa()[2].group_id);
  EXPECT_EQ(3, p.nfa()[2].next);
  EXPECT_EQ(k_accept, p.nfa()[3].opcode);
  EXPECT_EQ(1, p.nfa().start_id());
}

TEST(RegexParserTest, OneChar) {
  std::string v("a");
  auto p = make_parser(v);
  ASSERT_EQ(4u, p.nfa().size());
  EXPECT_EQ(k_match_char_category, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(2, p.nfa()[0].next);
  EXPECT_EQ(k_mark_group_start, p.nfa()[1].opcode);
  EXPECT_EQ(0u, p.nfa()[1].group_id);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[2].opcode);
  EXPECT_EQ(0u, p.nfa()[2].group_id);
  EXPECT_EQ(3, p.nfa()[2].next);
  EXPECT_EQ(k_accept, p.nfa()[3].opcode);
  EXPECT_EQ(1, p.nfa().start_id());
}

TEST(RegexParserTest, Star) {
  std::string v("a*");
  auto p = make_parser(v);
  ASSERT_EQ(5u, p.nfa().size());
  EXPECT_EQ(k_match_char_category, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(k_fork, p.nfa()[1].opcode);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(3, p.nfa()[1].next2);
  EXPECT_EQ(k_mark_group_start, p.nfa()[2].opcode);
  EXPECT_EQ(0u, p.nfa()[2].group_id);
  EXPECT_EQ(1, p.nfa()[2].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[3].opcode);
  EXPECT_EQ(0u, p.nfa()[3].group_id);
  EXPECT_EQ(4, p.nfa()[3].next);
  EXPECT_EQ(k_accept, p.nfa()[4].opcode);
  EXPECT_EQ(2, p.nfa().start_id());
}

TEST(RegexParserTest, Plus) {
  std::string v("a+");
  auto p = make_parser(v);
  ASSERT_EQ(5u, p.nfa().size());
  EXPECT_EQ(k_match_char_category, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(k_fork, p.nfa()[1].opcode);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(3, p.nfa()[1].next2);
  EXPECT_EQ(k_mark_group_start, p.nfa()[2].opcode);
  EXPECT_EQ(0u, p.nfa()[2].group_id);
  EXPECT_EQ(0, p.nfa()[2].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[3].opcode);
  EXPECT_EQ(0u, p.nfa()[3].group_id);
  EXPECT_EQ(4, p.nfa()[3].next);
  EXPECT_EQ(k_accept, p.nfa()[4].opcode);
  EXPECT_EQ(2, p.nfa().start_id());
}

TEST(RegexParserTest, Optional) {
  std::string v("a?");
  auto p = make_parser(v);
  ASSERT_EQ(6u, p.nfa().size());
  EXPECT_EQ(k_match_char_category, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(k_goto, p.nfa()[1].opcode);
  EXPECT_EQ(4, p.nfa()[1].next);
  EXPECT_EQ(k_fork, p.nfa()[2].opcode);
  EXPECT_EQ(0, p.nfa()[2].next);
  EXPECT_EQ(1, p.nfa()[2].next2);
  EXPECT_EQ(k_mark_group_start, p.nfa()[3].opcode);
  EXPECT_EQ(0u, p.nfa()[3].group_id);
  EXPECT_EQ(2, p.nfa()[3].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[4].opcode);
  EXPECT_EQ(0u, p.nfa()[4].group_id);
  EXPECT_EQ(5, p.nfa()[4].next);
  EXPECT_EQ(k_accept, p.nfa()[5].opcode);
  EXPECT_EQ(3, p.nfa().start_id());
}

TEST(RegexParserTest, Or) {
  std::string v("a|b");
  auto p = make_parser(v);
  ASSERT_EQ(7u, p.nfa().size());
  EXPECT_EQ(k_match_char_category, p.nfa()[0].opcode);
  EXPECT_TRUE(p.nfa()[0].cc.match('a'));
  EXPECT_EQ(3, p.nfa()[0].next);
  EXPECT_EQ(k_match_char_category, p.nfa()[1].opcode);
  EXPECT_TRUE(p.nfa()[1].cc.match('b'));
  EXPECT_EQ(3, p.nfa()[1].next);
  EXPECT_EQ(k_fork, p.nfa()[2].opcode);
  EXPECT_EQ(0, p.nfa()[2].next);
  EXPECT_EQ(1, p.nfa()[2].next2);
  EXPECT_EQ(k_goto, p.nfa()[3].opcode);
  EXPECT_EQ(5, p.nfa()[3].next);
  EXPECT_EQ(k_mark_group_start, p.nfa()[4].opcode);
  EXPECT_EQ(0u, p.nfa()[4].group_id);
  EXPECT_EQ(2, p.nfa()[4].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[5].opcode);
  EXPECT_EQ(0u, p.nfa()[5].group_id);
  EXPECT_EQ(6, p.nfa()[5].next);
  EXPECT_EQ(k_accept, p.nfa()[6].opcode);
  EXPECT_EQ(4, p.nfa().start_id());
}

TEST(RegexParserTest, Group) {
  std::string v("(ab)*");
  auto p = make_parser(v);
  ASSERT_EQ(8u, p.nfa().size());
  EXPECT_EQ(k_match_char_category, p.nfa()[0].opcode);
  EXPECT_EQ(1, p.nfa()[0].next);
  EXPECT_EQ(k_match_char_category, p.nfa()[1].opcode);
  EXPECT_EQ(3, p.nfa()[1].next);
  EXPECT_EQ(k_mark_group_start, p.nfa()[2].opcode);
  EXPECT_EQ(1u, p.nfa()[2].group_id);
  EXPECT_EQ(0, p.nfa()[2].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[3].opcode);
  EXPECT_EQ(1u, p.nfa()[3].group_id);
  EXPECT_EQ(4, p.nfa()[3].next);
  EXPECT_EQ(k_fork, p.nfa()[4].opcode);
  EXPECT_EQ(2, p.nfa()[4].next);
  EXPECT_EQ(6, p.nfa()[4].next2);
  EXPECT_EQ(k_mark_group_start, p.nfa()[5].opcode);
  EXPECT_EQ(0u, p.nfa()[5].group_id);
  EXPECT_EQ(4, p.nfa()[5].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[6].opcode);
  EXPECT_EQ(0u, p.nfa()[6].group_id);
  EXPECT_EQ(7, p.nfa()[6].next);
  EXPECT_EQ(k_accept, p.nfa()[7].opcode);
  EXPECT_EQ(5, p.nfa().start_id());
}

TEST(RegexParserTest, EmptyStar) {
  std::string v("()*");
  auto p = make_parser(v);
  ASSERT_EQ(8u, p.nfa().size());
  EXPECT_EQ(k_goto, p.nfa()[0].opcode);
  EXPECT_EQ(2, p.nfa()[0].next);
  EXPECT_EQ(k_mark_group_start, p.nfa()[1].opcode);
  EXPECT_EQ(1u, p.nfa()[1].group_id);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[2].opcode);
  EXPECT_EQ(1u, p.nfa()[2].group_id);
  EXPECT_EQ(4, p.nfa()[2].next);
  EXPECT_EQ(k_advance, p.nfa()[3].opcode);
  EXPECT_EQ(1, p.nfa()[3].next);
  EXPECT_EQ(k_fork, p.nfa()[4].opcode);
  EXPECT_EQ(3, p.nfa()[4].next);
  EXPECT_EQ(6, p.nfa()[4].next2);
  EXPECT_EQ(k_mark_group_start, p.nfa()[5].opcode);
  EXPECT_EQ(0u, p.nfa()[5].group_id);
  EXPECT_EQ(4, p.nfa()[5].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[6].opcode);
  EXPECT_EQ(0u, p.nfa()[6].group_id);
  EXPECT_EQ(7, p.nfa()[6].next);
  EXPECT_EQ(k_accept, p.nfa()[7].opcode);
  EXPECT_EQ(5, p.nfa().start_id());
}

TEST(RegexParserTest, EmptyPlus) {
  std::string v("()+");
  auto p = make_parser(v);
  ASSERT_EQ(8u, p.nfa().size());
  EXPECT_EQ(k_goto, p.nfa()[0].opcode);
  EXPECT_EQ(2, p.nfa()[0].next);
  EXPECT_EQ(k_mark_group_start, p.nfa()[1].opcode);
  EXPECT_EQ(1u, p.nfa()[1].group_id);
  EXPECT_EQ(0, p.nfa()[1].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[2].opcode);
  EXPECT_EQ(1u, p.nfa()[2].group_id);
  EXPECT_EQ(4, p.nfa()[2].next);
  EXPECT_EQ(k_advance, p.nfa()[3].opcode);
  EXPECT_EQ(1, p.nfa()[3].next);
  EXPECT_EQ(k_fork, p.nfa()[4].opcode);
  EXPECT_EQ(3, p.nfa()[4].next);
  EXPECT_EQ(6, p.nfa()[4].next2);
  EXPECT_EQ(k_mark_group_start, p.nfa()[5].opcode);
  EXPECT_EQ(0u, p.nfa()[5].group_id);
  EXPECT_EQ(3, p.nfa()[5].next);
  EXPECT_EQ(k_mark_group_end, p.nfa()[6].opcode);
  EXPECT_EQ(0u, p.nfa()[6].group_id);
  EXPECT_EQ(7, p.nfa()[6].next);
  EXPECT_EQ(k_accept, p.nfa()[7].opcode);
  EXPECT_EQ(5, p.nfa().start_id());
}

TEST(RegexParserTest, IllegalStar) {
  std::string v("*a");
  EXPECT_THROW(make_parser(v), regex_error);
}

TEST(RegexParserTest, IllegalPlus) {
  std::string v("+a");
  EXPECT_THROW(make_parser(v), regex_error);
}

TEST(RegexParserTest, IllegalOptional) {
  std::string v("?a");
  EXPECT_THROW(make_parser(v), regex_error);
}

TEST(RegexParserTest, IllegalGroupA) {
  std::string v("a(bc");
  EXPECT_THROW(make_parser(v), regex_error);
}

TEST(RegexParserTest, IllegalGroupB) {
  std::string v("a(b)c)");
  EXPECT_THROW(make_parser(v), regex_error);
}
