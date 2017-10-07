#include <experimental/any>
#include <iostream>
#include <typeinfo>
#include <unistd.h>

#include "src/ast.hpp"
#include "src/parser.hpp"
#include "src/scanner.hpp"
#include "src/interpreter.hpp"

namespace stdx = std::experimental;

void runPrompt() {
  std::string line;
  Evaluator eval;
  for (;;) {
    if (isatty(fileno(stdin)))
      std::cout << "> ";
    if (!std::getline(std::cin, line))
      break;
    auto scanner = Scanner(line);
    try {
      auto tokens = scanner.scanTokens();
      auto parser = Parser(tokens);
      auto stmt = parser.parseProgram();
      auto ret = eval.run(stmt);
      if (ret.type() == typeid(double)) {
        double ans = stdx::any_cast<double>(ret);
        std::cout << "< " << ans << std::endl;
      }
    } catch (const char *e) {
      std::cerr << e << std::endl;
    } catch (stdx::bad_any_cast &e) {
      // no return value
    }
  }
}

void runFile(std::string fname) {
  std::cout << "file " << fname << " goes here" << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    runPrompt();
  } else {
    runFile(argv[1]);
  }
  return 0;
}
