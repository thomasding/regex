#include <string>

#include "gtest/gtest.h"

#include "regex/regex_nfa.h"
#include "regex/regex_optimizer.h"

using namespace regex;

typedef NFA<Instruction<char>, std::allocator<Instruction<char>>> CNFA;

typedef RedundantInsnOptimizer<CNFA, std::allocator<int>>
    CRedundantInsnOptimizer;

TEST(RedundantInsnOptimizerTest, EmptyNFA) {
  CNFA nfa;
  CRedundantInsnOptimizer opt(nfa);
  static_cast<void>(opt);
}

TEST(RedundantInsnOptimizerTest, OrdinaryNFA) {
  CNFA nfa;
  nfa.append_fork(1, 2);
  nfa.append_goto(3);
  nfa.append_goto(4);
  nfa.append_goto(2);
  nfa.append_accept();
  nfa.set_start_id(0);

  CRedundantInsnOptimizer opt(std::move(nfa));
  ASSERT_EQ(5ul, opt.nfa().size());
  ASSERT_EQ(kFork, opt.nfa()[0].opcode);
  ASSERT_EQ(4, opt.nfa()[0].next);
  ASSERT_EQ(4, opt.nfa()[0].next2);
  ASSERT_EQ(kAccept, opt.nfa()[4].opcode);
}
