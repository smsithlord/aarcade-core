#ifndef ARCADE_KEYVALUES_H
#define ARCADE_KEYVALUES_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

class ArcadeKeyValues {
private:
    std::string name;
    std::string stringValue;
    int intValue;
    float floatValue;
    std::unordered_map<std::string, std::unique_ptr<ArcadeKeyValues>> children;
    ArcadeKeyValues* parent;

    enum ValueType {
        TYPE_NONE = 0,
        TYPE_STRING = 1,
        TYPE_INT = 2,
        TYPE_FLOAT = 3,
        TYPE_SUBSECTION = 4
    } valueType;

public:
    // Constructors
    ArcadeKeyValues(const std::string& keyName = "")
        : name(keyName), intValue(0), floatValue(0.0f), parent(nullptr), valueType(TYPE_NONE) {
    }

    ~ArcadeKeyValues() = default;

    // Static factory method to parse from hex data
    static std::unique_ptr<ArcadeKeyValues> ParseFromHex(const std::string& hexData) {
        auto bytes = hexToBytes(hexData);
        size_t position = 0;
        return parseRecursive(bytes, position, "root");
    }

    // Core accessor methods (Valve-style API)
    const char* GetName() const {
        return name.c_str();
    }

    const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") const {
        if (keyName == nullptr) {
            return (valueType == TYPE_STRING) ? stringValue.c_str() : defaultValue;
        }

        auto it = children.find(keyName);
        if (it != children.end() && it->second->valueType == TYPE_STRING) {
            return it->second->stringValue.c_str();
        }
        return defaultValue;
    }

    int GetInt(const char* keyName = nullptr, int defaultValue = 0) const {
        if (keyName == nullptr) {
            return (valueType == TYPE_INT) ? intValue : defaultValue;
        }

        auto it = children.find(keyName);
        if (it != children.end()) {
            if (it->second->valueType == TYPE_INT) {
                return it->second->intValue;
            }
            else if (it->second->valueType == TYPE_STRING) {
                // Try to convert string to int
                try {
                    return std::stoi(it->second->stringValue);
                }
                catch (...) {
                    return defaultValue;
                }
            }
        }
        return defaultValue;
    }

    float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) const {
        if (keyName == nullptr) {
            return (valueType == TYPE_FLOAT) ? floatValue : defaultValue;
        }

        auto it = children.find(keyName);
        if (it != children.end()) {
            if (it->second->valueType == TYPE_FLOAT) {
                return it->second->floatValue;
            }
            else if (it->second->valueType == TYPE_STRING) {
                // Try to convert string to float
                try {
                    return std::stof(it->second->stringValue);
                }
                catch (...) {
                    return defaultValue;
                }
            }
            else if (it->second->valueType == TYPE_INT) {
                return static_cast<float>(it->second->intValue);
            }
        }
        return defaultValue;
    }

    bool GetBool(const char* keyName = nullptr, bool defaultValue = false) const {
        if (keyName == nullptr) {
            if (valueType == TYPE_INT) {
                return intValue != 0;
            }
            else if (valueType == TYPE_STRING) {
                return stringValue == "1" || stringValue == "true" || stringValue == "True";
            }
            return defaultValue;
        }

        auto it = children.find(keyName);
        if (it != children.end()) {
            if (it->second->valueType == TYPE_INT) {
                return it->second->intValue != 0;
            }
            else if (it->second->valueType == TYPE_STRING) {
                const std::string& val = it->second->stringValue;
                return val == "1" || val == "true" || val == "True";
            }
        }
        return defaultValue;
    }

    // Subsection access
    ArcadeKeyValues* FindKey(const char* keyName, bool createIfNotFound = false) {
        auto it = children.find(keyName);
        if (it != children.end()) {
            return it->second.get();
        }

        if (createIfNotFound) {
            auto newKey = std::make_unique<ArcadeKeyValues>(keyName);
            newKey->parent = this;
            newKey->valueType = TYPE_SUBSECTION;
            ArcadeKeyValues* ptr = newKey.get();
            children[keyName] = std::move(newKey);
            return ptr;
        }

        return nullptr;
    }

    const ArcadeKeyValues* FindKey(const char* keyName) const {
        auto it = children.find(keyName);
        return (it != children.end()) ? it->second.get() : nullptr;
    }

    // Iteration support
    ArcadeKeyValues* GetFirstSubKey() const {
        if (children.empty()) return nullptr;
        return children.begin()->second.get();
    }

    ArcadeKeyValues* GetNextKey() const {
        if (!parent) return nullptr;

        auto& parentChildren = parent->children;
        auto it = parentChildren.find(name);
        if (it != parentChildren.end()) {
            ++it;
            if (it != parentChildren.end()) {
                return it->second.get();
            }
        }
        return nullptr;
    }

    // Value setting methods
    void SetString(const char* keyName, const char* value) {
        if (keyName == nullptr) {
            stringValue = value;
            valueType = TYPE_STRING;
        }
        else {
            auto key = FindKey(keyName, true);
            key->stringValue = value;
            key->valueType = TYPE_STRING;
        }
    }

    void SetInt(const char* keyName, int value) {
        if (keyName == nullptr) {
            intValue = value;
            valueType = TYPE_INT;
        }
        else {
            auto key = FindKey(keyName, true);
            key->intValue = value;
            key->valueType = TYPE_INT;
        }
    }

    void SetFloat(const char* keyName, float value) {
        if (keyName == nullptr) {
            floatValue = value;
            valueType = TYPE_FLOAT;
        }
        else {
            auto key = FindKey(keyName, true);
            key->floatValue = value;
            key->valueType = TYPE_FLOAT;
        }
    }

    void SetBool(const char* keyName, bool value) {
        SetInt(keyName, value ? 1 : 0);
    }

    // Utility methods
    bool IsEmpty() const {
        return valueType == TYPE_NONE && children.empty();
    }

    int GetChildCount() const {
        return static_cast<int>(children.size());
    }

    void Clear() {
        children.clear();
        stringValue.clear();
        intValue = 0;
        floatValue = 0.0f;
        valueType = TYPE_NONE;
    }

    // Debug output
    void PrintToConsole(int depth = 0) const {
        std::string indent(depth * 2, ' ');

        std::cout << indent << "\"" << name << "\"";

        switch (valueType) {
        case TYPE_STRING:
            std::cout << " \"" << stringValue << "\"" << std::endl;
            break;
        case TYPE_INT:
            std::cout << " " << intValue << std::endl;
            break;
        case TYPE_FLOAT:
            std::cout << " " << floatValue << std::endl;
            break;
        case TYPE_SUBSECTION:
        case TYPE_NONE:
            std::cout << std::endl << indent << "{" << std::endl;
            for (const auto& pair : children) {
                const std::string& childName = pair.first;
                const std::unique_ptr<ArcadeKeyValues>& child = pair.second;
                child->PrintToConsole(depth + 1);
            }
            std::cout << indent << "}" << std::endl;
            break;
        }
    }

    // Get the children map for iteration (needed for JS conversion)
    const std::unordered_map<std::string, std::unique_ptr<ArcadeKeyValues>>& GetChildren() const {
        return children;
    }

