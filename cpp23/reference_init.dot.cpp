#ifndef REFERENCE_INIT_INCLUDED
#define REFERENCE_INIT_INCLUDED

#include "common.h"

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

			INSTRUCTION_NODE(reference_lvals_compatible_bind, "Bind the destination reference to the initializer expression lvalue (or appropriate base class subobject).", "[dcl.init.ref]/5.1")
			-> NEW_DONE()
		}

		// 5.1.2
		{
			YN_QUESTION_NODE(reference_dest_lval_is_source_class, "Is T2 a class type?", "[dcl.init.ref]/5.1.2", reference_dest_lval_source_class_is_reference_related, reference_is_dest_lval_for_step_2)
			YN_QUESTION_NODE(reference_dest_lval_source_class_is_reference_related, "Is T1 reference-related to T2?", "[dcl.init.ref]/5.1.2", reference_is_dest_lval_for_step_2, reference_dest_lval_source_class_is_convertible)
			YN_QUESTION_NODE(reference_dest_lval_source_class_is_convertible, "Is the initializer expression convertible to an lvalue of type \"cv3 T3\" such that cv1 T1 is reference-compatible with cv3 T3?", "[dcl.init.ref]/5.1.2", reference_class_enumerate_conversions, reference_is_dest_lval_for_step_2)

			INSTRUCTION_NODE(reference_class_enumerate_conversions, "Enumerate applicable conversion functions.", "[dcl.init.ref]/5.1.2")
			-> INSTRUCTION_NODE(reference_class_select_conversion, "Select the best applicable conversion function.", "[dcl.init.ref]/5.1.2")
			-> INSTRUCTION_NODE(reference_class_do_initialization, "Bind the destination reference to the lvalue result of the conversion (or appropriate base class subobject).", "[dcl.init.ref]/5.1")
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
			YN_QUESTION_NODE(reference_rval_conv_source_is_class, "Is T2 a class type?", "[dcl.init.ref]/5.3.2", reference_rval_conv_source_class_is_ref_related, reference_temp_is_dest_class)
			YN_QUESTION_NODE(reference_rval_conv_source_class_is_ref_related, "Is T1 reference-related to T2?", "[dcl.init.ref]/5.3.2", reference_temp_is_dest_class, reference_rval_conv_source_class_convertible_target)
			YN_QUESTION_NODE(reference_rval_conv_source_class_convertible_target, "Is the initializer convertible to an rvalue or function lvalue of type \"cv3 T3\", where \"cv1 T1\" is reference-compatible with \"cv3 T3\"?", "[dcl.init.ref]/5.3.2", reference_rval_conv_bind_converted, reference_temp_is_dest_class)

			INSTRUCTION_NODE(reference_rval_conv_bind_converted, "The converted initializer is the converted expression.", "[dcl.init.ref]/5.3")
			-> reference_rval_conv_is_converted_prval
		}

		YN_QUESTION_NODE(reference_rval_conv_is_converted_prval, "Is the converted initializer a prvalue?", "[dcl.init.ref]/5.3", reference_rval_conv_prval_t4_defn, reference_rval_conv_bind_glval)

		INSTRUCTION_NODE(reference_rval_conv_prval_t4_defn, "Let the prvalue have type \"T4\".", "[dcl.init.ref]/5.3")
		-> INSTRUCTION_NODE(reference_rval_conv_prval_adjust_type, "Adjust the prvalue's type to \"cv1 T4\".")
		-> INSTRUCTION_NODE(reference_rval_conv_prval_materialize, "Apply the temporary materialization conversion to the prvalue.", "[dcl.init.ref]/5.3")
		-> reference_rval_conv_bind_glval

		INSTRUCTION_NODE(reference_rval_conv_bind_glval, "Bind the destination reference to the resulting glvalue (or to an appropriate base class subobject).", "[dcl.init.ref]/5.3")
		-> NEW_DONE()
	}

	// 5.4
	{
		// 5.4.1
		{
			YN_QUESTION_NODE(reference_temp_is_dest_class, "Is T1 a class type?", "[dcl.init.ref]/5.4.1", reference_temp_is_related, reference_temp_is_source_class)
			YN_QUESTION_NODE(reference_temp_is_source_class, "Is T2 a class type?", "[dcl.init.ref]/5.4.1", reference_temp_is_related, reference_temp_implicit_conv)
			YN_QUESTION_NODE(reference_temp_is_related, "Is T1 reference-related to T2?", "[dcl.init.ref]/5.4.1", reference_temp_implicit_conv, reference_temp_user_defined_conv)

			INSTRUCTION_NODE(reference_temp_user_defined_conv, "Consider user-defined conversions for the copy-initialization of an object of type \"cv1 T1\" by user-defined conversion.", "[dcl.init.ref]/5.4.1")
			-> reference_temp_user_defined_conv_is_ill_formed

			YN_QUESTION_NODE(reference_temp_user_defined_conv_is_ill_formed, "Would the non-reference copy-initialization be ill-formed?", "[dcl.init.ref]/5.4.1", NEW_ILL_FORMED(), reference_temp_user_defined_conv_direct_initialize)

			INSTRUCTION_NODE(reference_temp_user_defined_conv_direct_initialize, "The result of the call to the conversion function, as described by non-reference copy-initialization, is used to direct-initialize the reference. For the direct-initialization, user-defined conversions are not considered.", "[dcl.init.ref]/5.4.1")
			-> NEW_RECURSE()
			-> reference_temp_implicit_conv_is_reference_related
		}

		// 5.4.2
		{
			INSTRUCTION_NODE(reference_temp_implicit_conv, "The initializer expression is implicitly converted to a prvalue of type \"T1\".", "[dcl.init.ref]/5.4.2")
 			-> INSTRUCTION_NODE(reference_temp_implicit_conv_materialize, "Apply the temporary materialization conversion to the prvalue, considering the type of the prvalue to be \"cv1 T1\".", "[dcl.init.ref]/5.4.2")
			-> INSTRUCTION_NODE(reference_temp_implicit_conv_materialize_bind, "Bind reference to the result.", "[dcl.init.ref]/5.4.2")
			-> reference_temp_implicit_conv_is_reference_related
 		}

		YN_QUESTION_NODE(reference_temp_implicit_conv_is_reference_related, "Is T1 reference-related to T2?", "[dcl.init.ref]/5.4", reference_temp_implicit_conv_is_cv_okay, NEW_DONE())
		YN_QUESTION_NODE(reference_temp_implicit_conv_is_cv_okay, "Is cv1 at least as qualified as cv2?", "[dcl.init.ref]/5.4.3", reference_temp_implicit_conv_is_dest_rval, NEW_ILL_FORMED())

		YN_QUESTION_NODE(reference_temp_implicit_conv_is_dest_rval, "Is the destination an rvalue reference?", "[dcl.init.ref]/5.4.4", reference_temp_implicit_conv_is_source_lval, NEW_DONE())
		YN_QUESTION_NODE(reference_temp_implicit_conv_is_source_lval, "Is the initializer an lvalue?", "[dcl.init.ref]/5.4.4", NEW_ILL_FORMED(), NEW_DONE())
	}
}

#endif
