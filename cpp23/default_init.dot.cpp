#ifndef DEFAULT_INIT_INCLUDED
#define DEFAULT_INIT_INCLUDED

#include "common.h"

#define NODE_DEFAULT_INIT_HEAD default_initialization_head

#define DEFAULT_INIT_BACKREF(source) { { source; } -> NODE_DEFAULT_INIT_HEAD [style="dotted"] }
#define INTERNALLY_DEFAULT_INITS(id) { id -> NODE_DEFAULT_INIT_HEAD [style="dotted"]; }

#define NEW_IS_CDC() NEW_YES("T is const-default-constructible.")
#define NEW_NOT_CDC() NEW_NO("T is not const-default-constructible.")

#define REFERENCES_CDC(node) { node -> cdc_head [style="dotted"]; }

// Forward declare to avoid cdc_head being in cluster_default_initialization.
subgraph cluster_is_const_default_constructible {
	cdc_head
}

// [dcl.init.general]/7
subgraph cluster_default_initialization {
	INSTRUCTION_NODE(NODE_DEFAULT_INIT_HEAD, "Default-initialization", "[dcl.init.general]/7")
	-> default_is_reference

	// 10
	{
		YN_QUESTION_NODE(default_is_reference, "Is the destination a reference?", "[dcl.init.general]/10", NEW_ILL_FORMED(), default_defn_t)
	}

	INSTRUCTION_NODE(default_defn_t, "Let the type of the object be T.", "[dcl.init.general]/7")
	-> default_is_const

	// 8
	{
		YN_QUESTION_NODE(default_is_const, "Is T const-qualified?", "[dcl.init.general]/8", default_const_is_class, default_is_class)

		YN_QUESTION_NODE(default_const_is_class, "Is T a class type?", "[dcl.init.general]/8", default_const_class_is_cdc, default_const_is_array)
		YN_QUESTION_NODE(default_const_class_is_cdc, "Is T const-default-constructible", "[dcl.init.general]/8", default_is_class, NEW_ILL_FORMED())
		REFERENCES_CDC(default_const_class_is_cdc)

		YN_QUESTION_NODE(default_const_is_array, "Is T an array type?", "[dcl.init.general]/8", default_const_array_element_is_class, NEW_ILL_FORMED())
		YN_QUESTION_NODE(default_const_array_element_is_class, "Is T's element type a class type?", "[dcl.init.general]/8", default_const_array_element_class_is_cdc, NEW_ILL_FORMED())
		YN_QUESTION_NODE(default_const_array_element_class_is_cdc, "Is that class type const-default-constructible?", "[dcl.init.general]/8", default_is_class, NEW_ILL_FORMED())
		REFERENCES_CDC(default_const_array_element_class_is_cdc)
	}

	// 7.1
	{
		YN_QUESTION_NODE(default_is_class, "Is T a (possibly cv-qualified) class type?", "[dcl.init.general]/7.1", default_class_enumerate, default_is_array)

		INSTRUCTION_NODE(default_class_enumerate, "Enumerate the applicable constructors.", "[dcl.init.general]/7.1")
		-> INSTRUCTION_NODE(default_class_select, "Select the best constructor for the initializer \"()\".", "[dcl.init.general]/7.1")
		-> INSTRUCTION_NODE(default_class_init, "Call the selected constructor with an empty argument list to initialize the object", "[dcl.init.general]/7.1")
		-> NEW_DONE()
	}

	// 7.2
	{
		YN_QUESTION_NODE(default_is_array, "Is T an array type?", "[dcl.init.general]/7.2", default_array_init, default_no_init)

		INSTRUCTION_NODE(default_array_init, "Default-initialize each element of the array.", "[dcl.init.general]/7.2")
		-> NEW_DONE()

		DEFAULT_INIT_BACKREF(default_array_init)
	}

	// 7.3
	{
		INSTRUCTION_NODE(default_no_init, "No initialization is performed.", "[dcl.init.general]/7.3")
		-> NEW_DONE()
	}
}

