#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <string_view>
#include <sstream>

namespace json {

std::string Escaped(const std::string& str);
std::string EscapedPrint(const std::string& str);

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
   /* Реализуйте Node, используя std::variant */
    using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

    Node() = default;
    Node(std::nullptr_t);
    Node(std::string value);
    Node(bool value);
    Node(int value);
    Node(double value);
    Node(Array array);
    Node(Dict map);

    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    std::string AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;

    const Value& GetValue() const;

    bool operator!=(const Node& other);
    bool operator==(const Node& other);

private:
    Value value_;

};

void PrintValue(std::nullptr_t, std::ostream& out);
void PrintValue(const std::string& val, std::ostream& out);
void PrintValue(bool val, std::ostream& out);
// Шаблон, подходящий для вывода double и int
template <typename Val>
void PrintValue(const Val& value, std::ostream& out) {
    out << value;
}
void PrintValue(const Array& array, std::ostream& out);
void PrintValue(const Dict& dict, std::ostream& out);

void PrintNode(const Node& node, std::ostream& out);

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent};
    }
};

template <typename Value>
void PrintValue(const Value& value, const PrintContext& ctx) {
    PrintValue(value, ctx.Indented().out);
}

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

bool operator==(const Node& node1, const Node& node2);
bool operator!=(const Node& node1, const Node& node2);
//Внезапно сравниваем документы без упоминания в тз, стоило сразу читать всю пачку
bool operator==(const Document& doc1, const Document& doc2);
bool operator!=(const Document& doc1, const Document& doc2);
}  // namespace json

