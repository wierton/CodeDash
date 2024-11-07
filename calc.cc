#include <iostream>
#include <sstream>
#include <assert.h>
#include <vector>

enum token_type_t {
  tok_plus,
  tok_minus,
  tok_mult,
  tok_div,
  tok_number,
};

struct token_t {
  token_type_t token_type;
  int value;
};

token_t lex(std::istream &is) {
  char ch = is.get();
  switch (ch) {
  case '+': return token_t{tok_plus};
  case '-': return token_t{tok_minus};
  case '*': return token_t{tok_mult};
  case '/': return token_t{tok_div};
  case '0' ... '9': {
    int value = ch - '0';
    while (is.good() && std::isdigit(is.peek())) {
      value = value * 10 + is.peek() - '0';
      is.get();
    }
    return token_t{tok_number, value};
  }
  }
  assert (0 && "unreachable");
}

int get_priority(token_type_t tt) {
  if (tt == tok_plus || tt == tok_minus)
    return 1;
  if (tt == tok_mult || tt == tok_div)
    return 2;
  return 0;
}

void pop_opcode(std::vector<token_type_t> &opcodes,
  std::vector<int> &operands) {
  if (operands.size() <= 1) return;
  int rhs = operands.back(); operands.pop_back();
  int lhs = operands.back(); operands.pop_back();
  token_type_t op = opcodes.back();
  opcodes.pop_back();
  std::cout << "op=" << op << " @ " << lhs << ", " << rhs << "\n";
  switch (op) {
  case tok_plus: operands.push_back(lhs + rhs); break;
  case tok_minus: operands.push_back(lhs - rhs); break;
  case tok_mult: operands.push_back(lhs * rhs); break;
  case tok_div: operands.push_back(lhs / rhs); break;
  default: assert (0 && "unreachable");
  }
}

int compute(std::istream &is) {
  std::vector<token_t> toks;
  while ((is.peek(), is.good())) {
    toks.push_back(lex(is));
    std::cout << toks.back().token_type << ", "
      << toks.back().value << std::endl;
  }

  std::vector<token_type_t> opcodes;
  std::vector<int> operands;

  for (token_t &tok : toks) {
    if (tok.token_type == tok_number) {
      operands.push_back(tok.value);
    } else {
      while (opcodes.size() && get_priority(opcodes.back()) > get_priority(tok.token_type)) {
        pop_opcode(opcodes, operands);
      }
      opcodes.push_back(tok.token_type);
    }
  }

  std::cout << "here" << std::endl;
  while (operands.size() > 1)
    pop_opcode(opcodes, operands);
  return operands[0];
}

int main() {
  const std::string input = "4*2+3+3";
  std::istringstream iss(input);
  int result = compute(iss);
  std::cout << result << "\n";
  return 0;
}
