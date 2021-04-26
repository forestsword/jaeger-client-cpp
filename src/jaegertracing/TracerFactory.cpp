/*
 * Copyright (c) 2018 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TracerFactory.h"

#include "jaegertracing/Tracer.h"

namespace jaegertracing {

opentracing::expected<std::shared_ptr<opentracing::Tracer>>
TracerFactory::MakeTracer(const char* configuration,
                          std::string& errorMessage) const noexcept try {
#ifndef JAEGERTRACING_WITH_YAML_CPP
    errorMessage =
        "Failed to construct tracer: Jaeger was not build with yaml support.";
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::not_supported));
#else
    std::cout << "config" << std::endl;
    std::cout << configuration << std::endl;
    YAML::Node yaml;

    if (yaml) {
      std::cout << "DEBUG FIRST YAML";
    } else {
      std::cout << "DEBUG FIRST NO YAML";
    }

    try {
        yaml = YAML::Load(configuration);
        std::cout << "yaml" << std::endl;
    } catch (const YAML::ParserException& e) {
        errorMessage = e.what();
        return opentracing::make_unexpected(
            opentracing::configuration_parse_error);
    }
    yaml = YAML::Load(configuration);

    if (yaml) {
      std::cout << "DEBUG YAML";
    } else {
      std::cout << "DEBUG NO YAML";
    }

    if (yaml == NULL) {
      std::cout << "DEBUG YAML NULL";
    } else {
      std::cout << "DEBUG YAML NOT NULL";
    }

    if (yaml["service_name"]) {
      std::cout << "DEBUG service_name";
      std::cout << yaml["service_name"] << std::endl;
    } else {
      std::cout << "DEBUG no service name?";
    }

    std::cout << "DEBUG call parse" << std::endl;
    auto tracerConfig = jaegertracing::Config::parse(yaml);
    std::cout << "DEBUG parse done?" << std::endl;

    if (_readFromEnv) {
        tracerConfig.fromEnv();
    }

    if (tracerConfig.serviceName().empty()) {
        errorMessage = "`service_name` not provided";
        return opentracing::make_unexpected(
            opentracing::invalid_configuration_error);
    }

    return jaegertracing::Tracer::make(tracerConfig);
#endif  // JAEGERTRACING_WITH_YAML_CPP
} catch (const std::bad_alloc&) {
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::not_enough_memory));
} catch (const std::exception& e) {
    errorMessage = e.what();
    return opentracing::make_unexpected(
        opentracing::invalid_configuration_error);
}
}  // namespace jaegertracing
