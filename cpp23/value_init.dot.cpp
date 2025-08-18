#ifndef VALUE_INIT_INCLUDED
#define VALUE_INIT_INCLUDED

#include "common.h"

#define NODE_VALUE_INIT_HEAD value_initialization_head
#define INST_VALUE_INIT(id, ...) { INSTRUCTION_NODE(id, "Value-initialize the object.", __VA_ARGS__); } -> NODE_VALUE_INIT_HEAD
#define INTERNALLY_VALUE_INITS(id) { id -> NODE_VALUE_INIT_HEAD [style="dotted"]; }

#include "default_init.dot.cpp"
#include "zero_init.dot.cpp"

// [dcl.init.general]/9
subgraph cluster_value_initialization {
	INSTRUCTION_NODE(NODE_VALUE_INIT_HEAD, "Value-initialization", "[dcl.init.general]/9")
	-> value_is_reference

	// 10
	{
		YN_QUESTION_NODE(value_is_reference, "Is the destination a reference?", "[dcl.init.general]/10", NEW_ILL_FORMED(), value_defn_t)
	}

	INSTRUCTION_NODE(value_defn_t, "Let T be the type of the object.", "[dcl.init.general]/9")
	-> value_is_class

	// 9.1
	{
		YN_QUESTION_NODE(value_is_class, "Is the type a (possibly cv-qualified) class type?", "[dcl.init.general]/9.1", value_class_has_default, value_is_array)

		YN_QUESTION_NODE(value_class_has_default, "Does T have a default constructor?", "[dcl.init.general]/9.1", value_class_default_is_deleted, value_class_direct_default_init)
		YN_QUESTION_NODE(value_class_default_is_deleted, "Does T have a default constructor that is deleted?", "[dcl.init.general]/9.1", value_class_direct_default_init, value_class_default_is_user_provided)
		YN_QUESTION_NODE(value_class_default_is_user_provided, "Does T have a default constructor that is user-provided?", "[dcl.init.general]/9.1", value_class_direct_default_init, value_class_dflt_ctor_check_zero)

		INSTRUCTION_NODE(value_class_direct_default_init, "Default-initialize the object.", "[dcl.init.general]/9.1")
		-> NODE_DEFAULT_INIT_HEAD

		INSTRUCTION_NODE(value_class_dflt_ctor_check_zero, "Check the semantic constraints for default-initialization.", "[dcl.init.general]/9.1")
		-> INSTRUCTION_NODE(value_class_dflt_ctor_zero_init, "Zero-initialize the object.", "[dcl.init.general]/9.1")
		-> INSTRUCTION_NODE(value_class_dflt_ctor_default_init, "Default-initialize the object.", "[dcl.init.general]/9.1")
		-> NODE_DEFAULT_INIT_HEAD

		INTERNALLY_ZERO_INITS(value_class_dflt_ctor_zero_init)
	}

	// 9.3
	{
		YN_QUESTION_NODE(value_is_array, "Is the type an array type?", "[dcl.init.general]/9.2", value_array_initialize_elements, value_zero_initialize_fallback)

		INSTRUCTION_NODE(value_array_initialize_elements, "Value-initialize each element of the array.", "[dcl.init.general]/9.2")
		-> NEW_DONE()

		INTERNALLY_VALUE_INITS(value_array_initialize_elements)
	}

	// 9.3
	{
		INSTRUCTION_NODE(value_zero_initialize_fallback, "Zero-initialize the object.", "[dcl.init.general]/9.3")
		-> NODE_ZERO_INIT_HEAD
	}
}

#endif
