#include <iostream>
#include <vector>
#include <string>
#include <set>

void dfs(int v, std::vector<bool>& marked,
         std::vector<std::set<int>> possible_substr) {
  marked[v] = true;
  for (auto u : possible_substr[v]) {
    if (!marked[u]) {
      dfs(u, marked, possible_substr);
    }
  }
}

void sum(size_t sz, bool& is_error,
         std::vector<std::vector<std::set<int>>>& expression_stack) {
  if (expression_stack.size() < 2) {
    is_error = true;
    return;
  }
  std::vector<std::set<int>> term1 = expression_stack.back();
  expression_stack.pop_back();
  std::vector<std::set<int>> term2 = expression_stack.back();
  expression_stack.pop_back();
  std::vector<std::set<int>> new_term(sz + 1);
  for (size_t i = 0; i < sz + 1; ++i) {
    new_term[i] = term1[i];
    for (auto& elem : term2[i]) {
      new_term[i].insert(elem);
    }
  }
  expression_stack.push_back(new_term);
}

void concatenation(size_t sz, bool& is_error,
                   std::vector<std::vector<std::set<int>>>& expression_stack) {
  if (expression_stack.size() < 2) {
    is_error = true;
    return;
  }
  std::vector<std::set<int>> term2(expression_stack.back());
  expression_stack.pop_back();
  std::vector<std::set<int>> term1(expression_stack.back());
  expression_stack.pop_back();
  std::vector<std::set<int>> new_term(sz + 1);
  for (size_t i = 0; i < sz + 1; ++i) {
    for (auto& middle : term1[i]) {
      for (auto& ending : term2[middle]) {
        new_term[i].insert(ending);
      }
    }
  }
  expression_stack.push_back(new_term);
}

void klini(size_t sz, bool& is_error,
           std::vector<std::vector<std::set<int>>>& expression_stack) {
  if (expression_stack.empty()) {
    is_error = true;
    return;
  }
  std::vector<std::set<int>> term = expression_stack.back();
  expression_stack.pop_back();
  std::vector<std::set<int>> new_term(sz + 1);
  for (size_t i = 0; i < sz + 1; ++i) {
    std::vector<bool> marked(sz + 1);
    dfs(i, marked, term);
    for (size_t j = 0; j < sz + 1; ++j) {
      if (marked[j]) new_term[i].insert(j);
    }
  }
  expression_stack.push_back(new_term);
}

void empty_word(size_t sz,
                std::vector<std::vector<std::set<int>>>& expression_stack) {
  expression_stack.emplace_back(sz + 1);
  for (size_t j = 0; j < sz + 1; ++j) {
    expression_stack.back()[j].insert(j);
  }
}

void literal(size_t sz,
             std::vector<std::vector<std::set<int>>>& expression_stack,
             std::string str, char liter) {
  expression_stack.emplace_back(sz + 1);
  for (size_t j = 0; j < sz; ++j) {
    if (str[j] == liter) {
      expression_stack.back()[j].insert(j + 1);
    }
  }
}

std::pair<bool, int> answer(
    size_t sz, bool& is_error,
    std::vector<std::vector<std::set<int>>>& expression_stack) {
  if (expression_stack.size() != 1) {
    is_error = true;
    return std::make_pair(false, 0);
  }
  auto possible_substr = expression_stack[0];
  for (size_t i = 0; i < sz + 1; ++i) {
    bool last_is_sz = *possible_substr[i].rbegin() == sz;
    if (!possible_substr[i].empty() && last_is_sz) {
      return std::make_pair(true, sz - i);
    }
  }
  return std::make_pair(false, 0);
}

std::pair<bool, int> longest_suffix(std::string regular_exp, std::string str) {
  std::vector<std::vector<std::set<int>>> expression_stack;
  size_t sz = str.size();
  bool is_error = false;
  for (size_t ind = 0; ind < regular_exp.size(); ++ind) {
    if (regular_exp[ind] == '*') {
      klini(sz, is_error, expression_stack);
    } else if (regular_exp[ind] == '.') {
      concatenation(sz, is_error, expression_stack);
    } else if (regular_exp[ind] == '+') {
      sum(sz, is_error, expression_stack);
    } else if (regular_exp[ind] == '1') {
      empty_word(sz, expression_stack);
    } else {
      literal(sz, expression_stack, str, regular_exp[ind]);
    }
    if (is_error) {
      return std::make_pair(false, 0);
    }
  }
  auto ans = answer(sz, is_error, expression_stack);
  if (ans.first) {
    return std::make_pair(true, ans.second);
  }

  if (is_error) {
    return std::make_pair(false, 0);
  }
  return std::make_pair(false, 1);
}

