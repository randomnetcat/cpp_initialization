#ifndef STRING_INIT_INCLUDED
#define STRING_INIT_INCLUDED

#include "common.h"

#define STRING_LITERAL_INIT_HEAD string_literal_initialization_head

#include "zero_init.dot.cpp"

subgraph cluster_string_literal_initialization {
	INSTRUCTION_NODE(STRING_LITERAL_INIT_HEAD, "Initialization as follows:", "[dcl.init.string]")
	-> INSTRUCTION_NODE(string_literal_verify_kind, "Verify array type and literal type match.")
	-> {
		rank=same;

		string_literal_kind_char [label="char[] / ordinary literal"]
		string_literal_kind_char8 [label="char8_t[] / UTF-8 literal"]
		string_literal_kind_char16 [label="char16_t[] / UTF-16 literal"]
		string_literal_kind_char32 [label="char32_t[] / UTF-32 literal"]
		string_literal_kind_wchar [label="wchar_t[] / wide literal"]
		string_literal_kind_other [label="Anything else"]

		// Force these nodes to layout in the following order
		string_literal_kind_char -> string_literal_kind_char8 -> string_literal_kind_char16 -> string_literal_kind_char32 -> string_literal_kind_wchar -> string_literal_kind_other [style=invis]
	}

	string_literal_kind_other -> NEW_ILL_FORMED()

	{ string_literal_kind_char, string_literal_kind_char8, string_literal_kind_char16, string_literal_kind_char32, string_literal_kind_wchar } -> string_literal_initialize_first

	INSTRUCTION_NODE(string_literal_initialize_first, "Initialize the first elements of the array with successive values from the string literal.")
	-> string_literal_has_too_many

	YN_QUESTION_NODE_NO_CITE(string_literal_has_too_many, "Are there more initializers than array elements?", NEW_ILL_FORMED(), string_literal_initialize_rest)

	INSTRUCTION_NODE(string_literal_initialize_rest, "Zero-initialize the remaining elements of the array (if any).")
	-> NEW_DONE()

	INTERNALLY_ZERO_INITS(string_literal_initialize_rest)
}

#endif
