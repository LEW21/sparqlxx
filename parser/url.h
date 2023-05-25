#pragma once

#include <string>
#include <variant>
#include <optional>
#include <vector>

namespace sparqlxx
{
	class URL {
	public:
		using Host = std::string;
		using Path = std::variant<std::vector<std::string>, std::string>;

		URL(const std::u32string& url, const std::optional<URL>& base = std::nullopt);
		URL(const std::string& url, const std::optional<URL>& base = std::nullopt);

		auto href() const -> std::string;

		std::string scheme;
		std::string username;
		std::string password;
		std::optional<Host> host;
		std::optional<uint16_t> port;
		Path path;
		std::optional<std::string> query;
		std::optional<std::string> fragment;
	};
}
