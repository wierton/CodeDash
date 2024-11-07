#include <stdio.h>
#include <assert.h>
#include <vector>
#include <iostream>
#include <sstream>

enum token_type_t {
  tok_inv,
  tok_plus,
  tok_minus,
  tok_glue,
  tok_equal,
  tok_number,
  tok_x,
};

struct token_t {
  token_type_t token_type;
  int value;
};

struct value_t { int a, b; };

token_t lex(std::istream &is) {
  char ch = is.get();
  switch (ch) {
  case '+': return token_t{tok_plus};
  case '-': return token_t{tok_minus};
  case '=': return token_t{tok_equal};
  case 'x': return token_t{tok_x};
  case '0' ... '9':
    return token_t{tok_number, ch - '0'};
  }
  assert (0 && "unreachable");
}

int get_prior(token_type_t token_type) {
  if (token_type == tok_plus || token_type == tok_minus)
    return 1;
  if (token_type == tok_glue)
    return 2;
  if (token_type == tok_equal)
    return 0;
  assert (0 && "unexpected token");
}

void pop_opcode(std::vector<token_type_t> &opcodes,
    std::vector<value_t> &operands) {
  if (opcodes.empty()) return;
  auto op = opcodes.back(); opcodes.pop_back();
  value_t rhs = operands.back(); operands.pop_back();
  value_t lhs = operands.back(); operands.pop_back();
  value_t result = {0, 0};
  switch (op) {
  case tok_plus:
    result.a = lhs.a + rhs.a;
    result.b = lhs.b + rhs.b;
    break;
  case tok_minus:
    result.a = lhs.a - rhs.a;
    result.b = lhs.b - rhs.b;
    break;
  case tok_glue:
    result.a = 0;
    result.b = lhs.a;
    break;
  case tok_equal:
    result.a = lhs.a - rhs.a;
    result.b = lhs.b - rhs.b;
    break;
  default:
    assert (0 && "unreachable");
    break;
  }
  operands.push_back(result);
  std::cout << "lhs=(" << lhs.a << ", " << lhs.b << ") ";
  std::cout << "op=" << op << " ";
  std::cout << "rhs=(" << rhs.a << ", " << rhs.b << ") ";
  std::cout << "result=(" << result.a << ", " << result.b << ")";
  std::cout << std::endl;
}

int compute(std::istream &is) {
  std::vector<token_t> tokens;
  token_t prev {tok_inv};
  while ((is.peek(), is.good())) {
    auto tok = lex(is);
    tokens.push_back(tok);
    if ((tok.token_type == tok_number && prev.token_type == tok_x)
        || (tok.token_type == tok_x && prev.token_type == tok_number))
      tokens.push_back(token_t{tok_glue});
    prev = tok;
  }

  std::vector<token_type_t> opcodes;
  std::vector<value_t> operands;
  for (auto &tok : tokens) {
    if (tok.token_type == tok_number) {
      operands.push_back(value_t{tok.value, 0});
    } else if (tok.token_type == tok_x) {
      operands.push_back(value_t{0, 1});
    } else {
      while (opcodes.size() && get_prior(opcodes.back()) > get_prior(tok.token_type)) {
        pop_opcode(opcodes, operands);
      }
      opcodes.push_back(tok.token_type);
    }
  }

  while (opcodes.size())
    pop_opcode(opcodes, operands);

  assert (operands.size());
  return - operands[0].a / operands[0].b;
}

int main() {
  const std::string input = "1+2x=3+4x";
  std::istringstream iss(input);
  int result = compute(iss);
  std::cout << "result=" << result << std::endl;
  return 0;
}
