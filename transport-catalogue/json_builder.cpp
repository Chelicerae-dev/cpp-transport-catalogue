#include "json_builder.h"

namespace json {

    BaseContext Builder::Value(Node::Value value) {
        if(!init_) {
            root_ = Node(std::move(value));
            init_ = true;
        } else if(nodes_stack_.size() == 0 && init_) {
            throw std::logic_error("Invalid node structure");
        } else if(std::holds_alternative<Array>(nodes_stack_.back()->GetValue())) {
            std::get<Array>(nodes_stack_.back()->GetValue()).push_back(std::move(value));
        } else if(std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
            if(key_.has_value()) {
                std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()] = std::move(value);
                key_.reset();
            } else {
                throw std::logic_error("No key given for Dict element");
            }
        } else {
            throw std::logic_error("Invalid node structure");
        }
        return BaseContext(this);
    }

    ArrayValueContext Builder::StartArray() {
        if(!init_) {
            root_ = Node(Array());
            nodes_stack_.push_back(&root_);
            init_ = true;
        } else if(nodes_stack_.size() == 0 && init_) {
            throw std::logic_error("Invalid node structure");
        } else if(std::holds_alternative<Array>(nodes_stack_.back()->GetValue())) {
            std::get<Array>(nodes_stack_.back()->GetValue()).emplace_back(Array());
            nodes_stack_.push_back(&std::get<Array>(nodes_stack_.back()->GetValue()).back());
        } else if(std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
            if(key_.has_value()) {
                std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()] = Node(Array());
                nodes_stack_.push_back(&std::get<Dict>(nodes_stack_.back()->GetValue()).at(key_.value()));
                key_.reset();
            } else {
                throw std::logic_error("No key given for Dict element");
            }
        } else {
            throw std::logic_error("Invalid node structure");
        }
        return ArrayValueContext(this);
    }
    Builder& Builder::EndArray() {
        if(nodes_stack_.size() == 0) {
            throw std::logic_error("No nodes in stack");
        } else if(!std::holds_alternative<Array>(nodes_stack_.back()->GetValue())) {
            throw std::logic_error("No array to end.");
        } else {
            nodes_stack_.pop_back();
        }
        return *this;
    }

    DictItemContext Builder::StartDict() {
        if(!init_) {
            root_ = Node(Dict());
            nodes_stack_.push_back(&root_);
            init_ = true;
        } else if(nodes_stack_.size() == 0 && init_) {
            throw std::logic_error("Invalid node structure");
        } else if(std::holds_alternative<Array>(nodes_stack_.back()->GetValue())) {
            std::get<Array>(nodes_stack_.back()->GetValue()).emplace_back(Dict());
            nodes_stack_.push_back(&std::get<Array>(nodes_stack_.back()->GetValue()).back());
        } else if(std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
            if(key_.has_value()) {
                std::get<Dict>(nodes_stack_.back()->GetValue())[key_.value()] = Node(Dict());
                nodes_stack_.push_back(&std::get<Dict>(nodes_stack_.back()->GetValue()).at(key_.value()));
                key_.reset();
            } else {
                throw std::logic_error("No key given for Dict element");
            }
        } else {
            throw std::logic_error("Invalid node structure");
        }
        return DictItemContext(this);
    }
    DictValueContext Builder::Key(std::string key) {
        if(nodes_stack_.size() != 0 && std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
            if(key_.has_value()) {
                throw std::logic_error("Key already set");
            }
            key_ = std::move(key);
            return DictValueContext(this);
        } else {
            throw std::logic_error("Calling Key() outside StartDict()");
        }
    }
    Builder& Builder::EndDict() {
        if(nodes_stack_.size() == 0) {
            throw std::logic_error("No nodes in stack");
        } else if(!std::holds_alternative<Dict>(nodes_stack_.back()->GetValue())) {
            throw std::logic_error("No array to end.");
        } else if(key_.has_value()){
            throw std::logic_error("Value for Dict element not set");
        } else {
            nodes_stack_.pop_back();
        }
        return *this;
    }

    Node Builder::Build() {
        if(nodes_stack_.size() == 0) {
            if(init_)return root_;
            throw std::logic_error("Building empty node");
        }
        throw std::logic_error("Building unfinished node");
    }

    BaseContext BaseContext::Value(Node::Value value) {
        return builder_->Value(std::move(value));
    }

    ArrayValueContext BaseContext::StartArray() {
        return builder_->StartArray();
    }

    Builder& BaseContext::EndArray() {
        return builder_->EndArray();
    }

    DictItemContext BaseContext::StartDict() {
        return builder_->StartDict();
    }
    DictValueContext BaseContext::Key(std::string key) {
        return builder_->Key(key);
    }
    Builder& BaseContext::EndDict() {
        return builder_->EndDict();
    }

    Node BaseContext::Build() {
        return builder_->Build();
    }

    DictItemContext DictValueContext::Value(Node::Value value) {
        auto temp = builder_->Value(value);
        return static_cast<DictItemContext&>(temp);
    }

    ArrayValueContext ArrayValueContext::Value(Node::Value value) {
        auto temp = builder_->Value(value);
        return static_cast<ArrayValueContext&>(temp);
    }

}