// [dcl.init.general]/8
subgraph cluster_is_const_default_constructible {
	INSTRUCTION_NODE(cdc_head, "To determine if class type T is const-default-constructible:", "[dcl.init.general/8]")
	-> cdc_would_invoke_user_provided

	YN_QUESTION_NODE(cdc_would_invoke_user_provided, "Would default-initialization of T invoke a user-provided constructor of T (not inherited from a base class)?", "[dcl.init.general/8]", NEW_IS_CDC(), cdc_base_is_cdc)

	// 8.4
	YN_QUESTION_NODE(cdc_base_is_cdc, "Is each potentially constructed base class of T const-default-constructible?", "[dcl.init.general]/8.4", cdc_member_each, NEW_NOT_CDC())

	// 8.1
	{
		INSTRUCTION_NODE(cdc_member_each, "For each direct non-variant non-static data member M of T:", "[dcl.init.general]/8.1")
		-> cdc_member_has_default

		YN_QUESTION_NODE_NO_CITE(cdc_member_has_default, "Does M have a default member initializer?", cdc_member_repeat, cdc_member_is_class)

		YN_QUESTION_NODE_NO_CITE(cdc_member_is_class, "Does M have class type?", cdc_member_class_defn_x, cdc_member_is_array_of_class)
		YN_QUESTION_NODE_NO_CITE(cdc_member_is_array_of_class, "Does M have array of class type?", cdc_member_array_defn_x, NEW_NOT_CDC())

		INSTRUCTION_NODE(cdc_member_class_defn_x, "Let X be that class type.")
		-> cdc_member_x_is_cdc

		INSTRUCTION_NODE(cdc_member_array_defn_x, "Let X be the element (class) type.")
		-> cdc_member_x_is_cdc

		YN_QUESTION_NODE_NO_CITE(cdc_member_x_is_cdc, "Is X const-default-constructible?", cdc_member_repeat, NEW_NOT_CDC())
		REFERENCES_CDC(cdc_member_x_is_cdc)

		INSTRUCTION_NODE(cdc_member_repeat, "Repeat with the next non-static data member.")
		-> cdc_is_union [label="Once done with all non-static data members"]

		LOOP_BACKEDGE(cdc_member_repeat, cdc_member_each)
	}

	YN_QUESTION_NODE(cdc_is_union, "Is T a union?", "[dcl.init.general]/8.2-3", cdc_union_has_nsdm, cdc_nonunion_anon_union_each)

	// 8.2
	{
		YN_QUESTION_NODE(cdc_union_has_nsdm, "Does T have at least one non-static data member?", "[dcl.init.general]/8.2", cdc_union_has_one_init_nsdm, NEW_IS_CDC())
		YN_QUESTION_NODE(cdc_union_has_one_init_nsdm, "Does exactly one variant member of T have a default member initializer?", "[dcl.init.general]/8.2", NEW_IS_CDC(), NEW_NOT_CDC())
	}

	// 8.3
	{
		INSTRUCTION_NODE(cdc_nonunion_anon_union_each, "For each anonymous union member of T:", "[dcl.init.general]/8.3")
		-> cdc_nonunion_anon_union_has_nsdm

		YN_QUESTION_NODE_NO_CITE(cdc_nonunion_anon_union_has_nsdm, "Does that anonymous union member have at least one non-static data member?", cdc_nonunion_anon_union_one_init_nsdm, cdc_nonunion_anon_union_repeat)
		YN_QUESTION_NODE_NO_CITE(cdc_nonunion_anon_union_one_init_nsdm, "Does exactly one non-static data member of that anonymous union member have a default member initializer?", cdc_nonunion_anon_union_repeat, NEW_NOT_CDC())

		INSTRUCTION_NODE(cdc_nonunion_anon_union_repeat, "Repeat with the next anonymous data member.")
		-> NEW_IS_CDC() [label="Once done with all anonymous union members"]

		LOOP_BACKEDGE(cdc_nonunion_anon_union_repeat, cdc_nonunion_anon_union_each)
	}
}

#endif
