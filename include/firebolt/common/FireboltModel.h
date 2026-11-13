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

#include "FireboltObject.h"
#include <string>

namespace firebolt {
namespace common {

/**
 * @brief CRTP base class for generated Firebolt models
 *
 * Provides shared serialization and deserialization behavior while delegating
 * model-specific field mappings back to the generated type via CRTP.
 *
 * @tparam T The concrete model type (CRTP pattern)
 */
template<typename T>
class FireboltModel {
public:
    /**
     * @brief Converts the model to a JSON string
     * @return JSON string representation
     */
    std::string toString() const;

    /**
     * @brief Populates the model from a JSON string
     * @param payload JSON string to parse
     */
    void fromString(const std::string& payload);

    /**
     * @brief Converts the model to a FireboltObject
     * @return FireboltObject containing the model's data
     */
    FireboltObject toObject() const;

    /**
     * @brief Populates the model from a FireboltObject
     * @param object FireboltObject to read from
     */
    void fromObject(const FireboltObject& object);

protected:
    /**
     * @brief Writes model fields to a FireboltObject
     * @param object Object to write to
     */
    void write(FireboltObject& object) const;

    /**
     * @brief Reads model fields from a FireboltObject
     * @param object Object to read from
     */
    void read(const FireboltObject& object);
};

// Template implementations

template<typename T>
std::string FireboltModel<T>::toString() const {
    FireboltObject object;
    static_cast<const T*>(this)->write(object);
    return object.toString();
}

template<typename T>
void FireboltModel<T>::fromString(const std::string& payload) {
    FireboltObject object;
    object.fromString(payload);
    static_cast<T*>(this)->read(object);
}

template<typename T>
FireboltObject FireboltModel<T>::toObject() const {
    FireboltObject object;
    static_cast<const T*>(this)->write(object);
    return object;
}

template<typename T>
void FireboltModel<T>::fromObject(const FireboltObject& object) {
    static_cast<T*>(this)->read(object);
}

} // namespace common
} // namespace firebolt
