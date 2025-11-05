#ifndef ARCADE_KEYVALUES_H
#define ARCADE_KEYVALUES_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstring>

class ArcadeKeyValues {
public:
    enum ValueType {
        TYPE_NONE = 0,
        TYPE_STRING = 1,
        TYPE_INT = 2,
        TYPE_FLOAT = 3,
        TYPE_SUBSECTION = 4
    };

private:
    std::string name;
    std::string stringValue;
    int intValue;
    float floatValue;
    std::vector<std::pair<std::string, std::unique_ptr<ArcadeKeyValues>>> children;
    ArcadeKeyValues* parent;
    int childIndex; // Index of this node in parent's children vector
    ValueType valueType;

public:
    // Constructors
    ArcadeKeyValues(const std::string& keyName = "")
        : name(keyName), intValue(0), floatValue(0.0f), parent(nullptr), childIndex(-1), valueType(TYPE_NONE) {
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

        for (const auto& pair : children) {
            if (pair.first == keyName && pair.second->valueType == TYPE_STRING) {
                return pair.second->stringValue.c_str();
            }
        }
        return defaultValue;
    }

    int GetInt(const char* keyName = nullptr, int defaultValue = 0) const {
        if (keyName == nullptr) {
            return (valueType == TYPE_INT) ? intValue : defaultValue;
        }

        for (const auto& pair : children) {
            if (pair.first == keyName) {
                if (pair.second->valueType == TYPE_INT) {
                    return pair.second->intValue;
                }
                else if (pair.second->valueType == TYPE_STRING) {
                    // Try to convert string to int
                    try {
                        return std::stoi(pair.second->stringValue);
                    }
                    catch (...) {
                        return defaultValue;
                    }
                }
                break;
            }
        }
        return defaultValue;
    }

