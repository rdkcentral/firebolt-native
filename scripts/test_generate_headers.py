#!/usr/bin/env python3
"""
Unit tests for generate_headers.py
"""

import unittest
import sys
from pathlib import Path

# Add parent directory to path to import generate_headers
sys.path.insert(0, str(Path(__file__).parent))

from generate_headers import format_constraint, sanitize_identifier, map_type, generate_params, generate_model_types, generate_test


class TestFormatConstraint(unittest.TestCase):
    """Test the format_constraint function."""

    def test_numeric_constraints(self):
        """Test numeric constraint formatting."""
        constraints = {"minimum": 0, "maximum": 100}
        result = format_constraint(constraints)
        self.assertEqual(result, "minimum=0, maximum=100")

    def test_string_constraints(self):
        """Test string constraint formatting."""
        constraints = {"minLength": 1, "maxLength": 50}
        result = format_constraint(constraints)
        self.assertEqual(result, "minLength=1, maxLength=50")

    def test_pattern_constraint(self):
        """Test pattern constraint formatting."""
        constraints = {"pattern": "^[a-zA-Z0-9]+$"}
        result = format_constraint(constraints)
        self.assertEqual(result, "pattern=^[a-zA-Z0-9]+$")

    def test_mixed_constraints(self):
        """Test mixed constraint types."""
        constraints = {"minimum": 0, "maxLength": 100, "pattern": "^[a-z]+$"}
        result = format_constraint(constraints)
        self.assertEqual(result, "minimum=0, maxLength=100, pattern=^[a-z]+$")

    def test_empty_constraints(self):
        """Test empty constraints return empty string."""
        constraints = {}
        result = format_constraint(constraints)
        self.assertEqual(result, "")

    def test_no_constraints(self):
        """Test None constraints return empty string."""
        constraints = None
        result = format_constraint(constraints)
        self.assertEqual(result, "")


class TestSanitizeIdentifier(unittest.TestCase):
    """Test the sanitize_identifier function."""

    def test_simple_identifier(self):
        """Test simple identifier sanitization."""
        result = sanitize_identifier("initializing")
        self.assertEqual(result, "Initializing")

    def test_colon_replacement(self):
        """Test colon replacement in identifiers."""
        result = sanitize_identifier("app:adult")
        self.assertEqual(result, "App_adult")

    def test_dot_replacement(self):
        """Test dot replacement in identifiers."""
        result = sanitize_identifier("hdcp2.2")
        self.assertEqual(result, "Hdcp2_2")

    def test_special_characters(self):
        """Test special character replacement."""
        result = sanitize_identifier("test-value")
        self.assertEqual(result, "Test_value")

    def test_leading_digit(self):
        """Test leading digit prefixing."""
        result = sanitize_identifier("2point0")
        self.assertEqual(result, "_2point0")

    def test_multiple_special_chars(self):
        """Test multiple special characters."""
        result = sanitize_identifier("app:adult.v2")
        self.assertEqual(result, "App_adult_v2")


class TestMapType(unittest.TestCase):
    """Test the map_type function."""

    def test_primitive_bool(self):
        """Test bool type mapping."""
        result = map_type({"type": "bool"})
        self.assertEqual(result, "bool")

    def test_primitive_string(self):
        """Test string type mapping."""
        result = map_type({"type": "string"})
        self.assertEqual(result, "std::string")

    def test_primitive_unsigned(self):
        """Test unsigned type mapping."""
        result = map_type({"type": "unsigned"})
        self.assertEqual(result, "uint64_t")

    def test_primitive_double(self):
        """Test double type mapping."""
        result = map_type({"type": "double"})
        self.assertEqual(result, "double")

    def test_array_type(self):
        """Test array type mapping."""
        result = map_type({"type": "array", "items": {"type": "string"}})
        self.assertEqual(result, "std::vector<std::string>")

    def test_ref_local(self):
        """Test local reference type mapping."""
        result = map_type({"$ref": "#/types/SomeType"})
        self.assertEqual(result, "SomeType")

    def test_ref_shared(self):
        """Test shared reference type mapping."""
        result = map_type({"$ref": "shared:AgePolicy"})
        self.assertEqual(result, "AgePolicy")

    def test_none_type(self):
        """Test None type mapping."""
        result = map_type(None)
        self.assertEqual(result, "void")

    def test_optional_primitive(self):
        """Test optional primitive type mapping."""
        result = map_type({"type": "string"}, required=False)
        self.assertEqual(result, "std::optional<std::string>")

    def test_optional_ref(self):
        """Test optional reference type mapping."""
        result = map_type({"$ref": "#/types/SomeType"}, required=False)
        self.assertEqual(result, "std::optional<SomeType>")

    def test_optional_array(self):
        """Test optional array type mapping."""
        result = map_type({"type": "array", "items": {"type": "string"}}, required=False)
        self.assertEqual(result, "std::optional<std::vector<std::string>>")


