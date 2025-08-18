#include "common.h"

digraph initialization {
	compound="true";

	#include "default_init.dot.cpp"
	#include "zero_init.dot.cpp"
	#include "value_init.dot.cpp"
	#include "string_init.dot.cpp"
	#include "reference_init.dot.cpp"
	#include "list_init.dot.cpp"

	start [label="So you want to initialize something?\nN4950 [dcl.init.general]/16\nCopyright Janet Cobb 2025, Licensed CC BY 4.0", style=filled, fillcolor=COLOR_GREEN, shape=box, color=COLOR_GREEN, fontcolor=white]
		start -> is_direct_braced_init_list

	YN_QUESTION_NODE(is_direct_braced_init_list, "Is the initializer a non-parenthesized braced-init-list?", "[dcl.init.general]/16.1", list_initialization_head, is_copy_braced_init_list)
	YN_QUESTION_NODE(is_copy_braced_init_list, "Is the initializer of the form \"= braced-init-list\"?", "[dcl.init.general]/16.1", list_initialization_head, is_dest_reference)

	YN_QUESTION_NODE(is_dest_reference, "Is the destination type a reference type?", "[dcl.init.general]/16.2", reference_initialization_head, is_char_arr_init_char:n)

	YN_QUESTION_NODE(is_char_arr_init_char, "Is the destination type an array of char?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_uchar)
	YN_QUESTION_NODE(is_char_arr_init_uchar, "Is the destination type an array of unsigned char?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_schar)
	YN_QUESTION_NODE(is_char_arr_init_schar, "Is the destination type an array of signed char?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_char8)
	YN_QUESTION_NODE(is_char_arr_init_char8, "Is the destination type an array of char8_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_char16)
	YN_QUESTION_NODE(is_char_arr_init_char16, "Is the destination type an array of char16_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_char32)
	YN_QUESTION_NODE(is_char_arr_init_char32, "Is the destination type an array of char32_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_wchar)
	YN_QUESTION_NODE(is_char_arr_init_wchar, "Is the destination type an array of wchar_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_initializer_empty_parens)

	YN_QUESTION_NODE(is_char_arr_literal_init, "Is the initializer a string literal?", "[dcl.init.general]/16.3", STRING_LITERAL_INIT_HEAD, is_initializer_empty_parens)

	YN_QUESTION_NODE(is_initializer_empty_parens, "Is the initializer \"()\"?", "[dcl.init.general]/16.4", empty_parens_value_init, is_dest_array)
	INST_VALUE_INIT(empty_parens_value_init, "[dcl.init/16.3]")

	YN_QUESTION_NODE(is_dest_array, "Is the destination type an array?", "[dcl.init.general]/16.5", array_initialization_head, is_dest_class_type)

	subgraph array_initialization {
		INIT_AS_FOLLOWS(array_initialization_head, "[dcl.init.general]/16.5")
			array_initialization_head -> array_is_paren_expr_list

		YN_QUESTION_NODE(array_is_paren_expr_list, "Is the initializer of the form \"( expression-list )\"?", "CWG2824 resolution", array_x_definition, NEW_ILL_FORMED())

		INSTRUCTION_NODE(array_x_definition, "Let x_1, ..., x_k be the elements of the expression-list.")
		array_x_definition -> array_is_dest_unknown

		YN_QUESTION_NODE_NO_CITE(array_is_dest_unknown, "Is the destination type an array of unknown bound?", array_unknown_dest, array_n_definition)

		INSTRUCTION_NODE(array_unknown_dest, "The destination type is defined as having k elements.")
		-> array_n_definition

		INSTRUCTION_NODE(array_n_definition, "Let n be the size of the array (after adjustment).")
		array_n_definition -> array_is_k_gt_n

		YN_QUESTION_NODE_NO_CITE(array_is_k_gt_n, "Is k > n?", NEW_ILL_FORMED(), array_copy_init_k)

		INSTRUCTION_NODE(array_copy_init_k, "For each 1 <= i <= k, copy-initialize the i-th array element with x_i.")
		-> INSTRUCTION_NODE(array_value_init_n, "For each k < i <= n, value-initialize the i-th array element.")
		-> NEW_DONE()

		INTERNALLY_RECURSES_SUBOBJECT(array_copy_init_k)
		INTERNALLY_VALUE_INITS(array_value_init_n)
	}

	YN_QUESTION_NODE(is_dest_class_type, "Is the destination type a (possibly cv-qualified) class type?", "[dcl.init.general]/16.6", class_dest_initialization_head, is_source_class_type)

	YN_QUESTION_NODE(is_source_class_type, "Is the source type a (possibly cv-qualified) class type?", "[dcl.init.general]/16.7", class_source_initialization_head, is_direct_init_for_nullptr)

	YN_QUESTION_NODE(is_direct_init_for_nullptr, "Is the initialization direct-initialization?", "[dcl.init.general]/16.8", is_source_type_nullptr, standard_conv_seq_initialization_head)

	YN_QUESTION_NODE(is_source_type_nullptr, "Is the source type std::nullptr_t?", "[dcl.init.general]/16.8", is_dest_type_bool_for_nullptr, standard_conv_seq_initialization_head)

	YN_QUESTION_NODE(is_dest_type_bool_for_nullptr, "Is the destination type bool?", "[dcl.init.general]/16.8", nullptr_to_bool_init, standard_conv_seq_initialization_head)

	INSTRUCTION_NODE(nullptr_to_bool_init, "Initialize the bool to false.", "[dcl.init.general]/16.8")
	-> NEW_DONE()

	subgraph class_dest_initialization {
		INIT_AS_FOLLOWS(class_dest_initialization_head, "[dcl.init.general]/16.6")
		-> class_is_initializer_prvalue

		YN_QUESTION_NODE(class_is_initializer_prvalue, "Is the initializer a prvalue?", "[dcl.init.general]/16.6.1", class_is_initializer_prvalue_same_class, class_is_copy_init)

		YN_QUESTION_NODE(class_is_initializer_prvalue_same_class, "Is the cv-unqualified version of the source type the same class as the class of the destination?", "[dcl.init.general]/16.6.1", class_initialize_by_prvalue, class_is_copy_init)

		INSTRUCTION_NODE(class_initialize_by_prvalue, "Use the prvalue to initialize the destination object.", "[dcl.init.general]/16.6.1")
		-> NEW_DONE()

		YN_QUESTION_NODE(class_is_copy_init, "Is the initialization copy-initialization?", "[dcl.init.general]/16.6.2", class_is_copy_init_same_class, class_is_direct_init)

		YN_QUESTION_NODE(class_is_copy_init_same_class, "Is the cv-unqualified version of the source type the same class as the class of the destination?", "[dcl.init.general]/16.6.2", class_consider_constructors, class_is_copy_init_derived_class)
		YN_QUESTION_NODE(class_is_copy_init_derived_class, "Is the cv-unqualified version of the source type a derived class of the class of the destination?", "[dcl.init.general]/16.6.2", class_consider_constructors, class_user_defined_conv_head)

		INSTRUCTION_NODE(class_is_direct_init, "The initialization is direct-initialization.", "[dcl.init.general]/16.6.2")
			class_is_direct_init -> class_consider_constructors

		INSTRUCTION_NODE(class_consider_constructors, "Enumerate applicable constructors and select the best through overload resolution.", "[dcl.init.general]/16.6.2")
			class_consider_constructors -> class_constructors_is_resolution_successful

		YN_QUESTION_NODE(class_constructors_is_resolution_successful, "Is overload resolution succesful?", "[dcl.init.general]/16.6.2.1", class_constructors_use_selected, class_is_constructor_viable)

		INSTRUCTION_NODE(class_constructors_use_selected, "Call the selected constructor to initialize the object with the initializer expression or expression-list as its argument(s).", "[dcl.init.general]/16.6.2.1")
		-> NEW_DONE()

		YN_QUESTION_NODE(class_is_constructor_viable, "Is a constructor viable?", "[dcl.init.general]/16.6.2.2", NEW_ILL_FORMED(), class_is_aggregate)
		YN_QUESTION_NODE(class_is_aggregate, "Is the destination type an aggregate class?", "[dcl.init.general]/16.6.2.2", class_aggregate_is_initializer_expr_list, NEW_ILL_FORMED())
		YN_QUESTION_NODE(class_aggregate_is_initializer_expr_list, "Is the initializer a parenthesized expression-list?", "[dcl.init.general]/16.6.2.2", class_aggregate_paren_init_head, NEW_ILL_FORMED())

		subgraph class_aggregate_paren_init {
			INIT_AS_FOLLOWS(class_aggregate_paren_init_head, "[dcl.init.general]/16.6.2.2")
			-> INSTRUCTION_NODE(class_aggregate_paren_e_defn, "Let e_1, ..., e_k be the elements of the aggregate.")
			-> INSTRUCTION_NODE(class_aggregate_paren_x_defn, "Let x_1, ..., x_n be the elements of the expression-list.")

			class_aggregate_paren_x_defn -> class_aggregate_paren_is_k_gt_n

			YN_QUESTION_NODE_NO_CITE(class_aggregate_paren_is_k_gt_n, "Is k > n?", NEW_ILL_FORMED(), class_aggregate_paren_initialize_first_k)

			INSTRUCTION_NODE(class_aggregate_paren_initialize_first_k, "For each 1 <= i <= k, copy-initialize e_i with x_i.")
			-> INSTRUCTION_NODE(class_aggregate_paren_initialize_rest, "The remaining elements are initialized with their default member initializers, if any, and are otherwise value-initialized.")
			-> NEW_DONE()

			INTERNALLY_RECURSES_SUBOBJECT(class_aggregate_paren_initialize_first_k)
			INTERNALLY_VALUE_INITS(class_aggregate_paren_initialize_rest)
		}

		subgraph class_user_defined_conv {
			INIT_AS_FOLLOWS(class_user_defined_conv_head, "[dcl.init.general]/16.6.3")
			-> INSTRUCTION_NODE(class_user_defined_conv_enumerate, "Enumerate the user-defined conversions that can convert from the source type to the destination type or (when a conversion function is used) to a derived class thereof (as described in [over.match.copy]).")
			-> INSTRUCTION_NODE(class_user_defined_conv_overload_resolution, "Use overload resolution to select the best conversion.")
			-> class_user_defined_conv_is_possible

			YN_QUESTION_NODE_NO_CITE(class_user_defined_conv_is_possible, "Is the conversion ambiguous or impossible?", NEW_ILL_FORMED(), class_user_defined_conv_do_conversion)

			INSTRUCTION_NODE(class_user_defined_conv_do_conversion, "Call the selected function with the initializer expression as its argument.")
			-> call_user_defined_conv_is_constructor

			YN_QUESTION_NODE_NO_CITE(call_user_defined_conv_is_constructor, "Is the selected function a constructor?", call_user_defined_conv_constructor, class_user_defined_conv_initialize)

			INSTRUCTION_NODE(call_user_defined_conv_constructor, "The call is a prvalue of the cv-unqualified version of the destination type whose result object is initialized by the constructor.")
			call_user_defined_conv_constructor -> class_user_defined_conv_initialize

			INSTRUCTION_NODE(class_user_defined_conv_initialize, "Direct-initialize the destination object using the call.")
			-> NEW_RECURSE()
		}
	}

	subgraph class_source_initialization {
		INIT_AS_FOLLOWS(class_source_initialization_head, "[dcl.init.general]/16.7")
		-> INSTRUCTION_NODE(class_source_enumerate_conversion_functions, "Enumerate the applicable conversion functions (as in [over.match.conv]).")
		-> INSTRUCTION_NODE(class_source_overload_resolution, "Use overload resolution to select the best applicable conversion function.")
		-> class_source_conversion_is_impossible

		YN_QUESTION_NODE_NO_CITE(class_source_conversion_is_impossible, "Is the conversion ambiguous or impossible?", NEW_ILL_FORMED(), class_source_initialize)

		INSTRUCTION_NODE(class_source_initialize, "Call the selected user-defined conversion to convert the initializer expression into the object being initialized.")
		-> NEW_DONE()
	}

	subgraph standard_conv_seq_initialization {
		INIT_AS_FOLLOWS(standard_conv_seq_initialization_head, "[dcl.init.general]/16.9")
		-> INSTRUCTION_NODE(standard_conv_seq_do_init, "Initialize the object using the value of the initializer expression, using a standard conversion sequence if necessary, not considering any user-defined conversions.")
		-> standard_conv_seq_is_possible

		YN_QUESTION_NODE_NO_CITE(standard_conv_seq_is_possible, "Is the conversion possible?", standard_conv_seq_is_bitfield, NEW_ILL_FORMED())
		YN_QUESTION_NODE_NO_CITE(standard_conv_seq_is_bitfield, "Is the object to be initialized a bit-field?", standard_conv_seq_is_bitfield_in_range, NEW_DONE())
		YN_QUESTION_NODE_NO_CITE(standard_conv_seq_is_bitfield_in_range, "Is the value representable by the bit-field?", NEW_DONE(), standard_conv_seq_bitfield_imp_def)

		INSTRUCTION_NODE(standard_conv_seq_bitfield_imp_def, "The value of the bit-field is implementation-defined.")
		-> NEW_DONE()
	}
}