private:
    // Helper methods for parsing
    static std::vector<uint8_t> hexToBytes(const std::string& hex) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            uint8_t byte = static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }

    static std::pair<std::string, size_t> readString(const std::vector<uint8_t>& bytes, size_t offset) {
        std::string result;
        size_t pos = offset;

        while (pos < bytes.size() && bytes[pos] != 0) {
            result += static_cast<char>(bytes[pos]);
            pos++;
        }

        return { result, pos + 1 };
    }

    static std::unique_ptr<ArcadeKeyValues> parseRecursive(const std::vector<uint8_t>& bytes, size_t& position, const std::string& rootName) {
        auto kv = std::make_unique<ArcadeKeyValues>(rootName);
        kv->valueType = TYPE_SUBSECTION;

        while (position < bytes.size()) {
            // Get type byte
            uint8_t typeByte = bytes[position++];

            // End of object marker
            if (typeByte == 0x08) {
                break;
            }

            // Read key name
            std::pair<std::string, size_t> keyResult = readString(bytes, position);
            std::string keyName = keyResult.first;
            size_t newPos = keyResult.second;
            position = newPos;

            // Empty key means end of object
            if (keyName.empty()) {
                break;
            }

            // Create child key
            auto child = std::make_unique<ArcadeKeyValues>(keyName);
            child->parent = kv.get();

            // Parse value based on type
            if (typeByte == 0x00) { // Nested object
                child = parseRecursive(bytes, position, keyName);
                child->parent = kv.get();
            }
            else if (typeByte == 0x01) { // String
                std::pair<std::string, size_t> valueResult = readString(bytes, position);
                std::string value = valueResult.first;
                size_t valuePos = valueResult.second;
                position = valuePos;
                child->stringValue = value;
                child->valueType = TYPE_STRING;
            }
            else if (typeByte == 0x02) { // Int32 (little-endian)
                if (position + 4 <= bytes.size()) {
                    int32_t value = bytes[position] |
                        (bytes[position + 1] << 8) |
                        (bytes[position + 2] << 16) |
                        (bytes[position + 3] << 24);
                    child->intValue = value;
                    child->valueType = TYPE_INT;
                    position += 4;
                }
                else {
                    break;
                }
            }
            else {
                std::cerr << "Unknown type byte: 0x" << std::hex << static_cast<int>(typeByte) << std::endl;
                break;
            }

            kv->children[keyName] = std::move(child);
        }

        return kv;
    }
};

#endif