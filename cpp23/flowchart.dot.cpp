#define TOKEN_CAT_(X, Y) X##Y
#define TOKEN_CAT(X, Y) TOKEN_CAT_(X, Y)

#define DONE_NODE(id) { id [label="Done.", style=filled, fillcolor=green, shape=box, color=green, fontcolor=white]; }
#define ILL_FORMED_NODE(id) { id [label="The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]; }
#define RECURSE_NODE(id) { id [label="Recurse with the new initialization.", shape=box, style=filled, color=blue, fontcolor=white]}

#define NEW_DONE() { DONE_NODE(TOKEN_CAT(__local_done_, __LINE__)); }
#define NEW_ILL_FORMED() { ILL_FORMED_NODE(TOKEN_CAT(__local_ill_formed_, __LINE__)); }
#define NEW_RECURSE() { RECURSE_NODE(TOKEN_CAT(__local_recurse, __LINE__)); }
#define INTERNALLY_RECURSES(id) { id -> { RECURSE_NODE(TOKEN_CAT(id, __recurse)); } [style="dotted"]; }

// Arguments: name of source node, optional edge attrs.
#define LINK_TO_DONE(id, ...) { id -> { DONE_NODE(TOKEN_CAT(id, __generated_done)); } __VA_ARGS__; }
#define LINK_TO_ILL_FORMED(id, ...) { id -> { ILL_FORMED_NODE(TOKEN_CAT(id, __generated_ill_formed)); } __VA_ARGS__; }

// Arguments: name, description (string literal), optional citation.
#define QUESTION_NODE(id, text, ...) { id [shape=diamond, label=text __VA_OPT__(+ "\n" +) __VA_ARGS__]; }
#define INSTRUCTION_NODE(id, text, ...) { id [shape=box, label=text __VA_OPT__(+ "\n" +) __VA_ARGS__]; }

// Arguments: name, description (string literal), citation (string literal), yes node ID, no node ID.
#define YN_QUESTION_NODE(id, text, cite, yes_id, no_id) { \
	QUESTION_NODE(id, text, cite); \
	id -> yes_id [label="Yes"]; \
	id -> no_id [label="No"]; \
}

// Arguments: name, description (string literal), citation (string literal), yes node ID, no node ID.
#define YN_QUESTION_NODE_NO_CITE(id, text, yes_id, no_id) { \
	QUESTION_NODE(id, text); \
	id -> yes_id [label="Yes"]; \
	id -> no_id [label="No"]; \
}

#define VALUE_INIT_TARGET value_initialization_head
#define INST_VALUE_INIT(id, ...) { INSTRUCTION_NODE(id, "The object is value-initialized.", __VA_ARGS__); } -> VALUE_INIT_TARGET
#define INTERNALLY_VALUE_INITS(id) { id -> VALUE_INIT_TARGET [style="dotted"]; }

#define INIT_AS_FOLLOWS(id, ...) { INSTRUCTION_NODE(id, "The object is initialized as follows:", __VA_ARGS__); }

#define LOOP_BACKEDGE(source, loop_header) { source -> loop_header [style="dashed", constraint="true"]; }

