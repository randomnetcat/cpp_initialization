#ifndef STRING_INIT_INCLUDED
#define STRING_INIT_INCLUDED

#include "common.h"

#define STRING_LITERAL_INIT_HEAD string_literal_initialization_head

subgraph cluster_string_literal_initialization {
	INSTRUCTION_NODE(STRING_LITERAL_INIT_HEAD, "Initialization as follows:", "[dcl.init.string]")
		STRING_LITERAL_INIT_HEAD -> string_literal_verify_kind

	INSTRUCTION_NODE(string_literal_verify_kind, "Verify array type and literal type match.")
		string_literal_verify_kind -> { string_literal_kind_char, string_literal_kind_char8, string_literal_kind_char16, string_literal_kind_char32, string_literal_kind_wchar, string_literal_kind_other }

	{
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

	LINK_TO_ILL_FORMED(string_literal_kind_other)

	{ string_literal_kind_char, string_literal_kind_char8, string_literal_kind_char16, string_literal_kind_char32, string_literal_kind_wchar } -> string_literal_initialize_first

	INSTRUCTION_NODE(string_literal_initialize_first, "Initialize the first elements of the array with successive values from the string literal.")
	-> string_literal_has_too_many

	YN_QUESTION_NODE_NO_CITE(string_literal_has_too_many, "Are there more initializers than array elements?", string_literal_ill_formed_too_many, string_literal_initialize_rest)

	ILL_FORMED_NODE(string_literal_ill_formed_too_many)

	INSTRUCTION_NODE(string_literal_initialize_rest, "Zero-initialize the remaining elements of the array (if any).")
		LINK_TO_DONE(string_literal_initialize_rest)
}

#endif
