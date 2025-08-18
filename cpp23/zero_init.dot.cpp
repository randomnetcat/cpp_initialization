#ifndef ZERO_INIT_INCLUDED
#define ZERO_INIT_INCLUDED

#include "common.h"

#define NODE_ZERO_INIT_HEAD zero_initialization_head

#define ZERO_INIT_BACKREF(source) { { source; } -> NODE_ZERO_INIT_HEAD [style="dotted"] }
#define INTERNALLY_ZERO_INITS(id) { id -> NODE_ZERO_INIT_HEAD [style="dotted"]; }

// [dcl.init.general]/9
subgraph cluster_zero_initialization {
	INSTRUCTION_NODE(NODE_ZERO_INIT_HEAD, "Zero-initialization", "[dcl.init.general]/6")
	-> INSTRUCTION_NODE(zero_defn_t, "Let the type of the object or reference be T.", "[dcl.init.general]/6")
	-> zero_is_scalar

	// 6.1
	{
		YN_QUESTION_NODE(zero_is_scalar, "Is T a scalar type?", "[dcl.init.general]/6.1", zero_scalar_init, zero_is_class)
		INSTRUCTION_NODE(zero_scalar_init, "Initialized the object to the value obtained by converting the literal 0 to T.", "[dcl.init.general]/6.1")
		-> NEW_DONE()
	}

	// 6.2-6.3
	{
		YN_QUESTION_NODE(zero_is_class, "Is T a class type?", "[dcl.init.general]/6.2", zero_is_union, zero_is_array)
		YN_QUESTION_NODE(zero_is_union, "Is T a union type?", "[dcl.init.general]/6.2-3", zero_union_padding, zero_class_padding)

		INSTRUCTION_NODE(zero_class_padding, "Initialize the object's padding bits to zero bits.", "[dcl.init.general]/6.2")
		-> INSTRUCTION_NODE(zero_class_members, "Zero-initialize each of the object's non-static data members.", "[dcl.init.general]/6.2")
		-> INSTRUCTION_NODE(zero_class_bases, "Zero-initialize each of the object's non-virtual base class subobjects.", "[dcl.init.general]/6.2")
		-> zero_class_is_base_subobject

		YN_QUESTION_NODE(zero_class_is_base_subobject, "Is the object a base class subobject?", "[dcl.init.general]/6.2", zero_class_virtual_bases, NEW_DONE())

		INSTRUCTION_NODE(zero_class_virtual_bases, "Zero-initialize each of the object's virtual base class subobjects.", "[dcl.init.general]/6.2")
		-> NEW_DONE()

		ZERO_INIT_BACKREF({
			zero_class_members zero_class_bases zero_class_virtual_bases
		})

		INSTRUCTION_NODE(zero_union_padding, "Initialize the object's padding bits to zero bits.", "[dcl.init.general]/6.3")
		-> INSTRUCTION_NODE(zero_union_first_nsdm, "Zero-nitialize the object's first non-static named data member.")
		-> NEW_DONE()

		ZERO_INIT_BACKREF(zero_union_first_nsdm)
	}

	// 6.4
	{
		YN_QUESTION_NODE(zero_is_array, "Is T an array type?", "[dcl.init.general]/6.4", zero_array_elements, zero_is_reference)

		INSTRUCTION_NODE(zero_array_elements, "Zero-initialize each element.", "[dcl.init.general]/6.4")
		-> NEW_DONE()

		ZERO_INIT_BACKREF(zero_array_elements)
	}

	// 6.5
	{
		YN_QUESTION_NODE(zero_is_reference, "Is T a reference type?", "[dcl.init.general]/6.5", zero_reference_no_init, NEW_DONE())
		INSTRUCTION_NODE(zero_reference_no_init, "No initialization is performed.", "[dcl.init.general]/6.5")
	}
}

#endif
