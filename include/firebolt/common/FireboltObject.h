/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace firebolt {
namespace common {

/**
 * @brief JSON utility class that abstracts the JSON library from generated models
 *
 * Internally uses nlohmann::json but provides a type-safe interface that can
 * be switched to a different JSON library implementation in the future.
 */
class FireboltObject {
public:
    FireboltObject();
    ~FireboltObject() = default;

    /**
     * @brief Sets a field value in the object
     * @tparam T Type of the value (string, int, bool, etc.)
     * @param name Field name
     * @param value Field value
     */
    template<typename T>
    void set(const std::string& name, const T& value);

    /**
     * @brief Gets a field value from the object
     * @tparam T Type of the value (string, int, bool, etc.)
     * @param name Field name
     * @return Field value
     */
    template<typename T>
    T get(const std::string& name) const;

    /**
     * @brief Converts the object to a JSON string
     * @return JSON string representation
     */
    std::string toString() const;

    /**
     * @brief Populates the object from a JSON string
     * @param payload JSON string to parse
     */
    void fromString(const std::string& payload);

    /**
     * @brief Checks whether a field exists in the object
     * @param name Field name
     * @return true if the field is present
     */
    bool has(const std::string& name) const;

    /**
     * @brief Gets a nested object field
     * @param name Field name
     * @return FireboltObject wrapping the nested value
     */
    FireboltObject getObject(const std::string& name) const;

private:
    nlohmann::json m_json;
};

// Template implementations

template<typename T>
void FireboltObject::set(const std::string& name, const T& value) {
    m_json[name] = value;
}

template<typename T>
T FireboltObject::get(const std::string& name) const {
    return m_json[name].get<T>();
}

// Specializations for common types
template<>
inline void FireboltObject::set<std::string>(const std::string& name, const std::string& value) {
    m_json[name] = value;
}

template<>
inline std::string FireboltObject::get<std::string>(const std::string& name) const {
    return m_json[name].get<std::string>();
}

template<>
inline void FireboltObject::set<int32_t>(const std::string& name, const int32_t& value) {
    m_json[name] = value;
}

template<>
inline int32_t FireboltObject::get<int32_t>(const std::string& name) const {
    return m_json[name].get<int32_t>();
}

template<>
inline void FireboltObject::set<bool>(const std::string& name, const bool& value) {
    m_json[name] = value;
}

template<>
inline bool FireboltObject::get<bool>(const std::string& name) const {
    return m_json[name].get<bool>();
}

} // namespace common
} // namespace firebolt
