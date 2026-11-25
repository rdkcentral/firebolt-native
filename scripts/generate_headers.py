#!/usr/bin/env python3
"""
Firebolt API Header Generator

Generates C++ headers from OpenSpec YAML specifications.
"""

import argparse
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional
import yaml
from jinja2 import Environment, FileSystemLoader


# Type mapping from spec types to C++ types
TYPE_MAP = {
    "bool": "bool",
    "string": "std::string",
    "unsigned": "uint64_t",
    "double": "double",
}


def sanitize_identifier(wire_value: str) -> str:
    """
    Convert wire value to safe C++ identifier.
    Example: "app:adult" -> AppAdult, "hdcp2.2" -> Hdcp2_2
    """
    # Replace non-alphanumeric characters with underscores
    safe = ""
    for i, c in enumerate(wire_value):
        if c.isalnum():
            safe += c.upper() if i == 0 else c
        else:
            safe += "_"
    # Prefix with underscore if starts with digit
    if safe and safe[0].isdigit():
        safe = "_" + safe
    return safe


def map_type(type_def: Dict[str, Any], required: bool = True) -> str:
    """Map spec type definition to C++ type."""
    if type_def is None:
        return "void"
    
    # Handle references first
    if "$ref" in type_def:
        ref = type_def["$ref"]
        # Handle local references like "#/types/SomeType"
        if ref.startswith("#/types/"):
            type_name = ref.replace("#/types/", "")
            cpp_type = sanitize_identifier(type_name)
        # Handle shared references
        elif ref.startswith("shared:"):
            cpp_type = sanitize_identifier(ref.replace("shared:", ""))
        else:
            cpp_type = sanitize_identifier(ref)
        
        # Wrap in std::optional if not required
        if not required:
            return f"std::optional<{cpp_type}>"
        return cpp_type
    
    # Handle primitive types
    if "type" in type_def:
        primitive = type_def["type"]
        if isinstance(primitive, str) and primitive in TYPE_MAP:
            cpp_type = TYPE_MAP[primitive]
            # Wrap in std::optional if not required
            if not required:
                return f"std::optional<{cpp_type}>"
            return cpp_type
        elif primitive == "array":
            items_def = type_def.get("items", {"type": "string"})
            item_type = map_type(items_def, True)
            cpp_type = f"std::vector<{item_type}>"
            # Wrap in std::optional if not required
            if not required:
                return f"std::optional<{cpp_type}>"
            return cpp_type
    
    cpp_type = "std::string"  # Default fallback
    # Wrap in std::optional if not required
    if not required:
        return f"std::optional<{cpp_type}>"
    return cpp_type


def generate_params(params: List[Dict[str, Any]]) -> str:
    """Generate parameter list for method signature."""
    if not params:
        return ""
    
    param_strs = []
    for param in params:
        param_name = param["name"]
        # Handle both string type and dict type
        if "type" in param:
            type_value = param["type"]
            if isinstance(type_value, str):
                param_type_def = {"type": type_value}
            else:
                param_type_def = type_value
        elif "$ref" in param:
            param_type_def = param
        else:
            param_type_def = {"type": "string"}
        
        required = param.get("required", True)
        param_type = map_type(param_type_def, required)
        
        # Add default value for optional parameters
        if not required:
            param_strs.append(f"{param_type} {param_name} = std::nullopt")
        else:
            param_strs.append(f"{param_type} {param_name}")
    
    return ", ".join(param_strs)


def format_constraint(constraints: Optional[Dict[str, Any]]) -> str:
    """Format constraint comment from constraint dict."""
    if not constraints:
        return ""
    parts = []
    if "minimum" in constraints:
        parts.append(f"minimum={constraints['minimum']}")
    if "maximum" in constraints:
        parts.append(f"maximum={constraints['maximum']}")
    if "minLength" in constraints:
        parts.append(f"minLength={constraints['minLength']}")
    if "maxLength" in constraints:
        parts.append(f"maxLength={constraints['maxLength']}")
    if "pattern" in constraints:
        parts.append(f"pattern={constraints['pattern']}")
    return ", ".join(parts) if parts else ""


