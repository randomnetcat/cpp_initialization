#ifndef LIST_INIT_INCLUDED
#define LIST_INIT_INCLUDED

#include "common.h"
#include "value_init.dot.cpp"
#include "string_init.dot.cpp"

// [dcl.init.list]/3
subgraph list_initialization {
	#include "aggregate_init.dot.cpp"

	INSTRUCTION_NODE(list_initialization_head, "List-initialization", "[dcl.init.list]/3")
	-> INSTRUCTION_NODE(list_initialization_defn_t, "Let the initialized object or reference be of type T.", "[dcl.init.list]/3")
	-> list_has_designated_initializer

	// 3.1
	{
		YN_QUESTION_NODE(list_has_designated_initializer, "Does the braced-init-list contain a designated-initializer-list?", "[dcl.init.list]/3.1", list_designated_initalizer_is_aggregate, list_is_aggregate_class)
		YN_QUESTION_NODE(list_designated_initalizer_is_aggregate, "Is the type an aggregate class?", "[dcl.init.list]/3.1", list_designated_initializer_are_identifiers_valid, NEW_ILL_FORMED())
		YN_QUESTION_NODE(list_designated_initializer_are_identifiers_valid, "Do the ordered identifiers in the designators of the designated-initializer-list form a subsequence of the ordered idenitifiers in the direct non-static data members of the type?", "[dcl.init.list]/3.1", list_designated_initializer_aggregate_init, NEW_ILL_FORMED())

		INSTRUCTION_NODE(list_designated_initializer_aggregate_init, "Aggregate initialization is performed.", "[dcl.init.list]/3.1")
		-> AGGREGATE_INIT_HEAD
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
		YN_QUESTION_NODE(list_char_array_is_singleton, "Does the initializer list have a single element?", "[dcl.init.list/]3.3", list_char_array_singleton_is_typed, list_is_aggregate)
		YN_QUESTION_NODE(list_char_array_singleton_is_typed, "Is that element an appropriately-typed string-literal?", "[dcl.init.list]/3.3", list_char_array_string_literal_init, list_is_aggregate)

		INSTRUCTION_NODE(list_char_array_string_literal_init, "Initialization as in [dcl.init.string]", "[dcl.init.list]/3.3")
		-> STRING_LITERAL_INIT_HEAD
	}

	// 3.4
	{
		YN_QUESTION_NODE(list_is_aggregate, "Is T an aggregate?", "[dcl.init.list]/3.4", list_aggregate_aggregate_initialization, list_is_list_empty)

		INSTRUCTION_NODE(list_aggregate_aggregate_initialization, "Aggregate initialization is performed.", "[dcl.init.list]/3.4")
		-> AGGREGATE_INIT_HEAD
	}

	// 3.5
	{
		YN_QUESTION_NODE(list_is_list_empty, "Is the initializer list empty?", "[dcl.init.list]/3.5", list_empty_is_class, list_dest_is_initializer_list)
		YN_QUESTION_NODE(list_empty_is_class, "Is T a class type?", "[dcl.init.list]/3.5", list_empty_has_default_constructor, list_dest_is_initializer_list)
		YN_QUESTION_NODE(list_empty_has_default_constructor, "Does T have a default constructor?", "[dcl.init.list]/3.5", list_empty_value_initialize, list_dest_is_initializer_list)

		INST_VALUE_INIT(list_empty_value_initialize, "[dcl.init.list]/3.5")
	}

	// 3.6
	{
		YN_QUESTION_NODE(list_dest_is_initializer_list, "Is T a specialization of std::initializer_list?", "[dcl.init.list]/3.6", list_initializer_list_init, list_is_class)

		INIT_AS_FOLLOWS(list_initializer_list_init, "[dcl.init.list]/5")
		-> INSTRUCTION_NODE(list_initializer_list_defn_e, "Let the object be of type std::initializer_list<E>.")
		-> INSTRUCTION_NODE(list_initializer_list_defn_n, "Let N be the number of elements in the initalizer list.")
		-> INSTRUCTION_NODE(list_initializer_list_materialize_array, "Materialize a prvalue of type \"array of N const E\".")
		-> INSTRUCTION_NODE(list_initializer_list_init_array, "Copy-initialize each element of the array with the corresponding element of the initializer list.")
		-> list_initializer_list_is_narrowing

		INTERNALLY_RECURSES_SUBOBJECT(list_initializer_list_init_array)

		YN_QUESTION_NODE_NO_CITE(list_initializer_list_is_narrowing, "Is a narrowing conversion required to initialize any of the elements?", NEW_ILL_FORMED(), list_initializer_list_init_object)

		INSTRUCTION_NODE(list_initializer_list_init_object, "Construct the initializer_list to refer to the materialized array.")
		-> NEW_DONE()
	}

	// 3.7
	{
		YN_QUESTION_NODE(list_is_class, "Is T a class type?", "[dcl.init.list]/3.7", list_class_enumerate_ctors, list_is_enum)

		INSTRUCTION_NODE(list_class_enumerate_ctors, "Enumerate the applicable constructors.", "[dcl.init.list]/3.7")
		-> INSTRUCTION_NODE(list_class_select_ctor, "Select the best match selected through overload resolution (applying the special rules in [over.match.list]).", "[dcl.init.list]/3.7")
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
		YN_QUESTION_NODE(list_enum_is_direct, "Is the initialization direct-list-initialization?", "[dcl.init.list]/3.8", list_enum_initialization, list_final_is_singleton)

		INSTRUCTION_NODE(list_enum_initialization, "Initialized the object with the value \"T(v)\" (an explicit type conversion).")
		-> list_enum_is_narrowing

		YN_QUESTION_NODE(list_enum_is_narrowing, "Is a narrowing conversion required to convert v to U?", "[dcl.init.list]/3.8", NEW_ILL_FORMED(), NEW_DONE())
	}

	// 3.9
	{
		// Final just because I couldn't come up with a better name for it. "Final" as in "last".

		YN_QUESTION_NODE(list_final_is_singleton, "Does the initializer list have a single element?", "[dcl.init.list]/3.9", list_final_singleton_type_defn, list_ref_prvalue_is_ref)

		INSTRUCTION_NODE(list_final_singleton_type_defn, "Let E be the type of that element.", "[dcl.init.list]/3.9")
		-> list_final_singleton_is_dest_ref

		YN_QUESTION_NODE(list_final_singleton_is_dest_ref, "Is T a reference type?", "[dcl.init.list]/3.9", list_final_singleton_is_dest_ref_related, list_final_singleton_type)
		YN_QUESTION_NODE(list_final_singleton_is_dest_ref_related, "Is T's referenced type reference-related to E?", "[dcl.init.list]/3.9", list_final_singleton_type, list_ref_prvalue_is_ref)

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

		YN_QUESTION_NODE(list_ref_prvalue_type_is_unknown_bound, "Is T \"reference to array of unknown bound of U\"?", "[dcl.init.list]/3.10", list_ref_prvalue_type_unknown_bound, list_ref_prvalue_type_normal)

		INSTRUCTION_NODE(list_ref_prvalue_type_normal, "The type of the prvalue is the type referenced by T.", "[dcl.init.list]/3.10")
		-> list_ref_prvalue_init_prvalue

		INSTRUCTION_NODE(list_ref_prvalue_type_unknown_bound, "The type of the prvalue is the type of x in U x[] H, where H is the initializer list.", "[dcl.init.list]/3.10")
		-> list_ref_prvalue_init_prvalue

		INSTRUCTION_NODE(list_ref_prvalue_init_prvalue, "The prvalue initializes its result object by copy-list-initialization.", "[dcl.init.list]/3.10")
		-> NEW_RECURSE_SUBOBJECT()
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

#endif
