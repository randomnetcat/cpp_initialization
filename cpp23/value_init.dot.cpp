#ifndef VALUE_INIT_INCLUDED
#define VALUE_INIT_INCLUDED

#include "common.h"

#define NODE_VALUE_INIT_HEAD value_initialization_head
#define INST_VALUE_INIT(id, ...) { INSTRUCTION_NODE(id, "The object is value-initialized.", __VA_ARGS__); } -> NODE_VALUE_INIT_HEAD
#define INTERNALLY_VALUE_INITS(id) { id -> NODE_VALUE_INIT_HEAD [style="dotted"]; }

// [dcl.init.general]/9
subgraph cluster_value_initialization {
	INSTRUCTION_NODE(NODE_VALUE_INIT_HEAD, "Value-initialization", "[dcl.init.general]/9")
	-> value_is_class

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

#endif