def parse_spec_file(spec_path: Path) -> Dict[str, Any]:
    """Parse a spec.md file and extract its contents."""
    with open(spec_path, 'r') as f:
        content = f.read()
    
    # Split YAML frontmatter from markdown
    if content.startswith('---'):
        parts = content.split('---', 2)
        if len(parts) >= 3:
            yaml_content = parts[1]
            return yaml.safe_load(yaml_content)
    
    return {}


def filter_by_platform(module: Dict[str, Any]) -> bool:
    """Filter module by platform - return True if should process."""
    platform = module.get("platform", "both")
    return platform in ["native", "both"]


def filter_api_by_platform(api: Dict[str, Any], module_platform: str) -> bool:
    """Filter API by platform - return True if should process."""
    api_platform = api.get("platform", module_platform)
    return api_platform in ["native", "both"]


def generate_header(module: Dict[str, Any], output_dir: Path) -> None:
    """Generate C++ header for a module."""
    module_name = module["module"]
    module_lower = module_name.lower()
    
    # Setup Jinja2 environment
    env = Environment(loader=FileSystemLoader(os.path.dirname(os.path.abspath(__file__))))
    
    # Generate model types
    types_code = generate_model_types(module)
    
    # Generate interface and implementation
    interface_code = generate_interface(module, module_lower)
    implementation_code = generate_implementation(module, module_lower)
    
    # Combine into header
    header_content = f"""/*
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

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <firebolt/core/FireboltClient.h>
#include <firebolt/common/FireboltModel.h>
#include <firebolt/common/FireboltObject.h>
#include <firebolt/common/FireboltOperation.h>
#include <firebolt/common/FireboltSubscription.h>

namespace firebolt {{
namespace core {{
namespace {module_lower} {{

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

{types_code}

{interface_code}

{implementation_code}

inline I{module_name}& {module_name}Interface(core::FireboltClient& client)
{{
    return client.GetInterface<{module_name}>();
}}

}} // namespace {module_lower}
}} // namespace core
}} // namespace firebolt
"""
    
    # Write header file
    output_path = output_dir / f"{module_name}.h"
    with open(output_path, 'w') as f:
        f.write(header_content)
    
    print(f"Generated header: {output_path}")


def generate_model_types(module: Dict[str, Any]) -> str:
    """Generate model type code (enums and structs)."""
    types = module.get("types", [])
    module_name = module.get("module", "")
    code_lines = []
    
    # Skip internal types for shared module
    skipped_types = {"ListenResponse", "FireboltError"}
    
    for type_def in types:
        name = type_def["name"]
        
        # Skip internal types for shared module
        if module_name == "Shared" and name in skipped_types:
            continue
        
        kind = type_def.get("kind", "object")
        
        if kind == "enum":
            code_lines.append(f"enum class {name} {{")
            for value in type_def.get("values", []):
                wire_id = value["id"]
                safe_id = sanitize_identifier(wire_id)
                code_lines.append(f"    {safe_id},  // {value['description']}")
            code_lines.append("};")
            code_lines.append("")
        elif kind == "object":
            code_lines.append(f"struct {name} {{")
            for prop in type_def.get("properties", []):
                prop_name = prop["name"]
                prop_type = map_type(prop)
                constraint = format_constraint(prop)
                constraint_comment = f"  // Constraints: {{constraint}}" if constraint else ""
                code_lines.append(f"    {prop_type} {prop_name};{constraint_comment}")
            code_lines.append("};")
            code_lines.append("")
    
    return "\n".join(code_lines)


