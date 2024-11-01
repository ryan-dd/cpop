#include <cassert>
#include <optional>
#include <print>
#include <string>
#include <vector>

#include "cpop/error.h"
#include "cpop/params.h"
#include "cpop/tree.h"
#include "cpop/populate.h"

int main() {
  try {
    struct Config {
      cpop::Param<int> port{"port"};
      cpop::Param<std::string> host{"host"};
      cpop::OptParam<bool> debug{"debug"};
      cpop::OptParam<unsigned int> max_connections{"max_connections"};
    };

    struct ServerInfo {
      cpop::Param<std::string> name{"name"};
      cpop::Param<double> version{"version"};
    };

    struct Service {
      cpop::Param<ServerInfo> server{"server"};
      cpop::OptParam<Config> config{"config"};
    };

    struct Database {
      cpop::Param<std::string> name{"name"};
      cpop::Param<int> port{"port"};
    };

    struct ComplexConfig {
      cpop::Param<Service> primary_service{"primary"};
      cpop::Multiple<Database> databases{"db_list", "database"};
    };

    // Helper function to create a node
    auto makeNode = [](const std::string& value) -> cpop::Content {
        return cpop::Node{value};
    };

    // Helper function to make nested content
    auto makeNested = [](std::vector<cpop::Element> elements) -> cpop::Content {
        return elements;
    };

    // Test 1: Basic types and conversions
    {
        std::println("\nTest 1: Basic types and conversions");
        Config config;
        const cpop::Tree tree = {
            {.key = "port", .content = makeNode("8080")},
            {.key = "host", .content = makeNode("localhost")},
            {.key = "debug", .content = makeNode("true")},
            {.key = "max_connections", .content = makeNode("1000")}
        };

        cpop::populateFromTree(config, tree);
        assert(config.port.value == 8080);
        assert(config.host.value == "localhost");
        assert(config.debug.value.has_value() && config.debug.value.value() == true);
        assert(config.max_connections.value.has_value() && config.max_connections.value.value() == 1000);
        std::println("Basic types test passed");
    }

    // Test 2: Numeric limits
    {
        std::println("\nTest 2: Numeric limits");
        Config config;
        const cpop::Tree tree = {
            {.key = "port", .content = makeNode("2147483648")},  // Exceeds max int
            {.key = "host", .content = makeNode("localhost")},
            {.key = "max_connections", .content = makeNode("4294967296")}  // Exceeds max unsigned int
        };

        try {
          cpop::populateFromTree(config, tree);
        } catch (const cpop::PopulateError& e) {
          std::println("Caught expected error: {}", e.what());
        }
    }

    // Test 3: Boolean conversion
    {
        std::println("\nTest 3: Boolean conversion");
        Config config;
        cpop::Tree tree = {
            {.key = "port", .content = makeNode("8080")},
            {.key = "host", .content = makeNode("localhost")},
            {.key = "debug", .content = makeNode("true")},
        };

        cpop::populateFromTree(config, tree);
        assert(config.debug.value.has_value() && config.debug.value.value() == true);

        // Test invalid boolean
        tree = {
            {.key = "port", .content = makeNode("8080")},
            {.key = "host", .content = makeNode("localhost")},
            {.key = "debug", .content = makeNode("invalid_bool")},  // Should generate warning and not set value
        };

        config.debug.value = std::nullopt;  // Reset value
        cpop::populateFromTree(config, tree);
        assert(!config.debug.value.has_value());
        std::println("Boolean conversion test passed");
    }

    // Test 4: Nested structures
    {
        std::println("\nTest 4: Nested structures");
        Service service;
        const cpop::Tree tree = {
            {.key = "server", .content = makeNested({
                {.key = "name", .content = makeNode("main_server")},
                {.key = "version", .content = makeNode("1.0")}
            })},
            {.key = "config", .content = makeNested({
                {.key = "port", .content = makeNode("8080")},
                {.key = "host", .content = makeNode("localhost")},
                {.key = "debug", .content = makeNode("true")}
            })}
        };

        cpop::populateFromTree(service, tree);
        assert(service.server.value.name.value == "main_server");
        assert(service.server.value.version.value == 1.0);
        assert(service.config.value.has_value());
        assert(service.config.value.value().port.value == 8080);
        std::println("Nested structures test passed");
    }

    // Test 5: Multiple fields (lists)
    {
        std::println("\nTest 5: Multiple fields");
        ComplexConfig complex;
        const cpop::Tree tree = {
            {.key = "primary", .content = makeNested({
                {.key = "server", .content = makeNested({
                    {.key = "name", .content = makeNode("primary_server")},
                    {.key = "version", .content = makeNode("1.0")}
                })},
            })},
            {.key = "db_list", .content = makeNested({
                {.key = "database", .content = makeNested({
                    {.key = "name", .content = makeNode("db1")},
                    {.key = "port", .content = makeNode("5432")}
                })},
                {.key = "database", .content = makeNested({
                    {.key = "name", .content = makeNode("db2")},
                    {.key = "port", .content = makeNode("5433")}
                })}
            })}
        };

        cpop::populateFromTree(complex, tree);
        assert(complex.primary_service.value.server.value.name.value == "primary_server");
        assert(complex.databases.values.size() == 2);
        assert(complex.databases.values[0].name.value == "db1");
        assert(complex.databases.values[0].port.value == 5432);
        assert(complex.databases.values[1].name.value == "db2");
        assert(complex.databases.values[1].port.value == 5433);
        std::println("Multiple fields test passed");
    }

    // Test 6: Missing required fields
    {
        std::println("\nTest 6: Missing required fields");
        Config config;
        const cpop::Tree tree = {
            {.key = "host", .content = makeNode("localhost")}  // Missing required port
        };

        try {
            populateFromTree(config, tree);
            assert(false && "Should have thrown exception for missing required field");
        } catch (const cpop::PopulateError& e) {
            std::println("Caught expected error for missing field: {}", e.what());
        }
    }

    // Test 7: Type conversion errors
    {
        std::println("\nTest 7: Type conversion errors");
        Config config;
        const cpop::Tree tree = {
            {.key = "port", .content = makeNode("not_a_number")},
            {.key = "host", .content = makeNode("localhost")}
        };

        try {
            populateFromTree(config, tree);
            assert(false && "Should have thrown exception for invalid number conversion");
        } catch (const cpop::PopulateError& e) {
          std::println("Caught expected error for invalid conversion: {}", e.what());
        }
    }

    // Test 8: Empty optional fields
    {
        std::println("\nTest 8: Empty optional fields");
        Config config;
        const cpop::Tree tree = {
            {.key = "port", .content = makeNode("8080")},
            {.key = "host", .content = makeNode("localhost")}
            // Omitting debug and max_connections
        };

        populateFromTree(config, tree);
        assert(!config.debug.value.has_value());
        assert(!config.max_connections.value.has_value());
        std::println("Empty optional fields test passed");
    }

    std::println("\nAll tests completed successfully!");
    return 0;
  }
  catch(const std::exception& e) {
    std::println("Test failed: {}", e.what());
  }
}