class TestGenerateParams(unittest.TestCase):
    """Test the generate_params function."""

    def test_empty_params(self):
        """Test empty parameter list."""
        result = generate_params([])
        self.assertEqual(result, "")

    def test_required_param(self):
        """Test required parameter."""
        params = [{"name": "id", "type": "string", "required": True}]
        result = generate_params(params)
        self.assertEqual(result, "std::string id")

    def test_optional_param(self):
        """Test optional parameter."""
        params = [{"name": "id", "type": "string", "required": False}]
        result = generate_params(params)
        self.assertEqual(result, "std::optional<std::string> id = std::nullopt")

    def test_multiple_params(self):
        """Test multiple parameters."""
        params = [
            {"name": "id", "type": "string", "required": True},
            {"name": "count", "type": "unsigned", "required": False}
        ]
        result = generate_params(params)
        self.assertEqual(result, "std::string id, std::optional<uint64_t> count = std::nullopt")

    def test_param_with_ref(self):
        """Test parameter with reference."""
        params = [{"name": "policy", "$ref": "#/types/AgePolicy", "required": True}]
        result = generate_params(params)
        self.assertEqual(result, "AgePolicy policy")


class TestGenerateModelTypes(unittest.TestCase):
    """Test the generate_model_types function."""

    def test_shared_module_filters_internal_types(self):
        """Test that Shared module skips ListenResponse and FireboltError."""
        module = {
            "module": "Shared",
            "types": [
                {"name": "ListenResponse", "kind": "object", "properties": []},
                {"name": "FireboltError", "kind": "object", "properties": []},
                {"name": "AgePolicy", "kind": "enum", "values": [{"id": "app:adult", "description": "Adult"}]}
            ]
        }
        result = generate_model_types(module)
        # Should only contain AgePolicy, not ListenResponse or FireboltError
        self.assertIn("enum class AgePolicy", result)
        self.assertNotIn("ListenResponse", result)
        self.assertNotIn("FireboltError", result)

    def test_regular_module_includes_all_types(self):
        """Test that regular modules include all types."""
        module = {
            "module": "Device",
            "types": [
                {"name": "DeviceClass", "kind": "enum", "values": [{"id": "stb", "description": "STB"}]},
                {"name": "DeviceInfo", "kind": "object", "properties": []}
            ]
        }
        result = generate_model_types(module)
        # Should contain both types
        self.assertIn("enum class DeviceClass", result)
        self.assertIn("struct DeviceInfo", result)


class TestGenerateTest(unittest.TestCase):
    """Test the generate_test function."""

    def test_module_without_events(self):
        """Test that modules without events don't get event injection tests."""
        module = {
            "module": "Device",
            "properties": [{"name": "uid", "result": {"type": "string"}}],
            "actions": [],
            "events": []
        }
        from pathlib import Path
        import tempfile
        with tempfile.TemporaryDirectory() as tmpdir:
            test_dir = Path(tmpdir)
            generate_test(module, test_dir)
            test_file = test_dir / "test_core_device.cpp"
            self.assertTrue(test_file.exists())
            content = test_file.read_text()
            self.assertIn("TEST(DeviceIntegration, Instantiation)", content)
            self.assertNotIn("EventInjection", content)

    def test_module_with_events(self):
        """Test that modules with events get event injection tests."""
        module = {
            "module": "Lifecycle2",
            "platform": "native",
            "properties": [],
            "actions": [],
            "events": [
                {
                    "name": "onStateChanged",
                    "payload": {"$ref": "#/types/StateChangedEvent"},
                    "examples": [{"result": {"oldState": "paused", "newState": "active"}}]
                }
            ]
        }
        from pathlib import Path
        import tempfile
        with tempfile.TemporaryDirectory() as tmpdir:
            test_dir = Path(tmpdir)
            generate_test(module, test_dir)
            test_file = test_dir / "test_core_lifecycle2.cpp"
            self.assertTrue(test_file.exists())
            content = test_file.read_text()
            self.assertIn("TEST(Lifecycle2Integration, Instantiation)", content)
            self.assertIn("TEST(Lifecycle2Integration, EventInjection_StateChanged)", content)
            self.assertIn("Event has result field in examples - prefer result over params", content)


if __name__ == "__main__":
    unittest.main()