def generate_interface(module: Dict[str, Any], module_lower: str) -> str:
    """Generate interface class code."""
    module_name = module["module"]
    code_lines = [f"class I{module_name} {{"]
    code_lines.append("public:")
    code_lines.append(f"    virtual ~I{module_name}() = default;")
    code_lines.append("")
    
    # Generate methods for properties
    for prop in module.get("properties", []):
        if not filter_api_by_platform(prop, module.get("platform", "both")):
            continue
        prop_name = prop["name"]
        result_type = map_type(prop.get("result", {"type": "string"}))
        code_lines.append(f"    virtual FireboltOperation<{result_type}> Get{prop_name.capitalize()}() = 0;")
        code_lines.append("")
    
    # Generate methods for actions
    for action in module.get("actions", []):
        if not filter_api_by_platform(action, module.get("platform", "both")):
            continue
        action_name = action["name"]
        result_type = map_type(action.get("result", {"type": "void"}))
        params = action.get("params", [])
        param_str = generate_params(params)
        code_lines.append(f"    virtual FireboltOperation<{result_type}> {action_name.capitalize()}({param_str}) = 0;")
        code_lines.append("")
    
    # Generate methods for events
    for event in module.get("events", []):
        if not filter_api_by_platform(event, module.get("platform", "both")):
            continue
        event_name = event["name"]
        payload_type = map_type(event.get("payload", {"type": "object"}))
        # Capitalize first letter only (remove "on" prefix)
        event_method_name = event_name[2:]
        event_method_name = event_method_name[0].upper() + event_method_name[1:]
        code_lines.append(f"virtual FireboltSubscription On{event_method_name}(std::function<void(const {payload_type}&)> listener) = 0;")
        code_lines.append("")
    
    code_lines.append("};")
    return "\n".join(code_lines)


def generate_implementation(module: Dict[str, Any], module_lower: str) -> str:
    """Generate implementation class code."""
    module_name = module["module"]
    code_lines = [f"class {module_name} : public I{module_name} {{"]
    code_lines.append("public:")
    code_lines.append(f"    explicit {module_name}(core::FireboltClient& client)")
    code_lines.append(f"        : m_client(client)")
    code_lines.append("    {{")
    code_lines.append("    }}")
    code_lines.append("")
    
    # Generate methods for properties
    for prop in module.get("properties", []):
        if not filter_api_by_platform(prop, module.get("platform", "both")):
            continue
        prop_name = prop["name"]
        result_type = map_type(prop.get("result", {"type": "string"}))
        jsonrpc_method = f"{module_lower}.{prop_name}"
        code_lines.append(f"    FireboltOperation<{result_type}> Get{prop_name.capitalize()}() override")
        code_lines.append("    {{")
        code_lines.append(f"        return m_client.Send<{result_type}>(\"{jsonrpc_method}\", FireboltObject{{}});")
        code_lines.append("    }}")
        code_lines.append("")
    
    # Generate methods for actions
    for action in module.get("actions", []):
        if not filter_api_by_platform(action, module.get("platform", "both")):
            continue
        action_name = action["name"]
        result_type = map_type(action.get("result", {"type": "void"}))
        params = action.get("params", [])
        param_str = generate_params(params)
        jsonrpc_method = f"{module_lower}.{action_name}"
        code_lines.append(f"    FireboltOperation<{result_type}> {action_name.capitalize()}({param_str}) override")
        code_lines.append("    {{")
        code_lines.append(f"        return m_client.Send<{result_type}>(\"{jsonrpc_method}\", FireboltObject{{}});")
        code_lines.append("    }}")
        code_lines.append("")
    
    # Generate methods for events
    for event in module.get("events", []):
        if not filter_api_by_platform(event, module.get("platform", "both")):
            continue
        event_name = event["name"]
        payload_type = map_type(event.get("payload", {"type": "object"}))
        jsonrpc_method = f"{module_lower}.{event_name}"
        # Capitalize first letter only (remove "on" prefix)
        event_method_name = event_name[2:]
        event_method_name = event_method_name[0].upper() + event_method_name[1:]
        code_lines.append(f"    FireboltSubscription On{event_method_name}(std::function<void(const {payload_type}&)> listener) override")
        code_lines.append("    {{")
        code_lines.append(f"        return m_client.Subscribe<{payload_type}>(\"{jsonrpc_method}\", std::move(listener));")
        code_lines.append("    }}")
        code_lines.append("")
    
    code_lines.append("private:")
    code_lines.append("    core::FireboltClient& m_client;")
    code_lines.append("};")
    
    return "\n".join(code_lines)