void print_answer(std::pair<bool, int> ans) {
  if (!ans.first && ans.second == 0) {
    std::cout << "ERROR";
    return;
  }
  if (!ans.first && ans.second == 1) {
    std::cout << "INF";
    return;
  }
  std::cout << ans.second;
}

struct test {
  std::string regular_exp;
  std::string str;
  std::pair<bool, int> ans;
  test() = default;
  test(std::string regular_exp, std::string str, std::pair<bool, int> ans)
      : regular_exp(regular_exp), str(str), ans(ans){};
  bool run() { return ans == longest_suffix(regular_exp, str); }
};

bool run_all_tests() {
  size_t tests_estimation = 30;
  std::vector<test> tests;
  tests.reserve(tests_estimation);

  // sum tests
  tests.emplace_back("ab+", "a", std::make_pair(true, 1));
  tests.emplace_back("ab+", "b", std::make_pair(true, 1));
  tests.emplace_back("ab+", "caaba", std::make_pair(true, 1));
  tests.emplace_back("ab+", "cccc", std::make_pair(false, 1));
  tests.emplace_back("a1+", "ddda", std::make_pair(true, 1));
  tests.emplace_back("a1+", "addd", std::make_pair(true, 0));
  tests.emplace_back("1a+", "abba", std::make_pair(true, 1));

  // concatenation tests
  tests.emplace_back("ab.", "a", std::make_pair(false, 1));
  tests.emplace_back("ab.", "ab", std::make_pair(true, 2));
  tests.emplace_back("ab.", "caaba", std::make_pair(false, 1));
  tests.emplace_back("ab.", "ccab", std::make_pair(true, 2));
  tests.emplace_back("a1.", "ddda", std::make_pair(true, 1));
  tests.emplace_back("a1.", "addd", std::make_pair(false, 1));
  tests.emplace_back("1a.", "abba", std::make_pair(true, 1));

  // no operation tests
  tests.emplace_back("a", "a", std::make_pair(true, 1));
  tests.emplace_back("a", "ab", std::make_pair(false, 1));
  tests.emplace_back("1", "caaba", std::make_pair(true, 0));

  // klini tests
  tests.emplace_back("1*", "abc", std::make_pair(true, 0));
  tests.emplace_back("a*", "abaaaaa", std::make_pair(true, 5));
  tests.emplace_back("a*", "caabab", std::make_pair(true, 0));
  tests.emplace_back("ab+*", "cbacaba", std::make_pair(true, 3));
  tests.emplace_back("ab+*", "ababc", std::make_pair(true, 0));
  tests.emplace_back("ab.*", "caabab", std::make_pair(true, 4));
  tests.emplace_back("ab.*", "caaba", std::make_pair(true, 0));

  // non valid expressions test
  tests.emplace_back("a+", "", std::make_pair(false, 0));
  tests.emplace_back("a1.+", "", std::make_pair(false, 0));
  tests.emplace_back("a1.***+", "", std::make_pair(false, 0));
  tests.emplace_back("a1.abab", "", std::make_pair(false, 0));

  // difficult tests
  tests.emplace_back("ab+c.aba.*.bac.+.+*", "babc", std::make_pair(true, 2));
  tests.emplace_back("acb..bab.c.*.ab.ba.+.+*a.", "cbaa",
                     std::make_pair(true, 1));

  bool correct = true;
  for (auto& t : tests) {
    correct = (t.run() && correct);
    if (!correct) std::cout << t.regular_exp << " " << t.str << '\n';
  }
  return correct;
}

int main() {
  if (!run_all_tests()) {
    std::cout << "TESTING FAILED";
    return 0;
  }

  std::string regular_exp, str;
  std::cin >> regular_exp;
  std::cin >> str;
  auto ans = longest_suffix(regular_exp, str);
  print_answer(ans);
  return 0;
}