digraph initialization {
	compound="true";

	// Define early so that this ends up in the correct cluster.
	subgraph cluster_value_initialization {
		value_initialization_head;
	}

	start [label="So you want to initialize something?\n[dcl.init.general]/16\nCopyright Janet Cobb 2020, Licensed CC BY 4.0", style=filled, fillcolor=green, shape=box, color=green, fontcolor=white]
		start -> is_direct_braced_init_list

	YN_QUESTION_NODE(is_direct_braced_init_list, "Is the initializer a non-parenthesized braced-init-list?", "[dcl.init.general]/16.1", list_initialization_head, is_copy_braced_init_list)
	YN_QUESTION_NODE(is_copy_braced_init_list, "Is the initializer of the form \"= braced-init-list\"?", "[dcl.init.general]/16.1", list_initialization_head, is_dest_reference)

	YN_QUESTION_NODE(is_dest_reference, "Is the destination type a reference type?", "[dcl.init.general]/16.2", reference_initialization_head, is_char_arr_init_char:n)

	YN_QUESTION_NODE(is_char_arr_init_char, "Is the destination type an array of characters?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_char8)
	YN_QUESTION_NODE(is_char_arr_init_char8, "Is the destination type an array of char8_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_char16)
	YN_QUESTION_NODE(is_char_arr_init_char16, "Is the destination type an array of char16_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_char32)
	YN_QUESTION_NODE(is_char_arr_init_char32, "Is the destination type an array of char32_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_char_arr_init_wchar)
	YN_QUESTION_NODE(is_char_arr_init_wchar, "Is the destination type an array of wchar_t?", "[dcl.init.general]/16.3", is_char_arr_literal_init, is_initializer_empty_parens)

	YN_QUESTION_NODE(is_char_arr_literal_init, "Is the initializer a string literal?", "[dcl.init.general]/16.3", string_literal_initialization_head, is_initializer_empty_parens)

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

		INTERNALLY_RECURSES(array_copy_init_k)
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
		YN_QUESTION_NODE(class_is_copy_init_derived_class, "Is the cv-unqualified version of the source type a derived class of the destination type?", "[dcl.init.general]/16.6.2", class_consider_constructors, class_user_defined_conv_head)

		INSTRUCTION_NODE(class_is_direct_init, "The initialization is direct-initialization.", "[dcl.init.general]/16.6.2")
			class_is_direct_init -> class_consider_constructors

		INSTRUCTION_NODE(class_consider_constructors, "Enumerate constructors and select the best through overload resolution.", "[dcl.init.general]/16.6.2")
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

			INTERNALLY_RECURSES(class_aggregate_paren_initialize_first_k)
			INTERNALLY_VALUE_INITS(class_aggregate_paren_initialize_rest)
		}

		subgraph class_user_defined_conv {
			INIT_AS_FOLLOWS(class_user_defined_conv_head, "[dcl.init.general]/16.6.3")
			-> INSTRUCTION_NODE(class_user_defined_conv_enumerate, "Enumerate the user-defined conversions that can convert from the source type to the destination type or (when a conversion function is used) to a derived class thereof (as described in [over.match.copy])")
			-> INSTRUCTION_NODE(class_user_defined_conv_overload_resolution, "Use overload resolution to select the best conversion.")
			-> class_user_defined_conv_is_possible

			YN_QUESTION_NODE_NO_CITE(class_user_defined_conv_is_possible, "Is the conversion ambiguous or impossible?", NEW_ILL_FORMED(), class_user_defined_conv_do_conversion)

			INSTRUCTION_NODE(class_user_defined_conv_do_conversion, "Call the selected function with the initializer-expression as its argument.")
			-> call_user_defined_conv_is_constructor

			YN_QUESTION_NODE_NO_CITE(call_user_defined_conv_is_constructor, "Is the selected function a constructor?", call_user_defined_conv_constructor, class_user_defined_conv_initialize)

			INSTRUCTION_NODE(call_user_defined_conv_constructor, "The call is a prvalue of the cv-unqualified version of the destination type whose result object is initialized by the constructor.")
			call_user_defined_conv_constructor -> class_user_defined_conv_initialize

			INSTRUCTION_NODE(class_user_defined_conv_initialize, "Direct-initialize the destination object using the call.")
			-> NEW_RECURSE()
		}
	}

	subgraph cluster_string_literal_initialization {
		INSTRUCTION_NODE(string_literal_initialization_head, "Initialization as follows:", "[dcl.init.string]")
			string_literal_initialization_head -> string_literal_verify_kind

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

	subgraph class_source_initialization {
		INIT_AS_FOLLOWS(class_source_initialization_head, "[dcl.init.general]/16.7")
		-> INSTRUCTION_NODE(class_source_enumerate_conversion_functions, "Enumerate the applicable conversion functions (as in [over.match.conv]).")
		-> INSTRUCTION_NODE(class_source_overload_resolution, "Use overload resolution to select the best applicable conversion function.")
		-> class_source_conversion_is_impossible

		YN_QUESTION_NODE_NO_CITE(class_source_conversion_is_impossible, "Is the conversion impossible?", class_source_conversion_ill_formed, class_source_conversion_is_ambiguous)
		YN_QUESTION_NODE_NO_CITE(class_source_conversion_is_ambiguous, "Is the conversion ambiguous?", class_source_conversion_ill_formed, class_source_initialize)

		ILL_FORMED_NODE(class_source_conversion_ill_formed)

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

	subgraph cluster_reference_initialization {
		// Header in paragraph 5.
		INSTRUCTION_NODE(reference_initialization_head, "Reference initialization", "[dcl.init.ref]")
		-> INSTRUCTION_NODE(reference_dest_type_defn, "Let the destination be a reference to type \"cv1 T1\".", "[dcl.init.ref]/5")
		-> INSTRUCTION_NODE(reference_source_type_defn, "Let the initializer expression have type \"cv2 T2\".", "[dcl.init.ref]/5")
		-> reference_is_dest_lval_for_step_1

		// 5.1
		{
			YN_QUESTION_NODE(reference_is_dest_lval_for_step_1, "Is the destination an lvalue reference?", "[dcl.init.ref]/5.1", reference_dest_lval_is_source_lval, reference_is_dest_lval_for_step_2)

			// 5.1.1
			{
				YN_QUESTION_NODE(reference_dest_lval_is_source_lval, "Is the initializer expression an lvalue?", "[dcl.init.ref]/5.1.1", reference_dest_lval_is_source_lval_bitfield, reference_dest_lval_is_source_class)
				YN_QUESTION_NODE(reference_dest_lval_is_source_lval_bitfield, "Is the initializer expression a bit-field?", "[dcl.init.ref]/5.1.1", reference_dest_lval_is_source_class, reference_lvals_is_compatible)
				YN_QUESTION_NODE(reference_lvals_is_compatible, "Is cv1 T1 reference-compatible with cv2 T2?", "[dcl.init.ref]/5.1.1", reference_lvals_compatible_bind, reference_dest_lval_is_source_class)

				INSTRUCTION_NODE(reference_lvals_compatible_bind, "The destination reference is bound to the initializer expression lvalue (or appropriate base).", "[dcl.init.ref]/5.1")
				-> NEW_DONE()
			}

			// 5.1.2
			{
				YN_QUESTION_NODE(reference_dest_lval_is_source_class, "Is T2 a class type?", "[dcl.init.ref]/5.1.2", reference_dest_lval_source_class_is_reference_related, reference_is_dest_lval_for_step_2)
				YN_QUESTION_NODE(reference_dest_lval_source_class_is_reference_related, "Is T1 reference-related to T2?", "[dcl.init.ref]/5.1.2", reference_is_dest_lval_for_step_2, reference_dest_lval_source_class_is_convertible)
				YN_QUESTION_NODE(reference_dest_lval_source_class_is_convertible, "Is the initializer expression convertible to an lvalue of type \"cv3 T3\" such that cv1 T1 is reference-compatible with cv3 T3?", "[dcl.init.ref]/5.1.2", reference_class_enumerate_conversions, reference_is_dest_lval_for_step_2)

				INSTRUCTION_NODE(reference_class_enumerate_conversions, "Enumerate applicable conversion functions.", "[dcl.init.ref]/5.1.2")
				-> INSTRUCTION_NODE(reference_class_select_conversion, "Select the best applicable conversion function.", "[dcl.init.ref]/5.1.2")
				-> INSTRUCTION_NODE(reference_class_do_initialization, "The destination reference is bound to the lvalue result of the conversion (or appropriate base).", "[dcl.init.ref]/5.1")
				-> NEW_DONE()
			}
		}

		// 5.2
		{
			YN_QUESTION_NODE(reference_is_dest_lval_for_step_2, "Is the destination an lvalue reference?", "[dcl.init.ref]/5.2", reference_lval_dest_is_const, reference_rval_conv_source_is_rvalue)
			YN_QUESTION_NODE(reference_lval_dest_is_const, "Is the destination a reference to a const-qualified type?", "[dcl.init.ref]/5.2", reference_dest_is_volatile, NEW_ILL_FORMED())
			YN_QUESTION_NODE(reference_dest_is_volatile, "Is the destination a reference to a volatile-qualified type?", "[dcl.init.ref]/5.2", NEW_ILL_FORMED(), reference_rval_conv_source_is_rvalue)
		}

		// 5.3
		{
			// 5.3.1
			{
				YN_QUESTION_NODE(reference_rval_conv_source_is_rvalue, "Is the initializer an rvalue?", "[dcl.init.ref]/5.3.1", reference_rval_conv_source_is_rvalue_bitfield, reference_rval_conv_source_is_lval)
				YN_QUESTION_NODE(reference_rval_conv_source_is_rvalue_bitfield, "Is the initializer a bit-field?", "[dcl.init.ref]/5.3.1", reference_rval_conv_source_is_lval, reference_rval_conv_source_rval_or_function_is_ref_compat)

				YN_QUESTION_NODE(reference_rval_conv_source_is_lval, "Is the initializer an lvalue?", "[dcl.init.ref]/5.3.1", reference_rval_conv_source_lval_is_function, reference_rval_conv_source_is_class)
				YN_QUESTION_NODE(reference_rval_conv_source_lval_is_function, "Is the initializer of function type?", "[dcl.init.ref]/5.3.1", reference_rval_conv_source_rval_or_function_is_ref_compat, reference_rval_conv_source_is_class)

				YN_QUESTION_NODE(reference_rval_conv_source_rval_or_function_is_ref_compat, "Is cv1 T1 reference-compatible with cv2 T2?", "[dcl.init.ref]/5.3.1", reference_rval_conv_bind_direct, reference_rval_conv_source_is_class)

				INSTRUCTION_NODE(reference_rval_conv_bind_direct, "The converted initializer is the value of the initializer.", "[dcl.init.ref]/5.3")
				-> reference_rval_conv_is_converted_prval
			}

			// 5.3.2
			{
				YN_QUESTION_NODE(reference_rval_conv_source_is_class, "Is T2 a class type?", "[dcl.init.ref]/5.3.2", reference_rval_conv_source_class_is_ref_related, reference_temp_is_reference_related)
				YN_QUESTION_NODE(reference_rval_conv_source_class_is_ref_related, "Is T1 reference-related to T2?", "[dcl.init.ref]/5.3.2", reference_temp_is_reference_related, reference_rval_conv_source_class_convertible_target)
				YN_QUESTION_NODE(reference_rval_conv_source_class_convertible_target, "Is the initializer convertible to an rvalue or function lvalue of type \"cv3 T3\", where \"cv1 T1\" is reference-compatible with \"cv3 T3\"?", "[dcl.init.ref]/5.3.2", reference_rval_conv_bind_converted, reference_temp_is_reference_related)

				INSTRUCTION_NODE(reference_rval_conv_bind_converted, "The converted initializer is the result of the conversion.", "[dcl.init.ref]/5.3")
				-> reference_rval_conv_is_converted_prval
			}

			YN_QUESTION_NODE(reference_rval_conv_is_converted_prval, "Is the converted initializer a prvalue?", "[dcl.init.ref]/5.3", reference_rval_conv_prval_t4_defn, reference_rval_conv_bind_glval)

			INSTRUCTION_NODE(reference_rval_conv_prval_t4_defn, "Let the prvalue have type \"T4\".", "[dcl.init.ref]/5.3")
			-> INSTRUCTION_NODE(reference_rval_conv_prval_materialize, "The prvalue is materialized, considering its type to be \"cv1 T4\".", "[dcl.init.ref]/5.3")
			-> reference_rval_conv_bind_glval

			INSTRUCTION_NODE(reference_rval_conv_bind_glval, "The destination reference is bound to the resulting glvalue (or to an appropriate base class subobject).", "[dcl.init.ref]/5.3")
			-> NEW_DONE()
		}

		// 5.4
		{
			// 5.4.1
			{
				YN_QUESTION_NODE(reference_temp_is_reference_related, "Is T1 reference-related to T2?", "[dcl.init.ref]/5.4", NEW_ILL_FORMED(), reference_temp_is_dest_class)

				YN_QUESTION_NODE(reference_temp_is_dest_class, "Is T1 a class type?", "[dcl.init.ref]/5.4.1", reference_temp_user_defined_conv, reference_temp_is_source_class)
				YN_QUESTION_NODE(reference_temp_is_source_class, "Is T2 a class type?", "[dcl.init.ref]/5.4.1", reference_temp_user_defined_conv, reference_temp_implicit_conv)

				INSTRUCTION_NODE(reference_temp_user_defined_conv, "Consider user-defined conversions for the copy-initialization of an object of type \"cv1 T1\" by user-defined-conversion.", "[dcl.init.ref]/5.4.1")
				-> reference_temp_user_defined_conv_is_ill_formed

				YN_QUESTION_NODE(reference_temp_user_defined_conv_is_ill_formed, "Would the non-reference copy-initialization be ill-formed?", "[dcl.init.ref]/5.4.1", NEW_ILL_FORMED(), reference_temp_user_defined_conv_direct_initialize)

				INSTRUCTION_NODE(reference_temp_user_defined_conv_direct_initialize, "Use the result \"E\" of the call to the conversion function, as described by non-reference copy-initialization, to direct-initialize the reference using the form \"(E)\". For this direct-initialization, user-defined conversions are not considered.", "[dcl.init.ref]/5.4.1")
				-> NEW_RECURSE()
			}

			// 5.4.2
			{
				INSTRUCTION_NODE(reference_temp_implicit_conv, "The initializer expression is implicitly converted to a prvalue of type \"T1\".", "[dcl.init.ref]/5.4.2")
				-> INSTRUCTION_NODE(reference_temp_implicit_conv_materialize, "The temporary is materialized, considering the type of the prvalue to be \"cv1 T1\".", "[dcl.init.ref]/5.4.2")
				-> INSTRUCTION_NODE(reference_temp_implicit_conv_materialize_bind, "The reference is bound to the result.", "[dcl.init.ref]/5.4.2")
				-> NEW_DONE()
			}
		}
	}

	// [dcl.init.general]/9
	subgraph cluster_value_initialization {
		INSTRUCTION_NODE(value_initialization_head, "Value-initialization", "[dcl.init.general]/9")
			value_initialization_head -> value_is_class

		// 9.1
		{
			YN_QUESTION_NODE(value_is_class, "Is the type a (possibly cv-qualified) class type?", "[dcl.init.general]/9.1", value_class_find_dflt_ctor, value_is_array)

			INSTRUCTION_NODE(value_class_find_dflt_ctor, "Let C be the constructor selected to default-initialize the object, if any.", "[dcl.init.general]/9.1")
			-> value_class_dflt_ctor_is_user_provided

			YN_QUESTION_NODE(value_class_dflt_ctor_is_user_provided, "Is C user-provided?", "[dcl.init.general]/9.1", value_class_dflt_ctor_zero_init, value_class_default_initialize)

			INSTRUCTION_NODE(value_class_dflt_ctor_zero_init, "The object is zero-initialized.", "[dcl.init.general]/9.1")
			-> value_class_default_initialize

			INSTRUCTION_NODE(value_class_default_initialize, "The object is default-initialized.", "[dcl.init.general]/9.1")
			-> NEW_DONE()
		}

		// 9.3
		{
			YN_QUESTION_NODE(value_is_array, "Is the type an array type?", "[dcl.init.general]/9.2", value_array_semantic_constraints, value_zero_initialize_fallback)

			YN_QUESTION_NODE(value_array_semantic_constraints, "Are the semantic constraints of value-initializing a hypothetical element met?", "[dcl.init.general]/9.2", value_array_initialize_elements, NEW_ILL_FORMED())
			INSTRUCTION_NODE(value_array_initialize_elements, "Each element of the array is value-initialized.", "[dcl.init.general]/9.2")
			-> NEW_DONE()

			INTERNALLY_VALUE_INITS(value_array_initialize_elements)
		}

		// 9.3
		{
			INSTRUCTION_NODE(value_zero_initialize_fallback, "The object is zero-initialized.", "[dcl.init.general]/9.3")
			-> NEW_DONE()
		}
	}

	// [dcl.init.list]/3
	subgraph list_initialization {
		INSTRUCTION_NODE(list_initialization_head, "List-initialization", "[dcl.init.list]/3")
		-> INSTRUCTION_NODE(list_initialization_defn_t, "Let the initialized object or reference be of type T.", "[dcl.init.list]/3")
		-> list_has_designated_initializer

		// 3.1
		{
			YN_QUESTION_NODE(list_has_designated_initializer, "Does the braced-init-list contain a designated-initializer-list?", "[dcl.init.list]/3.1", list_designated_initalizer_is_aggregate, list_is_aggregate_class)
			YN_QUESTION_NODE(list_designated_initalizer_is_aggregate, "Is the type an aggregate class?", "[dcl.init.list]/3.1", list_designated_initializer_are_identifiers_valid, NEW_ILL_FORMED())
			YN_QUESTION_NODE(list_designated_initializer_are_identifiers_valid, "Do the designators form a subsequence of the ordered idenitifiers in the direct non-static data members of the type?", "[dcl.init.list]/3.1", list_designated_initializer_aggregate_init, NEW_ILL_FORMED())

			INSTRUCTION_NODE(list_designated_initializer_aggregate_init, "Aggregate initialization is performed.", "[dcl.init.list]/3.1")
			-> aggregate_initialization_head
		}

		// 3.2
		{
			YN_QUESTION_NODE(list_is_aggregate_class, "Is the type an aggregate class?", "[dcl.init.list]/3.2", list_aggregate_is_list_singleton, list_is_type_char_array)

			YN_QUESTION_NODE(list_aggregate_is_list_singleton, "Does the initializer list have a single element?", "[dcl.init.list]/3.2", list_aggregate_defn_u, list_is_type_char_array)

			INSTRUCTION_NODE(list_aggregate_defn_u, "Let the sole element have type \"cv U\".")
			-> list_aggregate_is_u_eq_t

			YN_QUESTION_NODE(list_aggregate_is_u_eq_t, "Is U the same as T?", "[dcl.init.list]/3.2", list_aggregate_singleton_type_init_type, list_aggreagte_is_u_derived_t)
			YN_QUESTION_NODE(list_aggreagte_is_u_derived_t, "Is U a class derived from T?", "[dcl.init.list]/3.2", list_aggregate_singleton_type_init_type, list_is_type_char_array)

			QUESTION_NODE(list_aggregate_singleton_type_init_type, "What is the type of initialization?", "[dcl.init.list]/3.2")
				list_aggregate_singleton_type_init_type -> list_aggregate_singleton_type_copy [label="copy-list-initialization"]
				list_aggregate_singleton_type_init_type -> list_aggregate_singleton_type_direct [label="direct-list-initialization"]

			INSTRUCTION_NODE(list_aggregate_singleton_type_copy, "Copy-initialize the object from the sole element.", "[dcl.init.list]/3.2")
			-> NEW_RECURSE()

			INSTRUCTION_NODE(list_aggregate_singleton_type_direct, "Direct-initialize the object from the sole element.", "[dcl.init.list]/3.2")
			-> NEW_RECURSE()
		}

		// 3.3
		{
			YN_QUESTION_NODE(list_is_type_char_array, "Is T a character array?", "[dcl.init.list]/3.3", list_char_array_is_singleton, list_is_aggregate)
			YN_QUESTION_NODE(list_char_array_is_singleton, "Does the iniitializer list have a single element?", "[dcl.init.list/]3.3", list_char_array_singleton_is_typed, list_is_aggregate)
			YN_QUESTION_NODE(list_char_array_singleton_is_typed, "Is that element an appropriately-typed string-literal?", "[dcl.init.list]/3.3", list_char_array_string_literal_init, list_is_aggregate)

			INSTRUCTION_NODE(list_char_array_string_literal_init, "Initialization as in [dcl.init.string]", "[dcl.init.list]/3.3")
			-> string_literal_initialization_head
		}

		// 3.4
		{
			YN_QUESTION_NODE(list_is_aggregate, "Is the type an aggregate?", "[dcl.init.list]/3.4", list_aggregate_aggregate_initialization, list_is_list_empty)

			INSTRUCTION_NODE(list_aggregate_aggregate_initialization, "Aggregate initialization is performed.", "[dcl.init.list]/3.4")
			-> aggregate_initialization_head
		}

		// 3.5
		{
			YN_QUESTION_NODE(list_is_list_empty, "Is the initializer list empty?", "[dcl.init.list]/3.5", list_empty_is_class, list_dest_is_initializer_list)
			YN_QUESTION_NODE(list_empty_is_class, "Is T a class type?", "[dcl.init.list]/3.5", list_empty_has_default_constructor, list_dest_is_initializer_list)
			YN_QUESTION_NODE(list_empty_has_default_constructor, "Does the class have a default constructor?", "[dcl.init.list]/3.5", list_empty_value_initialize, list_dest_is_initializer_list)

			INST_VALUE_INIT(list_empty_value_initialize, "[dcl.init.list]/3.5")
		}

		// 3.6
		{
			YN_QUESTION_NODE(list_dest_is_initializer_list, "Is the type a specialization of std::initializer_list?", "[dcl.init.list]/3.6", list_initializer_list_init, list_is_class)

			INIT_AS_FOLLOWS(list_initializer_list_init, "[dcl.init.list]/5")
			-> INSTRUCTION_NODE(list_initializer_list_defn_e, "Let the object be of type std::initializer_list<E>.")
			-> INSTRUCTION_NODE(list_initializer_list_defn_n, "Let N be the number of elements in the initalizer list.")
			-> INSTRUCTION_NODE(list_initializer_list_materialize_array, "Materialize a prvalue of type \"array of N const E\".")
			-> INSTRUCTION_NODE(list_initializer_list_init_array, "Copy-initialize each element of the array with the corresponding element of the initializer list.")
			-> list_initializer_list_is_narrowing

			INTERNALLY_RECURSES(list_initializer_list_init_array)

			YN_QUESTION_NODE_NO_CITE(list_initializer_list_is_narrowing, "Is a narrowing conversion required to initialize any of the elements?", NEW_ILL_FORMED(), list_initializer_list_init_object)

			INSTRUCTION_NODE(list_initializer_list_init_object, "The initializer_list is constructed to refer to the materialized array.")
			-> NEW_DONE()
		}

		// 3.7
		{
			YN_QUESTION_NODE(list_is_class, "Is T a class type?", "[dcl.init.list]/3.7", list_class_enumerate_ctors, list_is_enum)

			INSTRUCTION_NODE(list_class_enumerate_ctors, "Enumerate the applicable constructors.", "[dcl.init.list]/3.7")
			-> INSTRUCTION_NODE(list_class_select_ctor, "Select the best match is selected through overload resolution (applying the special rules in [over.match.list]).", "[dcl.init.list]/3.7")
			-> list_class_is_narrowing

			YN_QUESTION_NODE(list_class_is_narrowing, "Is a narrowing conversion required to convert any of the arguments?", "[dcl.init.list]/3.7", NEW_ILL_FORMED(), NEW_DONE())
		}

		// 3.8
		{
			YN_QUESTION_NODE(list_is_enum, "Is T an enumeration?", "[dcl.init.list]/3.8", list_enum_is_fixed, list_final_is_singleton)
			YN_QUESTION_NODE(list_enum_is_fixed, "Does T have fixed underlying type?", "[dcl.init.list]/3.8", list_enum_underlying_defn, list_final_is_singleton)

			INSTRUCTION_NODE(list_enum_underlying_defn, "Let U be the underlying type.", "[dcl.init.list]/3.8")
			-> list_enum_is_singleton

			YN_QUESTION_NODE(list_enum_is_singleton, "Does the initializer-list have a single element?", "[dcl.init.list]/3.8", list_enum_elem_defn, list_final_is_singleton)

			INSTRUCTION_NODE(list_enum_elem_defn, "Let v be that element.", "[dcl.init.list]/3.8")
			-> list_enum_is_convertible

			YN_QUESTION_NODE(list_enum_is_convertible, "Can v be implicitly converted to U?", "[dcl.init.list]/3.8", list_enum_is_direct, list_final_is_singleton)
			YN_QUESTION_NODE(list_enum_is_direct, "Is the initialization direct-list-initialization?", "[dcl.init.list]/3.8", list_enum_is_narrowing, list_final_is_singleton)

			YN_QUESTION_NODE(list_enum_is_narrowing, "Is a narrowing conversion required to convert v to U?", "[dcl.init.list]/3.8", NEW_ILL_FORMED(), list_enum_initialization)

			INSTRUCTION_NODE(list_enum_initialization, "The object is initialized with the value \"T(u)\" (an explicit type conversion).")
			-> NEW_DONE()
		}

		// 3.9
		{
			// Final just because I couldn't come up with a better name for it. "Final" as in "last".

			YN_QUESTION_NODE(list_final_is_singleton, "Does the initializer list have a single element?", "[dcl.init.list]/3.9", list_final_singleton_type_defn, list_ref_prvalue_is_ref)

			INSTRUCTION_NODE(list_final_singleton_type_defn, "Let E be the type of that element.", "[dcl.init.list]/3.9")
			-> list_final_singleton_is_dest_ref

			YN_QUESTION_NODE(list_final_singleton_is_dest_ref, "Is T a reference type?", "[dcl.init.list]/3.9", list_final_singleton_is_dest_ref_related, list_final_singleton_type)
			YN_QUESTION_NODE(list_final_singleton_is_dest_ref_related, "Is the T's referenced type reference-related to E?", "[dcl.init.list]/3.9", list_final_singleton_type, list_ref_prvalue_is_ref)

			QUESTION_NODE(list_final_singleton_type, "What is the type of initialization?", "[dcl.init.list]/3.9")
				list_final_singleton_type -> list_final_singleton_direct [label="direct-list-initialization"]
				list_final_singleton_type -> list_final_singleton_copy [label="copy-list-initialization"]

			INSTRUCTION_NODE(list_final_singleton_direct, "Direct-initialize the destination from the element.", "[dcl.init.list]/3.9")
			-> NEW_RECURSE()
			-> list_final_singleton_is_narrowing

			INSTRUCTION_NODE(list_final_singleton_copy, "Copy-initialize the destination from the element.", "[dcl.init.list]/3.9")
			-> NEW_RECURSE()
			-> list_final_singleton_is_narrowing

			YN_QUESTION_NODE(list_final_singleton_is_narrowing, "Is a narrowing conversion required to convert the element to the destination type?", "[dcl.init.list]/3.9", NEW_ILL_FORMED(), NEW_DONE())
		}

		// 3.10
		{
			YN_QUESTION_NODE(list_ref_prvalue_is_ref, "Is T a reference type?", "[dcl.init.list]/3.10", list_ref_prvalue_prvalue_generated, list_final_is_empty)

			INSTRUCTION_NODE(list_ref_prvalue_prvalue_generated, "Generate a prvalue.", "[dcl.init.list]/3.10")
			-> list_ref_prvalue_type_is_unknown_bound

			YN_QUESTION_NODE(list_ref_prvalue_type_is_unknown_bound, "Is the destination type \"referencce to array of unknown bound of U\"?", "[dcl.init.list]/3.10", list_ref_prvalue_type_unknown_bound, list_ref_prvalue_type_normal)

			INSTRUCTION_NODE(list_ref_prvalue_type_normal, "The type of the prvalue is the type referenced by the destination type.", "[dcl.init.list]/3.10")
			-> list_ref_prvalue_init_prvalue

			INSTRUCTION_NODE(list_ref_prvalue_type_unknown_bound, "The type of the prvalue is the type of x in U x[] H, where H is the initializer list.", "[dcl.init.list]/3.10")
			-> list_ref_prvalue_init_prvalue

			INSTRUCTION_NODE(list_ref_prvalue_init_prvalue, "The prvalue initializes its result object by copy-list-initialization.", "[dcl.init.list]/3.10")
			-> NEW_RECURSE()
			-> INSTRUCTION_NODE(list_ref_prvalue_init_ref, "The reference is direct-initialized by the prvalue.", "[dcl.init.list]/3.10")
			-> NEW_RECURSE()
		}

		// 3.11
		{
			// Final, again, as in "last".
			YN_QUESTION_NODE(list_final_is_empty, "Is the initializer list empty?", "[dcl.init.list]/3.11", list_final_empty_value_init, list_nothing_else_ill_formed)

			INST_VALUE_INIT(list_final_empty_value_init, "[dcl.init.list]/3.12")
		}

		ILL_FORMED_NODE(list_nothing_else_ill_formed)

	}

	// [dcl.init.aggr]
	subgraph cluster_aggregate_initialization {
		// 2
		{
			INSTRUCTION_NODE(aggregate_initialization_head, "Aggregate initialization", "[dcl.init.aggr]")
			-> INSTRUCTION_NODE(aggregate_determine_elements, "Determine the elements of the aggregate.", "[dcl.init.aggr]/2")
			-> aggregate_elements_type

			QUESTION_NODE(aggregate_elements_type, "What is the type of the aggregate?", "[dcl.init.aggr]/2")
				aggregate_elements_type -> aggregate_elements_array [label="an array type"]
				aggregate_elements_type -> aggregate_elements_class [label="a class type"]

			INSTRUCTION_NODE(aggregate_elements_array, "The elements are the array elements in increasing subscript order.", "[dcl.init.aggr]/2.1")
				aggregate_elements_array -> aggregate_determine_explicit_init_elems

			INSTRUCTION_NODE(aggregate_elements_class, "The elements are the direct base classes in declaration order, followed by the direct non-static data members that are not members of an anonymous union, in declaration order.", "[dcl.init.aggr]/2.2")
				aggregate_elements_class -> aggregate_determine_explicit_init_elems
		}

		// 3
		{
			INSTRUCTION_NODE(aggregate_determine_explicit_init_elems, "Determine the explicitly initialized elements", "[dcl.init.aggr]/3")
			-> aggregate_explicit_is_designated

			// 3.1
			{
				YN_QUESTION_NODE(aggregate_explicit_is_designated, "Is the initializer list a brace-enclosed designated-initializer-list?", "[dcl.init.aggr]/3.1", aggregate_explicit_desginated_is_class, aggregate_explicit_is_init_list)

				YN_QUESTION_NODE(aggregate_explicit_desginated_is_class, "Is the aggregate of class type?", "[dcl.init.aggr]/3.1", aggregate_explicit_designated_are_designators_valid, NEW_ILL_FORMED())
				YN_QUESTION_NODE(aggregate_explicit_designated_are_designators_valid, "Does each designator's identifier name a direct non-static data member of that class?", "[dcl.init.aggr]/3.1", aggregate_explicit_desginated, NEW_ILL_FORMED())

				INSTRUCTION_NODE(aggregate_explicit_desginated, "The explicitly initialized elements are those named by the designator's initializers (or those that contain the named elements).", "[dcl.init.aggr]/3.1")
				-> aggregate_explicit_is_union
			}

			// 3.2
			{
				YN_QUESTION_NODE(aggregate_explicit_is_init_list, "Is the initializer list an brace-enclosed initializer-list?", "[dcl.init.aggr]/3.2", aggregate_explicit_init_list_defn_n, aggregate_explicit_empty)

				INSTRUCTION_NODE(aggregate_explicit_init_list_defn_n, "Let n be the number of elements in the initializer list.", "[dcl.init.aggr]/3.2")
				-> INSTRUCTION_NODE(aggregate_explicit_init_list, "The explicitly initialized elements are the first n elements of the aggregate.", "[dcl.init.aggr]/3.2")
				-> aggregate_explicit_is_union
			}

			// 3.3
			{
				INSTRUCTION_NODE(aggregate_explicit_empty, "The initializer list is \"{}\", and there are no explicitly initialized elements.", "[dcl.init.aggr]/3.3")
				-> aggregate_explicit_is_union
			}
		}

		// 20
		{
			YN_QUESTION_NODE(aggregate_explicit_is_union, "Is the aggregate a union?", "[dcl.init.aggr]/20", aggregate_explicit_list_union_is_excess_explicit_init, aggregate_initialize_explicit)
			YN_QUESTION_NODE(aggregate_explicit_list_union_is_excess_explicit_init, "Is there more than one explicitly initialized element?", "[dcl.init.aggr]/20", NEW_ILL_FORMED(), aggregate_initialize_explicit)
		}

		// 4
		{
			INSTRUCTION_NODE(aggregate_initialize_explicit, "The explicitly initialized elements are initialized as follows:", "[dcl.init.aggr]/4")
			-> INSTRUCTION_NODE(aggregate_initialize_explicit_foreach, "For each explicitly initialized element...", "[dcl.init.aggr]/4")
			-> aggregate_initialize_explicit_is_anon_union

			// 4.1
			{
				YN_QUESTION_NODE(aggregate_initialize_explicit_is_anon_union, "Is the element an anonymous union member?", "[dcl.init.aggr]/4.1", aggregate_initialize_explicit_union_is_designated, aggregate_initialize_explicit_copy_kind)
				YN_QUESTION_NODE(aggregate_initialize_explicit_union_is_designated, "Is the initializer list a brace-enclosed designated-iniitializer-list?", "[dcl.init.aggr]/4.1", aggregate_initialize_explicit_union_is_duplicate_designator, aggregate_initialize_explicit_copy_kind)

				YN_QUESTION_NODE(aggregate_initialize_explicit_union_is_duplicate_designator, "Is there more than one designator that names a member of that anonymous union member?", "[dcl.init.aggr]/4.1", NEW_ILL_FORMED(), aggregate_initialization_explicit_union_defn_d)

				INSTRUCTION_NODE(aggregate_initialization_explicit_union_defn_d, "Let D be the designated-initializer-clause naming a member of the anonymous union member.", "[dcl.init.aggr]/4.1")
				-> INSTRUCTION_NODE(aggregate_initialize_explicit_union_init, "Initialize the element with the braced-init-list { D }.")
				-> NEW_RECURSE()
				-> aggregate_initialize_explicit_repeat_next
			}

			// 4.2
			{
				QUESTION_NODE(aggregate_initialize_explicit_copy_kind, "What is the kind of brace initializer?", "[dcl.init.aggr]/4.2")
					aggregate_initialize_explicit_copy_kind -> aggregate_initialize_explicit_copy_init_list [label="initializer-list"]
					aggregate_initialize_explicit_copy_kind -> aggregate_initialize_explicit_copy_designated [label="designated-initializer-list"]

				INSTRUCTION_NODE(aggregate_initialize_explicit_copy_init_list, "Copy-initialize the element from the corresponding initializer-clause.", "[dcl.init.aggr]/4.2")
				-> NEW_RECURSE()
				-> aggregate_initialize_explicit_is_narrowing_checked

				INSTRUCTION_NODE(aggregate_initialize_explicit_copy_designated, "Initialize the element with the brace-or-equal-initializer of the corresponding designated-initializer-clause.", "[dcl.init.aggr]/4.2")
				-> NEW_RECURSE()
				-> aggregate_initialize_explicit_is_narrowing_checked

				YN_QUESTION_NODE(aggregate_initialize_explicit_is_narrowing_checked, "Is the initializer of the form \"assignment-expression\" or \"= assignment-expression\"?", "[dcl.init.aggr]/4.2", aggregate_initialize_explicit_is_narrowing, aggregate_initialize_explicit_repeat_next)
				YN_QUESTION_NODE(aggregate_initialize_explicit_is_narrowing, "Is a narrowing conversion required to convert the expression?", "[dcl.init.aggr]/4.2", NEW_ILL_FORMED(), aggregate_initialize_explicit_repeat_next)
			}

			INSTRUCTION_NODE(aggregate_initialize_explicit_repeat_next, "Repeat with the next explicitly initialized element.")
			-> aggregate_initialize_nonexplicit_is_union [label="Once done with all explicitly initialized elements"]

			LOOP_BACKEDGE(aggregate_initialize_explicit_repeat_next, aggregate_initialize_explicit_foreach)
		}

		// 5
		{
			YN_QUESTION_NODE(aggregate_initialize_nonexplicit_is_union, "Is the aggregate a union?", "[dcl.init.aggr]/5", aggregate_initialize_nonexplicit_union_is_list_empty, aggregate_initialize_nonexplicit_nonunion)

			INSTRUCTION_NODE(aggregate_initialize_nonexplicit_nonunion, "For each non-explicitly-initialized element:", "[dcl.init.aggr]/5")
				aggregate_initialize_nonexplicit_nonunion -> aggregate_initialize_nonexplicit_nonunion_has_dflt_mem_init

			// 5.1
			{
				YN_QUESTION_NODE(aggregate_initialize_nonexplicit_nonunion_has_dflt_mem_init, "Does the element have a default member initializer?", "[dcl.init.aggr]/5.1", aggregate_initialize_nonexplicit_nonunion_dflt_mem_init, aggregate_initialize_nonexplicit_nonunion_is_reference)

				INSTRUCTION_NODE(aggregate_initialize_nonexplicit_nonunion_dflt_mem_init, "The element is initialized from that initializer.", "[dcl.init.aggr]/5.1")
				-> NEW_RECURSE()
				-> aggregate_initialize_nonexplicit_nonunion_repeat_next
			}

			// 5.2
			{
				YN_QUESTION_NODE(aggregate_initialize_nonexplicit_nonunion_is_reference, "Is the element a reference?", "[dcl.init.aggr]/5.2", NEW_ILL_FORMED(), aggregate_initialize_nonexplicit_nonunion_copy_init)

				INSTRUCTION_NODE(aggregate_initialize_nonexplicit_nonunion_copy_init, "The element is copy-initialized from an empty initializer list.", "[dcl.init.aggr]/5.2")
				-> NEW_RECURSE()
				-> aggregate_initialize_nonexplicit_nonunion_repeat_next
			}

			INSTRUCTION_NODE(aggregate_initialize_nonexplicit_nonunion_repeat_next, "Repeat with the next non-explicitly-initialized element.")
			-> NEW_DONE() [label="Once done with all explicitly initialized elements"]

			LOOP_BACKEDGE(aggregate_initialize_nonexplicit_nonunion_repeat_next, aggregate_initialize_nonexplicit_nonunion)

			QUESTION_NODE(aggregate_initialize_nonexplicit_union_is_list_empty, "Is the initializer list empty?", "[dcl.init.aggr]/5")
				aggregate_initialize_nonexplicit_union_is_list_empty -> aggregate_initialize_nonexplicit_union_empty_has_dflt [label="Yes"]
				LINK_TO_DONE(aggregate_initialize_nonexplicit_union_is_list_empty, [label="No"])

			// 5.3
			{
				YN_QUESTION_NODE(aggregate_initialize_nonexplicit_union_empty_has_dflt, "Does any variant member of the union have a default member initializer?", "[dcl.init.aggr]/5.4", aggregate_initialize_nonexplicit_union_dflt_mem, aggregate_initialize_nonexplicit_union_first_mem)

				INSTRUCTION_NODE(aggregate_initialize_nonexplicit_union_dflt_mem, "That member is initialized from its default member initializer.", "[dcl.init.aggr]/5.4")
				-> NEW_RECURSE()
				-> NEW_DONE()
			}

			// 5.4
			{
				INSTRUCTION_NODE(aggregate_initialize_nonexplicit_union_first_mem, "The first member of the union (if any) is copy-initialized from an empty initializer list.", "[dcl.init.aggr]/5.5")
				-> NEW_RECURSE()
				-> NEW_DONE()
			}
		}
	}
}