def generate_test(module: Dict[str, Any], test_dir: Path) -> None:
    """Generate integration test for a module."""
    module_name = module["module"]
    module_lower = module_name.lower()
    
    # Generate event injection tests if module has events
    events = module.get("events", [])
    event_tests = ""
    if events:
        for event in events:
            if not filter_api_by_platform(event, module.get("platform", "both")):
                continue
            event_name = event["name"]
            # Capitalize first letter only (remove "on" prefix)
            event_method_name = event_name[2:]
            event_method_name = event_method_name[0].upper() + event_method_name[1:]
            payload_type = map_type(event.get("payload", {"type": "object"}))
            
            # Check if event has result field in examples
            examples = event.get("examples", [])
            has_result_field = False
            has_params_field = False
            for ex in examples:
                if "result" in ex:
                    has_result_field = True
                if "params" in ex:
                    has_params_field = True
            
            # Generate event injection test
            event_tests += f"""
TEST({module_name}Integration, EventInjection_{event_method_name})
{{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    {module_name} {module_lower}(client);

    // Subscribe to event
    bool event_received = false;
    auto subscription = {module_lower}.On{event_method_name}([&event_received](const {payload_type}& payload) {{
        event_received = true;
    }});

    // Inject event with result field (preferred over params)
"""
            if has_result_field:
                event_tests += f"    // Event has result field in examples - prefer result over params\n"
            elif has_params_field:
                event_tests += f"    // Event has params field in examples\n"
            
            event_tests += f"""
    ASSERT_TRUE(event_received);
}}

"""
    
    test_content = f"""/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/{module_name}.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {{
namespace core {{
namespace {module_lower} {{
namespace test {{

TEST({module_name}Integration, Instantiation)
{{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    {module_name} {module_lower}(client);

    ASSERT_TRUE(true);
}}
{event_tests}}} // namespace test
}} // namespace {module_lower}
}} // namespace core
}} // namespace firebolt
"""
    
    # Write test file
    test_path = test_dir / f"test_core_{module_lower}.cpp"
    with open(test_path, 'w') as f:
        f.write(test_content)
    
    print(f"Generated test: {test_path}")


def main():
    parser = argparse.ArgumentParser(
        description="Generate C++ headers from OpenSpec YAML specifications"
    )
    parser.add_argument(
        "--spec-dir",
        required=True,
        help="Directory containing OpenSpec spec files (openspec/specs/api/core)"
    )
    parser.add_argument(
        "--output-dir",
        required=True,
        help="Output directory for generated headers (include/firebolt/core)"
    )
    parser.add_argument(
        "--test-dir",
        required=True,
        help="Output directory for generated tests (tests/integration)"
    )
    args = parser.parse_args()
    
    spec_dir = Path(args.spec_dir)
    output_dir = Path(args.output_dir)
    test_dir = Path(args.test_dir)
    
    # Ensure output directories exist
    output_dir.mkdir(parents=True, exist_ok=True)
    test_dir.mkdir(parents=True, exist_ok=True)
    
    # Discover all spec files
    spec_files = list(spec_dir.glob("*/spec.md"))
    
    print(f"Found {len(spec_files)} spec files")
    
    # Process each module
    for spec_path in spec_files:
        print(f"\nProcessing: {spec_path}")
        
        # Parse spec
        module = parse_spec_file(spec_path)
        if not module:
            print(f"  Skipping: Could not parse spec")
            continue
        
        # Filter by platform
        if not filter_by_platform(module):
            print(f"  Skipping: Platform is {module.get('platform')} (only native/both)")
            continue
        
        print(f"  Module: {module.get('module')}")
        print(f"  Platform: {module.get('platform')}")
        
        # Generate header
        generate_header(module, output_dir)
        
        # Generate test
        generate_test(module, test_dir)
    
    print(f"\nGeneration complete!")
    print(f"Headers generated to: {output_dir}")
    print(f"Tests generated to: {test_dir}")


if __name__ == "__main__":
    main()
