#include <locale>
#include <string>

#include "gtest/gtest.h"
#include "regex/regex_scanner.h"

using namespace regex;

template <class String>
regex_scanner<typename String::const_iterator> make_scanner(const String& s) {
  return regex_scanner<typename String::const_iterator>(s.cbegin(), s.cend(),
                                                        std::locale());
}

TEST(RegexScannerTest, EmptySequence) {
  std::string v("");
  auto s = make_scanner(v);
  EXPECT_EQ(k_eof, s.cur_token());
}

TEST(RegexScannerTest, OrdinarySequence) {
  std::string v("(a*?|b(+)");
  auto s = make_scanner(v);
  EXPECT_EQ(k_left_group, s.cur_token());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('a', s.cur_char());
  s.advance();
  EXPECT_EQ(k_star, s.cur_token());
  s.advance();
  EXPECT_EQ(k_optional, s.cur_token());
  s.advance();
  EXPECT_EQ(k_or, s.cur_token());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('b', s.cur_char());
  s.advance();
  EXPECT_EQ(k_left_group, s.cur_token());
  s.advance();
  EXPECT_EQ(k_plus, s.cur_token());
  s.advance();
  EXPECT_EQ(k_right_group, s.cur_token());
  s.advance();
  EXPECT_EQ(k_eof, s.cur_token());
}

TEST(RegexScannerTest, EscapedSequence) {
  std::string v(R"_(\*\+\?\(\)\|\\)_");
  auto s = make_scanner(v);

  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('*', s.cur_char());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('+', s.cur_char());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('?', s.cur_char());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('(', s.cur_char());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ(')', s.cur_char());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('|', s.cur_char());
  s.advance();
  EXPECT_EQ(k_character, s.cur_token());
  EXPECT_EQ('\\', s.cur_char());
  s.advance();
  EXPECT_EQ(k_eof, s.cur_token());
}

TEST(RegexScannerTest, IncompleteEscapedSequence) {
  std::string v("\\");
  EXPECT_THROW(make_scanner(v), regex_error);
}

TEST(RegexScannerTest, DisallowedEscapedSequence) {
  std::string v("\\a");
  EXPECT_THROW(make_scanner(v), regex_error);
}
