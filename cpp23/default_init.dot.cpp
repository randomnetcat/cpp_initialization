#ifndef DEFAULT_INIT_INCLUDED
#define DEFAULT_INIT_INCLUDED

#include "common.h"

#define NODE_DEFAULT_INIT_HEAD default_initialization_head

#define DEFAULT_INIT_BACKREF(source) { { source; } -> NODE_DEFAULT_INIT_HEAD [style="dotted"] }
#define INTERNALLY_DEFAULT_INITS(id) { id -> NODE_DEFAULT_INIT_HEAD [style="dotted"]; }

// [dcl.init.general]/7
subgraph cluster_default_initialization {
	INSTRUCTION_NODE(NODE_DEFAULT_INIT_HEAD, "Default-initialization", "[dcl.init.general]/7")
	-> INSTRUCTION_NODE(default_defn_t, "Let the type of the object be T.", "[dcl.init.general]/7")
	-> default_is_class

	// 7,1
	{
		YN_QUESTION_NODE(default_is_class, "Is T a (possibly cv-qualified class type)?", "[dcl.init.general]/7.1", default_class_enumerate, default_is_array)

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

#endif
