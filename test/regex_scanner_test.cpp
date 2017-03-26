#include <locale>
#include <string>

#include "gtest/gtest.h"
#include "regex/regex_scanner.h"

using namespace regex;

template <class String>
RegexScanner<typename String::const_iterator> make_scanner(const String& s) {
  return RegexScanner<typename String::const_iterator>(s.cbegin(), s.cend(),
                                                       std::locale());
}

TEST(RegexScannerTest, EmptySequence) {
  std::string v("");
  auto s = make_scanner(v);
  EXPECT_EQ(kEOF, s.cur_token());
}

TEST(RegexScannerTest, OrdinarySequence) {
  std::string v("(a*?|b(+)");
  auto s = make_scanner(v);
  EXPECT_EQ(kLeftGroup, s.cur_token());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('a', s.cur_char());
  s.advance();
  EXPECT_EQ(kStar, s.cur_token());
  s.advance();
  EXPECT_EQ(kOptional, s.cur_token());
  s.advance();
  EXPECT_EQ(kOr, s.cur_token());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('b', s.cur_char());
  s.advance();
  EXPECT_EQ(kLeftGroup, s.cur_token());
  s.advance();
  EXPECT_EQ(kPlus, s.cur_token());
  s.advance();
  EXPECT_EQ(kRightGroup, s.cur_token());
  s.advance();
  EXPECT_EQ(kEOF, s.cur_token());
}

TEST(RegexScannerTest, EscapedSequence) {
  std::string v(R"_(\*\+\?\(\)\|\\)_");
  auto s = make_scanner(v);

  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('*', s.cur_char());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('+', s.cur_char());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('?', s.cur_char());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('(', s.cur_char());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ(')', s.cur_char());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('|', s.cur_char());
  s.advance();
  EXPECT_EQ(kCharacter, s.cur_token());
  EXPECT_EQ('\\', s.cur_char());
  s.advance();
  EXPECT_EQ(kEOF, s.cur_token());
}

TEST(RegexScannerTest, IncompleteEscapedSequence) {
  std::string v("\\");
  EXPECT_THROW(make_scanner(v), RegexError);
}

TEST(RegexScannerTest, DisallowedEscapedSequence) {
  std::string v("\\a");
  EXPECT_THROW(make_scanner(v), RegexError);
}
