# TODO: Stop using .map({ .kv }) everywhere
sub ptr($name) {
  "std::shared_ptr<$name>"
}
sub vec($name) {
  "std::vector<$name>"
}
sub define-members(@members) {
  @members.map({ .kv }).map(-> ($k, $v) {
    "  $v $k;"
  }).join("\n");
}
sub define-methods($root, $name, @members) {
  my $args = @members.map({ .kv }).map(-> ($k, $v) {"$v $k"}).join(", ");
  my $alist = @members.map({ .kv }).map(-> ($k, $v) { "$k\($k)" }).join(", ");
  my $ctor = "  $name\($args) : $alist \{}";
  my $copy = "  $name\(const $name& other) = default;";
  my $write = qq:to/EOF/;
  void write_to(std::ostream& os) const \{
    os << "$name\(" << $(print-node @members).join(" << \", \" << ") << ")";
  }
  EOF
  my $accept = qq:to/EOF/;
  stdx::any accept($($root)Visitor& visitor) \{
    return visitor.visit$name\(*this);
  }
  EOF
  return ($ctor, $copy, $write, $accept).join("\n");
}
sub print-node(@members) {
  @members.map({ .kv }).map(-> ($k, $v) {
    if ($v ~~ /vec/) {
      qq[
        "$k = [";
        write_vec$($v ~~ /ptr/ ?? "_ptr" !! "")\(os, this->$k);  
        os << "]"
      ]
    } else {
      qq["$k = " << $($v ~~ /ptr/ ?? "*" !! "")this->$k]
    }
  })
}
sub define-ast($name, %types) {
  say qq:to/EOF/;
  %types.keys.map({ "  struct $_;" }).join("\n")
  class $($name)Visitor \{
    public:
    %types.keys.map({ "  virtual stdx::any visit$_\($_&) = 0;" }).join("\n")
  };
  struct $name : Node \{
    virtual void write_to(std::ostream&) const = 0;
    virtual stdx::any accept($($name)Visitor&) = 0;
  };
  inline std::ostream& operator<<(std::ostream& os, const $name& node) \{
    node.write_to(os);
    return os;
  }
  EOF
  for %types.kv -> $k, @v {
    say qq:to/EOF/;
    struct $k : $name \{
      $(define-members @v)
      $(define-methods $name, $k, @v)
    };
    EOF
  }
}
sub define-enum($name, @values) {
  say "enum class $name \{ @values.join(", ") };";
  say qq:to/EOF/;
  inline std::ostream& operator<<(std::ostream& os, const $name& node) \{
    switch (node) \{
      @values.map({ qq[case $name\::$_: os << "$_"; break; ] })
    }
    return os;
  }
  EOF
}

say q:to/HEADER/;
#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <experimental/any>

namespace stdx = std::experimental;

template<typename T> void write_vec(std::ostream& os, std::vector<T> vec) {
  bool first = true;
  for (auto& it : vec) {
    if (!first)
    os << ", ";
    os << it;
  }
}


template<typename T> void write_vec_ptr(std::ostream& os, std::vector<T> vec) {
  bool first = true;
  for (auto& it : vec) {
    if (!first)
    os << ", ";
    os << *it;
  }
}

class Node {};
HEADER

define-enum "BinopType", <ADD SUB MUL DIV LT LE GT GE EQ NE ASSIGN>;
define-enum "UnopType", <NEGATE NOT>;

define-ast("Expr", {
  Literal => (:value("double"), ),
  Variable => (:ident("std::string"), ),
  Binop => (:op("BinopType"), :lhs(ptr "Expr"), :rhs(ptr "Expr")),
  Unop => (:op("UnopType"), :rhs(ptr "Expr")),
  Call => (:callee(ptr "Expr"), :args(vec ptr "Expr")),
});

define-ast("Stmt", {
  ExpressionStmt => (:expr(ptr "Expr"), ),
  If => (:cond(ptr "Expr"), :ifTrue(ptr "Stmt"), :ifFalse(ptr "Stmt")),
  While => (:cond(ptr "Expr"), :body(ptr "Stmt")),
  Block => (:stmts(vec ptr "Stmt"), ),
  Fun => (:name("std::string"), :bindings(vec "std::string"), :body(vec ptr "Stmt")),
  Print => (:expr(ptr "Expr"), ),
});
