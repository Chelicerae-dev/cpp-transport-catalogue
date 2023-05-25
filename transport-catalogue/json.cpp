#include "json.h"
#include <algorithm>
#include <iterator>
#include <cctype>

using namespace std;

namespace json {

std::string EscapedPrint(const std::string& str) {
    stringstream strm;
    for(auto ch : str) {
        if (ch == '\n') {
            strm << "\\n"s;
        } else if (ch == '\r') {
            strm << "\\r"s;
        } else if (ch == '\t') {
            strm << "\t"s;
        } else if (ch == '\\') {
            strm << "\\\\"s;
        } else if (ch == '\"') {
            strm << "\\\"";
        } else {
            strm << ch;
        }
    }
    return std::string{strm.str()};
}

Node LoadNode(istream& input);

Node LoadNull(istream& input) {
    std::string line;
    for(char c; input >> c && std::isalpha(c);) {
        line.push_back(c);
    }
    if(line == "null"s) {
        return Node{nullptr};
    } else {
        throw ParsingError("Invalid data: "s + line + " isn't null"s);
    }
}

Node LoadBool(istream& input) {
    std::string line;
    char c;
    for(; input >> c && std::isalpha(c);) {
        line.push_back(c);
    }
    input.putback(c);
    if(line == "true"s) {
        return Node{true};
    } else if(line == "false"s) {
        return Node{false};
    } else {
        throw ParsingError("Invalid data: "s + line + " isn't bool"s);
    }
}

Node LoadInt(istream& input) {
    std::string line;
    bool is_int = true;
    char c;
    input >> c;
    line.push_back(c);
    while(std::isdigit(input.peek()) || input.peek() == '-' || input.peek() == '+' || input.peek() == '.'  || input.peek() == 'e' || input.peek() == 'E') {
        input >> c;
        line.push_back(c);
        if(!std::isdigit(c)) is_int = false;
    }

//    for (; input >> c && (std::isdigit(c) || c == '-' || c == '+' || c =='.' || c == 'e' || c == 'E');) {

//        line.push_back(c);
////        char ch = input.peek();
//        if(input.peek() == ']' || input.peek() == '}' || input.peek() == ',') {
//            break;
//        }
////            strm.clear();
//    }
//    result.push_back((LoadNode(strm)));
//    char peek = input.peek();
//    if(peek != '\n' || peek != ' ' || peek != '\r' || peek != '\t' || peek != ',' || peek != '}' || peek != ']') {
//    } else if(c == ']' || c == '}' || c == ',') {
//        input.putback(c);
//    } else if(!std::isdigit(c)){
//        throw ParsingError("Number loading error - occured "s + c + " symbol"s);
//    }
//    bool is_int = line.find('.') == line.npos && line.find('e') == line.npos;
    return is_int ? Node(std::stoi(line)) : Node(std::stod(line));
}

Node LoadString(std::istream& input) {
    using namespace std::literals;
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return Node{s};
}

Node LoadArray(istream& input) {
    if(input.peek() == ']') {
        input.get();
        return Node(Array{});
    }
    Array result;
//    std::stringstream strm;
    char c = 0;
    for (;input >> c && c != ']';) {
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            continue;
        }
        if(c == '}') {
            continue;
        }
        if(c == 4) {
            throw ParsingError("End of text before closing ]"s);
        }
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if(c != ']') {
        throw ParsingError("Closing ] not found"s);
    }
    return Node(std::move(result));
}

Node LoadDict(istream& input) {
    if(input.peek() == '}') return Node(Dict{});
    Dict result;
    char c;
    for (;input >> c && c != '}';) {
        if(c == '\0') {
            throw ParsingError("End of text before closing "s);
        }
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({std::move(key), LoadNode(input)});
    }
    if(c != '}') {
        throw ParsingError("Closing } not found"s);
    }
//    input >> c;
    return Node(std::move(result));
}


Node LoadNode(istream& input) {
    char c;
    input >> c;
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        input >> c;
        return LoadNode(input);
    } else if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (c == 'n') {
        input.putback(c);
        return LoadNull(input);
    } else if (c == 't' || c == 'f') {
        input.putback(c);
        return LoadBool(input);
    } else if (c == '-' || (std::isdigit(c))) {
        input.putback(c);
        return LoadInt(input);
    } else {
        throw ParsingError("Unable to load"s + c);
    }
}

//}  // namespace

Node::Node(std::nullptr_t) {
    value_.emplace<std::nullptr_t>(nullptr);
}

