#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>

namespace SimpleJson {

enum class JsonType { Null, Bool, Number, String, Array, Object };

struct JsonValue {
    JsonType type = JsonType::Null;
    bool bool_val = false;
    double num_val = 0.0;
    std::string str_val;
    std::vector<JsonValue> arr_val;
    std::map<std::string, JsonValue> obj_val;

    bool is_null() const { return type == JsonType::Null; }
    bool is_bool() const { return type == JsonType::Bool; }
    bool is_number() const { return type == JsonType::Number; }
    bool is_string() const { return type == JsonType::String; }
    bool is_array() const { return type == JsonType::Array; }
    bool is_object() const { return type == JsonType::Object; }

    const JsonValue& operator[](const std::string& key) const {
        static JsonValue null_v;
        if (type != JsonType::Object) return null_v;
        auto it = obj_val.find(key);
        return (it != obj_val.end()) ? it->second : null_v;
    }

    const JsonValue& operator[](size_t index) const {
        static JsonValue null_v;
        if (type != JsonType::Array || index >= arr_val.size()) return null_v;
        return arr_val[index];
    }

    bool has_key(const std::string& key) const {
        return type == JsonType::Object && obj_val.find(key) != obj_val.end();
    }

    double as_double(double default_val = 0.0) const {
        return (type == JsonType::Number) ? num_val : default_val;
    }

    int as_int(int default_val = 0) const {
        return (type == JsonType::Number) ? static_cast<int>(num_val) : default_val;
    }

    std::string as_string(const std::string& default_val = "") const {
        return (type == JsonType::String) ? str_val : default_val;
    }

    bool as_bool(bool default_val = false) const {
        return (type == JsonType::Bool) ? bool_val : default_val;
    }
};

class Parser {
    const std::string& src;
    size_t pos = 0;

    void skip_whitespace() {
        while (pos < src.size()) {
            char c = src[pos];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                pos++;
            } else if (c == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
                pos += 2;
                while (pos < src.size() && src[pos] != '\n') pos++;
            } else {
                break;
            }
        }
    }

    char peek() {
        skip_whitespace();
        return (pos < src.size()) ? src[pos] : '\0';
    }

    char get() {
        skip_whitespace();
        return (pos < src.size()) ? src[pos++] : '\0';
    }

public:
    explicit Parser(const std::string& input) : src(input), pos(0) {}

    JsonValue parse() {
        skip_whitespace();
        JsonValue val = parse_value();
        skip_whitespace();
        return val;
    }

private:
    JsonValue parse_value() {
        char c = peek();
        if (c == '{') return parse_object();
        if (c == '[') return parse_array();
        if (c == '"') return parse_string();
        if (c == 't' || c == 'f') return parse_bool();
        if (c == 'n') return parse_null();
        if (c == '-' || (c >= '0' && c <= '9')) return parse_number();
        return JsonValue();
    }

    JsonValue parse_object() {
        JsonValue val;
        val.type = JsonType::Object;
        get(); // '{'
        while (true) {
            char c = peek();
            if (c == '}' || c == '\0') {
                if (c == '}') get();
                break;
            }
            if (c != '"') break;
            JsonValue key = parse_string();
            skip_whitespace();
            if (peek() == ':') get();
            JsonValue child = parse_value();
            val.obj_val[key.str_val] = child;
            skip_whitespace();
            if (peek() == ',') get();
        }
        return val;
    }

    JsonValue parse_array() {
        JsonValue val;
        val.type = JsonType::Array;
        get(); // '['
        while (true) {
            char c = peek();
            if (c == ']' || c == '\0') {
                if (c == ']') get();
                break;
            }
            JsonValue child = parse_value();
            val.arr_val.push_back(child);
            skip_whitespace();
            if (peek() == ',') get();
        }
        return val;
    }

    JsonValue parse_string() {
        JsonValue val;
        val.type = JsonType::String;
        get(); // '"'
        std::string s;
        while (pos < src.size()) {
            char c = src[pos++];
            if (c == '"') break;
            if (c == '\\' && pos < src.size()) {
                char esc = src[pos++];
                if (esc == 'n') s += '\n';
                else if (esc == 't') s += '\t';
                else if (esc == 'r') s += '\r';
                else s += esc;
            } else {
                s += c;
            }
        }
        val.str_val = s;
        return val;
    }

    JsonValue parse_number() {
        JsonValue val;
        val.type = JsonType::Number;
        size_t start = pos;
        if (src[pos] == '-') pos++;
        while (pos < src.size() && ((src[pos] >= '0' && src[pos] <= '9') || src[pos] == '.' || src[pos] == 'e' || src[pos] == 'E' || src[pos] == '+' || src[pos] == '-')) {
            pos++;
        }
        std::string num_str = src.substr(start, pos - start);
        val.num_val = std::strtod(num_str.c_str(), nullptr);
        return val;
    }

    JsonValue parse_bool() {
        JsonValue val;
        val.type = JsonType::Bool;
        if (src.substr(pos, 4) == "true") { pos += 4; val.bool_val = true; }
        else if (src.substr(pos, 5) == "false") { pos += 5; val.bool_val = false; }
        return val;
    }

    JsonValue parse_null() {
        JsonValue val;
        val.type = JsonType::Null;
        if (src.substr(pos, 4) == "null") pos += 4;
        return val;
    }
};

} // namespace SimpleJson

#endif // JSON_PARSER_H
