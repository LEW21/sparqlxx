#include "../database.h"
#include "../autocomplete.h"
#include <iostream>
#include <signal.h>
#include <setjmp.h>
#include <cstdio>

#ifndef USE_READLINE
#define USE_READLINE 0
#endif

#if USE_READLINE
	#include <sys/stat.h>
	#include <readline/readline.h>
	#include <readline/history.h>
#endif

using namespace std;

void dump(const sparqlxx::Solutions& s)
{
	if (!s.vars.size())
	{
		std::cout << "No columns." << std::endl;
		return;
	}

	for (auto v : s.vars)
		cout << to_sparql(v) << " ";
	cout << endl;

	for (auto r : s.rows)
	{
		for (auto v : r)
			cout << to_sparql(v) << " ";
		cout << endl;
	}
}

auto prompt_start = string{};
auto prompt_continue = string{};

auto buffer = string{};

#if USE_READLINE
constexpr const auto spaces = " \f\n\r\t\v";

std::vector<std::string> matches;
int matches_i;

auto malloc_string(const std::string& s) -> char*
{
	auto out = (char*)malloc(s.size() + 1);
	memcpy(out, s.c_str(), s.size() + 1);
	return out;
}

auto autocomplete(const char* text, int state) -> char*
{
	if (state == 0)
	{
		try
		{
			matches = sparqlxx::autocomplete(buffer + std::string{rl_line_buffer});
			matches_i = 0;
		}
		catch (sparqlxx::parse_error& e)
		{
			std::cerr << std::endl << e.what() << std::endl;
			std::cerr << rl_prompt << rl_line_buffer << std::flush;
		}
	}

	if (matches.size() == 1 && matches[0] == " ")
	{
		matches = {};
		return malloc_string(std::string{text});
	}

	if (matches.size() == 1 && (matches[0] == "<" || matches[0] == "?" || matches[0] == "\"" || matches[0] == "_:"))
		rl_completion_suppress_append = 1;

	if (matches_i < matches.size())
		return malloc_string(matches[matches_i++]);

	return nullptr;
}

static jmp_buf ctrl_c_jmp_buf;

void intHandler(int)
{
	siglongjmp(ctrl_c_jmp_buf, 1);
}

inline auto cppreadline(const std::string& prompt) -> std::string
{
	if (sigsetjmp(ctrl_c_jmp_buf, 1))
		std::cout << std::endl;

	auto cline = readline(prompt.c_str());
	if (!cline)
	{
		std::cin.setstate(std::ios_base::eofbit);
		return {};
	}
	auto line = std::string{cline};
	free(cline);
	return line;
}
#else
auto prompt_cur = string{};

inline auto cppreadline(const std::string& prompt) -> std::string
{
	prompt_cur = prompt;
	std::cout << prompt << std::flush;
	auto line = std::string{};
	getline(std::cin, line);
	return line;
}

void intHandler(int)
{
	std::cout << std::endl << prompt_cur << std::flush;
}
#endif

int main(int argc, char** argv)
{
	auto dsn_str = std::string{(argc >= 2) ? argv[1] : "sparqlite"};
	auto dsn = sparqlxx::parseDSN(dsn_str);

	prompt_start = std::string(dsn.type.size() < 3 ? 3 - dsn.type.size() : 0, ' ') + dsn.type + "> ";
	prompt_continue = std::string(prompt_start.size()-5, ' ') + "...> ";

	auto db = sparqlxx::Database{dsn};

	signal(SIGINT, intHandler);

	#if USE_READLINE
		rl_readline_name = "sparql";
		rl_completion_entry_function = autocomplete;
		rl_completer_word_break_characters = const_cast<char*>(spaces);
	#endif

	while (!cin.eof())
	{
		auto line = cppreadline(!buffer.size() ? prompt_start : prompt_continue);

		if (!line.size())
			continue;

		#if USE_READLINE
			add_history(line.c_str());
		#endif

		buffer += line + "\n";
		try
		{
			auto q = sparqlxx::parse(buffer);
			buffer = {};

			auto r = db.query(q);
			if (r.is<sparqlxx::Solutions>())
				dump(r.get<sparqlxx::Solutions>());
			else
				cout << "OK" << endl;

			cout << endl;
		}
		catch (sparqlxx::parse_error& e)
		{
			if (e.got == "end-of-input")
				continue;

			cout << "Parse error!" << endl;
			cout << e.what() << endl;
			cout << endl;
			buffer = {};
			continue;
		}
	}

	std::cout << std::endl;
}
