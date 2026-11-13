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
#include <optional>

namespace firebolt {
namespace common {

/**
 * @brief Result wrapper that provides error-safe value access
 *
 * Wraps operation results, distinguishing success from failure and providing
 * safe value access. Basic error information is included; future proposals
 * will expand error classification.
 *
 * @tparam T Type of the successful result value
 */
template<typename T>
class FireboltResult {
public:
    /**
     * @brief Constructs a successful result
     * @param value The successful result value
     */
    static FireboltResult Success(T value);

    /**
     * @brief Constructs a failed result
     * @param error Error description
     */
    static FireboltResult Failure(const std::string& error);

    /**
     * @brief Checks if the result is successful
     * @return true if successful, false if failed
     */
    bool IsSuccess() const;

    /**
     * @brief Checks if the result is failed
     * @return true if failed, false if successful
     */
    bool IsFailure() const;

    /**
     * @brief Gets the result value
     * @return The result value (only valid if successful)
     * @throws std::runtime_error if called on a failed result
     */
    const T& Value() const;

    /**
     * @brief Gets the error description
     * @return Error description (only valid if failed)
     * @throws std::runtime_error if called on a successful result
     */
    const std::string& Error() const;

private:
    FireboltResult(T value, bool success, const std::string& error);

    std::optional<T> m_value;
    std::string m_error;
    bool m_success;
};

// Template implementations

template<typename T>
FireboltResult<T>::FireboltResult(T value, bool success, const std::string& error)
    : m_value(value)
    , m_error(error)
    , m_success(success) {
}

template<typename T>
FireboltResult<T> FireboltResult<T>::Success(T value) {
    return FireboltResult(value, true, "");
}

template<typename T>
FireboltResult<T> FireboltResult<T>::Failure(const std::string& error) {
    return FireboltResult(T(), false, error);
}

template<typename T>
bool FireboltResult<T>::IsSuccess() const {
    return m_success;
}

template<typename T>
bool FireboltResult<T>::IsFailure() const {
    return !m_success;
}

template<typename T>
const T& FireboltResult<T>::Value() const {
    if (!m_success || !m_value) {
        throw std::runtime_error("Attempted to get value from failed result");
    }
    return *m_value;
}

template<typename T>
const std::string& FireboltResult<T>::Error() const {
    if (m_success) {
        throw std::runtime_error("Attempted to get error from successful result");
    }
    return m_error;
}

// Specialization for void type
template<>
class FireboltResult<void> {
public:
    static FireboltResult Success();
    static FireboltResult Failure(const std::string& error);

    bool IsSuccess() const;
    bool IsFailure() const;
    const std::string& Error() const;

private:
    FireboltResult(bool success, const std::string& error);

    std::string m_error;
    bool m_success;
};

inline FireboltResult<void> FireboltResult<void>::Success() {
    return FireboltResult(true, "");
}

inline FireboltResult<void> FireboltResult<void>::Failure(const std::string& error) {
    return FireboltResult(false, error);
}

inline bool FireboltResult<void>::IsSuccess() const {
    return m_success;
}

inline bool FireboltResult<void>::IsFailure() const {
    return !m_success;
}

inline const std::string& FireboltResult<void>::Error() const {
    if (m_success) {
        throw std::runtime_error("Attempted to get error from successful result");
    }
    return m_error;
}

inline FireboltResult<void>::FireboltResult(bool success, const std::string& error)
    : m_error(error)
    , m_success(success) {
}

} // namespace common
} // namespace firebolt
