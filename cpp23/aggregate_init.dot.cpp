#ifndef AGGREGATE_INIT_INCLUDED
#define AGGREGATE_INIT_INCLUDED

#include "common.h"

#define AGGREGATE_INIT_HEAD aggregate_initialization_head

// [dcl.init.aggr]
subgraph cluster_aggregate_initialization {
	// 2
	{
		INSTRUCTION_NODE(AGGREGATE_INIT_HEAD, "Aggregate initialization", "[dcl.init.aggr]")
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
			-> NEW_RECURSE_SUBOBJECT()
			-> aggregate_initialize_explicit_repeat_next
		}

		// 4.2
		{
			QUESTION_NODE(aggregate_initialize_explicit_copy_kind, "What is the kind of brace initializer?", "[dcl.init.aggr]/4.2")
				aggregate_initialize_explicit_copy_kind -> aggregate_initialize_explicit_copy_init_list [label="initializer-list"]
				aggregate_initialize_explicit_copy_kind -> aggregate_initialize_explicit_copy_designated [label="designated-initializer-list"]

			INSTRUCTION_NODE(aggregate_initialize_explicit_copy_init_list, "Copy-initialize the element from the corresponding initializer-clause.", "[dcl.init.aggr]/4.2")
			-> NEW_RECURSE_SUBOBJECT()
			-> aggregate_initialize_explicit_is_narrowing_checked_no_eq

			INSTRUCTION_NODE(aggregate_initialize_explicit_copy_designated, "Initialize the element with the brace-or-equal-initializer of the corresponding designated-initializer-clause.", "[dcl.init.aggr]/4.2")
			-> NEW_RECURSE_SUBOBJECT()
			-> aggregate_initialize_explicit_is_narrowing_checked_no_eq

			YN_QUESTION_NODE(aggregate_initialize_explicit_is_narrowing_checked_no_eq, "Is the initializer of the form \"assignment-expression\"?", "[dcl.init.aggr]/4.2", aggregate_initialize_explicit_is_narrowing, aggregate_initialize_explicit_is_narrowing_checked_eq)
			YN_QUESTION_NODE(aggregate_initialize_explicit_is_narrowing_checked_eq, "Is the initializer of the form \"= assignment-expression\"?", "[dcl.init.aggr]/4.2", aggregate_initialize_explicit_is_narrowing, aggregate_initialize_explicit_repeat_next)
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
			-> NEW_RECURSE_SUBOBJECT()
			-> aggregate_initialize_nonexplicit_nonunion_repeat_next
		}

		// 5.2
		{
			YN_QUESTION_NODE(aggregate_initialize_nonexplicit_nonunion_is_reference, "Is the element a reference?", "[dcl.init.aggr]/5.2", NEW_ILL_FORMED(), aggregate_initialize_nonexplicit_nonunion_copy_init)

			INSTRUCTION_NODE(aggregate_initialize_nonexplicit_nonunion_copy_init, "The element is copy-initialized from an empty initializer list.", "[dcl.init.aggr]/5.2")
			-> NEW_RECURSE_SUBOBJECT()
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
			-> NEW_RECURSE_SUBOBJECT()
			-> NEW_DONE()
		}

		// 5.4
		{
			INSTRUCTION_NODE(aggregate_initialize_nonexplicit_union_first_mem, "The first member of the union (if any) is copy-initialized from an empty initializer list.", "[dcl.init.aggr]/5.5")
			-> NEW_RECURSE_SUBOBJECT()
			-> NEW_DONE()
		}
	}
}

#endif
