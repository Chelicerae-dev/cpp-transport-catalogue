#pragma once

#include <optional>
#include <vector>
#include <map>

#include "json.h"

namespace json {
    class BaseContext;
    class DictItemContext;
    class DictValueContext;
    class ArrayValueContext;

    class Builder {
    public:

        BaseContext Value(Node::Value value);

        ArrayValueContext StartArray();
        Builder& EndArray();

        DictItemContext StartDict();
        DictValueContext Key(std::string key);
        Builder& EndDict();

        Node Build();

    private:
        Node root_;
        std::vector<Node*> nodes_stack_;
        std::optional<std::string> key_;
        bool init_ = false;
    };

    class BaseContext {
    public:
        BaseContext(Builder* builder) : builder_(builder) {}

        BaseContext Value(Node::Value value);

        ArrayValueContext StartArray();
        Builder& EndArray();

        DictItemContext StartDict();
        DictValueContext Key(std::string key);
        Builder& EndDict();

        Node Build();
    protected:
        Builder* builder_;
    };

    class DictItemContext : public BaseContext {
    public:
        using BaseContext::BaseContext;
        using BaseContext::Key;
        using BaseContext::EndDict;
        BaseContext Value(Node::Value value) = delete;
        ArrayValueContext StartArray() = delete;
        Builder& EndArray() = delete;
        DictItemContext StartDict() = delete;
        Node Build() = delete;
    };

    class DictValueContext : public BaseContext {
    public:
        using BaseContext::BaseContext;
        DictItemContext Value(Node::Value value);
        using BaseContext::StartArray;
        using BaseContext::StartDict;
        DictValueContext Key(std::string key) = delete;
        Builder& EndArray() = delete;
        Builder& EndDict() = delete;
        Node Build() = delete;
    };

    class ArrayValueContext : public BaseContext {
    public:
        using BaseContext::BaseContext;
        ArrayValueContext Value(Node::Value value);
        using BaseContext::StartArray;
        using BaseContext::StartDict;
        using BaseContext::EndArray;
        DictValueContext Key(std::string key) = delete;
        Builder& EndDict() = delete;
        Node Build() = delete;
    };
}