    float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) const {
        if (keyName == nullptr) {
            return (valueType == TYPE_FLOAT) ? floatValue : defaultValue;
        }

        for (const auto& pair : children) {
            if (pair.first == keyName) {
                if (pair.second->valueType == TYPE_FLOAT) {
                    return pair.second->floatValue;
                }
                else if (pair.second->valueType == TYPE_STRING) {
                    // Try to convert string to float
                    try {
                        return std::stof(pair.second->stringValue);
                    }
                    catch (...) {
                        return defaultValue;
                    }
                }
                else if (pair.second->valueType == TYPE_INT) {
                    return static_cast<float>(pair.second->intValue);
                }
                break;
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

        for (const auto& pair : children) {
            if (pair.first == keyName) {
                if (pair.second->valueType == TYPE_INT) {
                    return pair.second->intValue != 0;
                }
                else if (pair.second->valueType == TYPE_STRING) {
                    const std::string& val = pair.second->stringValue;
                    return val == "1" || val == "true" || val == "True";
                }
                break;
            }
        }
        return defaultValue;
    }

    // Subsection access
    ArcadeKeyValues* FindKey(const char* keyName, bool createIfNotFound = false) {
        for (size_t i = 0; i < children.size(); ++i) {
            if (children[i].first == keyName) {
                return children[i].second.get();
            }
        }

        if (createIfNotFound) {
            auto newKey = std::make_unique<ArcadeKeyValues>(keyName);
            newKey->parent = this;
            newKey->childIndex = static_cast<int>(children.size());
            newKey->valueType = TYPE_SUBSECTION;
            ArcadeKeyValues* ptr = newKey.get();
            children.push_back({keyName, std::move(newKey)});
            return ptr;
        }

        return nullptr;
    }

    const ArcadeKeyValues* FindKey(const char* keyName) const {
        for (const auto& pair : children) {
            if (pair.first == keyName) {
                return pair.second.get();
            }
        }
        return nullptr;
    }

    // Iteration support
    ArcadeKeyValues* GetFirstSubKey() const {
        if (children.empty()) return nullptr;
        return children[0].second.get();
    }

    ArcadeKeyValues* GetNextKey() const {
        if (!parent || childIndex < 0) return nullptr;

        const auto& parentChildren = parent->children;
        int nextIndex = childIndex + 1;
        if (nextIndex < static_cast<int>(parentChildren.size())) {
            return parentChildren[nextIndex].second.get();
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

    // Remove a child key by name
    bool RemoveKey(const char* keyName) {
        if (keyName == nullptr) {
            return false;
        }

        for (size_t i = 0; i < children.size(); ++i) {
            if (children[i].first == keyName) {
                children.erase(children.begin() + i);
                // Update childIndex for all subsequent children
                for (size_t j = i; j < children.size(); ++j) {
                    children[j].second->childIndex = static_cast<int>(j);
                }
                return true;
            }
        }
        return false;
    }

    // Utility methods
    bool IsEmpty() const {
        return valueType == TYPE_NONE && children.empty();
    }

    int GetChildCount() const {
        return static_cast<int>(children.size());
    }

    ValueType GetValueType() const {
        return valueType;
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

    // Get the children vector for iteration (needed for JS conversion)
    const std::vector<std::pair<std::string, std::unique_ptr<ArcadeKeyValues>>>& GetChildren() const {
        return children;
    }

    // Serialize to binary format
    std::vector<uint8_t> SerializeToBinary() const {
        std::vector<uint8_t> result;
        serializeRecursive(result);
        // Add end-of-root object marker so the format matches what parseRecursive expects
        result.push_back(0x08);
        return result;
    }


    // Convert to hex string
    std::string SerializeToHex() const {
        std::vector<uint8_t> binary = SerializeToBinary();
        std::string hexString;
        hexString.reserve(binary.size() * 2);

        for (uint8_t byte : binary) {
            char hexByte[3];
            snprintf(hexByte, sizeof(hexByte), "%02x", byte);
            hexString += hexByte;
        }

        return hexString;
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
            else if (typeByte == 0x03) { // Float32 (little-endian)
                if (position + 4 <= bytes.size()) {
                    // Read 4 bytes as little-endian and interpret as float
                    uint32_t intBits = bytes[position] |
                        (bytes[position + 1] << 8) |
                        (bytes[position + 2] << 16) |
                        (bytes[position + 3] << 24);
                    float value;
                    std::memcpy(&value, &intBits, sizeof(float));
                    child->floatValue = value;
                    child->valueType = TYPE_FLOAT;
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

            // Set childIndex before adding to parent's children vector
            child->childIndex = static_cast<int>(kv->children.size());
            kv->children.push_back({keyName, std::move(child)});
        }

        return kv;
    }

    // Helper method to serialize to binary format
    void serializeRecursive(std::vector<uint8_t>& buffer) const {
        // Serialize all children in order
        for (const auto& pair : children) {
            const std::string& childName = pair.first;
            const std::unique_ptr<ArcadeKeyValues>& child = pair.second;

            // Determine if this is a subsection (has children OR explicitly marked as subsection)
            bool isSubsection = (child->valueType == TYPE_SUBSECTION || child->GetChildCount() > 0);

            // Skip empty strings to avoid cluttering the binary data
            if (child->valueType == TYPE_STRING && child->stringValue.empty()) {
                continue;
            }

            // Skip empty subsections (no children and TYPE_SUBSECTION or TYPE_NONE)
            if (isSubsection && child->GetChildCount() == 0) {
                continue;
            }

            // Write type byte based on actual value type
            if (isSubsection) {
                buffer.push_back(0x00); // Nested object
            }
            else if (child->valueType == TYPE_STRING) {
                buffer.push_back(0x01); // String
            }
            else if (child->valueType == TYPE_INT) {
                buffer.push_back(0x02); // Int32
            }
            else if (child->valueType == TYPE_FLOAT) {
                buffer.push_back(0x03); // Float32
            }
            else {
                continue; // Skip TYPE_NONE or unknown types
            }

            // Write key name
            for (char c : childName) {
                buffer.push_back(static_cast<uint8_t>(c));
            }
            buffer.push_back(0x00); // Null terminator

            // Write value based on type
            if (isSubsection) {
                // Recursively serialize nested object
                child->serializeRecursive(buffer);
                buffer.push_back(0x08); // End of object marker
            }
            else if (child->valueType == TYPE_STRING) {
                // Write string value
                for (char c : child->stringValue) {
                    buffer.push_back(static_cast<uint8_t>(c));
                }
                buffer.push_back(0x00); // Null terminator
            }
            else if (child->valueType == TYPE_INT) {
                // Write int32 value (little-endian)
                int32_t value = child->intValue;
                buffer.push_back(static_cast<uint8_t>(value & 0xFF));
                buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
                buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
                buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
            }
            else if (child->valueType == TYPE_FLOAT) {
                // Write float32 value (little-endian)
                float value = child->floatValue;
                uint32_t intBits;
                std::memcpy(&intBits, &value, sizeof(float));
                buffer.push_back(static_cast<uint8_t>(intBits & 0xFF));
                buffer.push_back(static_cast<uint8_t>((intBits >> 8) & 0xFF));
                buffer.push_back(static_cast<uint8_t>((intBits >> 16) & 0xFF));
                buffer.push_back(static_cast<uint8_t>((intBits >> 24) & 0xFF));
            }
        }
    }
};

#endif