Node::Node(std::string value){
    value_.emplace<std::string>(value);
}

Node::Node(bool value) {
    value_.emplace<bool>(value);
}

Node::Node(int value) {
    value_.emplace<int>(value);
}

Node::Node(double value) {
    value_.emplace<double>(value);
}

Node::Node(Array array) {
    value_.emplace<Array>(array);
}

Node::Node(Dict map) {
    value_.emplace<Dict>(map);
}



bool Node::IsInt() const {
    return holds_alternative<int>(value_);
}
bool Node::IsDouble() const {
    return holds_alternative<int>(value_) || holds_alternative<double>(value_);
}
bool Node::IsPureDouble() const {
    return holds_alternative<double>(value_);
}
bool Node::IsBool() const {
    return holds_alternative<bool>(value_);
}
bool Node::IsString() const {
    return holds_alternative<std::string>(value_);
}
bool Node::IsNull() const {
    return holds_alternative<std::nullptr_t>(value_);
}
bool Node::IsArray() const {
    return holds_alternative<Array>(value_);
}
bool Node::IsMap() const {
    return holds_alternative<Dict>(value_);
}

std::string Node::AsString() const {
    if(!IsString()) {
        throw std::logic_error("Not a string"s);
    }
    return get<std::string>(value_);
//    return Escaped(get<std::string>(value_));
}
int Node::AsInt() const {
    if(!IsInt()) {
        throw std::logic_error("Not an int"s);
    }
    return get<int>(value_);
}
bool Node::AsBool() const {
    if(!IsBool()) {
        throw std::logic_error("Not a bool"s);
    }
    return get<bool>(value_);
}
double Node::AsDouble() const {
    if(!IsDouble()) {
        throw std::logic_error("Not a double"s);
    }
    if(IsPureDouble()) {
        return get<double>(value_);
    } else {
        return get<int>(value_) * 1.0;
    }
}
const Array& Node::AsArray() const {
    if(!IsArray()) {
        throw std::logic_error("Not an array"s);
    }
    return get<Array>(value_);
}
const Dict& Node::AsMap() const {
    if(!IsMap()) {
        throw std::logic_error("Not a map"s);
    }
    return get<Dict>(value_);
}

const Node::Value& Node::GetValue() const {
    return value_;
}

bool Node::operator!=(const Node& other) {
    return value_ != other.value_;
}
bool Node::operator==(const Node& other) {
    return value_ == other.value_;
}

void PrintValue(std::nullptr_t, std::ostream& out) {
    using namespace std::literals;
    out << "null"s;
}

void PrintValue(const std::string& val, std::ostream& out) {
    std::string value = EscapedPrint(val);
    out << "\""s << value << "\""s;
}

void PrintValue(bool val, std::ostream& out) {
    if(val) {
        out << "true"s;
    } else {
        out << "false"s;
    }
}

void PrintValue(const Array& array, std::ostream& out) {
    using namespace std::literals;
    bool first = true;
    out << '[';
    for(size_t i = 0; i < array.size(); ++i) {
        if(first) {
            PrintNode(array[i], out);
            first = false;
        } else {
            out << ", "sv;
            PrintNode(array[i], out);
        }
    }
    out << ']';
}


void PrintValue(const Dict& dict, std::ostream& out) {
    using namespace std::literals;
    bool first = true;
    out << "{"sv;
    for(const auto& [key, val] : dict) {
        if(first) {
            out << '"' << key << "\":"sv;
            PrintNode(val, out);
            first = false;
        } else {
            out << ", \""sv << key << "\":"sv;
            PrintNode(val, out);
        }
    }
    out << "}"sv;
}

void PrintNode(const Node& node, std::ostream& out) {
    std::visit(
        [&out](const auto& value){ PrintValue(value, out); },
        node.GetValue());
}

Document::Document(Node root)
    : root_(std::move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    (void) &doc;
    (void) &output;
    PrintNode(doc.GetRoot(), output);
    // Реализуйте функцию самостоятельно
}

bool operator==(const Node& node1, const Node& node2) {
    return node1.GetValue() == node2.GetValue();
}
bool operator!=(const Node& node1, const Node& node2) {
    return node1.GetValue() != node2.GetValue();
}
bool operator==(const Document& doc1, const Document& doc2) {
    return doc1.GetRoot() == doc2.GetRoot();
}
bool operator!=(const Document& doc1, const Document& doc2) {
    return !(doc1 == doc2);
}


}  // namespace json
