define(`DEFINE_DONE', `$1 [label="Done", style=filled, fillcolor=green, shape=box, color=green, fontcolor=white]
')
define(`LINK_TO_DONE', `$1 -> $1'`__generated_done '`$2'`
DEFINE_DONE(`$1'`__generated_done')
')

define(`QUESTION_NODE', `$1 [label="$2'`ifelse($3,`', `', `\n'$3)'`", shape=diamond]')
define(`INSTRUCTION_NODE', `$1 [label="$2'`ifelse($3,`', `', `\n'$3)'`", shape=box]')

define(`YN_QUESTION_NODE', `QUESTION_NODE($1, $2, $3)
$1 -> $4 [label="Yes"]
$1 -> $5 [label="No"]
')

define(`YN_QUESTION_NODE_NO_CITE', `QUESTION_NODE($1, $2)
$1 -> $3 [label="Yes"]
$1 -> $4 [label="No"]
')

define(`ILL_FORMED_NODE', `$1 [label="The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]
')

define(`LINK_TO_ILL_FORMED', `$1 -> $1'`__generated_ill_formed $2'`
ILL_FORMED_NODE(`$1'`__generated_ill_formed')
')

digraph initialization {
    start [label="So you want to initialize something?\n[dcl.init]/16", style=filled, fillcolor=green, shape=box, color=green, fontcolor=white]
        start -> is_braced

    YN_QUESTION_NODE(is_braced, `Is the initializer in braces?', `[dcl.init]/16.1', list_initialization_head, is_dest_reference)
    
    YN_QUESTION_NODE(is_dest_reference, `Is the destination type a reference type?', `[dcl.init]/16.2', reference_initialization_head, is_char_arr_init)
    
    QUESTION_NODE(is_char_arr_init, `Is the destination type a char[] or a char*_t[]?', `[dcl.init]/16.3')
        is_char_arr_init -> is_char_arr_literal_init [label="Yes"]
    
    YN_QUESTION_NODE(is_char_arr_literal_init, `Is the initializer a string literal?', `[dcl.init]/16.3', string_literal_initialization_head, is_initializer_empty_parens)

    YN_QUESTION_NODE(is_initializer_empty_parens, `Is the initializer \"()\"?', `[dcl.init]/16.4', value_initialization_head, is_dest_array)

    YN_QUESTION_NODE(is_dest_array, `Is the destination type an array?', `[dcl.init]/16.5', array_initialization_head, is_dest_class_type)

    subgraph array_initialization {
        INSTRUCTION_NODE(array_initialization_head, `Initialization as follows:', `[dcl.init]/16.5')
            array_initialization_head -> array_k_definition
        
        INSTRUCTION_NODE(array_k_definition, `Let k be the number of elements in the initializer's expression list.')
            array_k_definition -> array_is_unsized

        YN_QUESTION_NODE_NO_CITE(array_is_unsized, `Is destination type an array of unknown bound?', array_unsized_n_defn, array_sized_n_defn)
        
        INSTRUCTION_NODE(array_unsized_n_defn, `Let n be k.')
            array_unsized_n_defn -> array_initialize_first_k

        INSTRUCTION_NODE(array_sized_n_defn, `Let n be the array size of the destination type.')
            array_sized_n_defn -> array_k_gt_n

        YN_QUESTION_NODE_NO_CITE(array_k_gt_n, `Is k > n?', array_k_gt_n_ill_formed, array_initialize_first_k)

        ILL_FORMED_NODE(array_k_gt_n_ill_formed)

        INSTRUCTION_NODE(array_initialize_first_k, `Copy-initialize the first k array elements from the expressions in the initailizer.')
            array_initialize_first_k -> array_initialize_rest

        INSTRUCTION_NODE(array_initialize_rest, `Value-initialize the remaining elements.')
            LINK_TO_DONE(array_initialize_rest)
    }

    YN_QUESTION_NODE(is_dest_class_type, `Is the destination type a class type?', `[dcl.init]/16.6', class_dest_initialization_head, is_source_class_type)

    YN_QUESTION_NODE(is_source_class_type, `Is the source type a class type?', `[dcl.init]/16.7', class_source_initialization_head, is_direct_init_for_nullptr)

    YN_QUESTION_NODE(is_direct_init_for_nullptr, `Is the initialization direct-initialization?', `[dcl.init]/16.8', is_source_type_nullptr, standard_conv_seq_initialization_head)    

    YN_QUESTION_NODE(is_source_type_nullptr, `Is the source type std::nullptr_t?', `[dcl.init]/16.8', is_dest_type_bool_for_nullptr, standard_conv_seq_initialization_head)

    YN_QUESTION_NODE(is_dest_type_bool_for_nullptr, `Is the destination type bool?', `[dcl.init]/16.8', nullptr_to_bool_init, standard_conv_seq_initialization_head)

    INSTRUCTION_NODE(nullptr_to_bool_init, `The bool is initialized to false.', `[dcl.init]/16.8')
        LINK_TO_DONE(nullptr_to_bool_init)

    subgraph class_dest_initialization {
        INSTRUCTION_NODE(class_dest_initialization_head, `Initialization as follows:', `[dcl.init]/16.6')
            class_dest_initialization_head -> class_is_initializer_prvalue

        YN_QUESTION_NODE(class_is_initializer_prvalue, `Is the initializer a prvalue?', `[dcl.init]/16.6.1', class_is_initializer_prvalue_same_class, class_is_copy_init)

        YN_QUESTION_NODE(class_is_initializer_prvalue_same_class, `Is the source type the same as the destination type (up to cv-qualification)?', `[dcl.init]/16.6.1', class_initialize_by_prvalue, class_is_copy_init)

        INSTRUCTION_NODE(class_initialize_by_prvalue, `Use the prvalue to initialize the destination object.', `[dcl.init]/16.6.1')
            LINK_TO_DONE(class_initialize_by_prvalue)

        YN_QUESTION_NODE(class_is_copy_init, `Is the initialization copy-initialization?', `[dcl.init]/16.6.2', class_is_copy_init_same_class, class_is_direct_init)
        
        INSTRUCTION_NODE(class_is_copy_init_same_class, `Is the source type the same class as the destination type (up to cv qualification)?', `[dcl.init]/16.6.2')
            class_is_copy_init_same_class -> class_consider_constructors [label="Yes"]
            class_is_copy_init_same_class -> class_is_copy_init_derived_class [label="No"]

        INSTRUCTION_NODE(class_is_copy_init_derived_class, `Is the source type a derived class of the destination type?', `[dcl.init]/16.6.2')
            class_is_copy_init_derived_class -> class_consider_constructors [label="Yes"]
            class_is_copy_init_derived_class -> class_user_defined_conv_head [label="No"]

        INSTRUCTION_NODE(class_is_direct_init, `The initialization is direct-initialization.', `[dcl.init]/16.6.2')
            class_is_direct_init -> class_consider_constructors

        INSTRUCTION_NODE(class_consider_constructors, `Enumerate constructors and select best through overload resolution.', `[dcl.init]/16.6.2')
            class_consider_constructors -> class_constructors_is_resolution_successful

        YN_QUESTION_NODE(class_constructors_is_resolution_successful, `Is overload resolution succesful?', `[dcl.init]/16.6.2', class_constructors_use_selected, class_is_aggregate)

        INSTRUCTION_NODE(class_constructors_use_selected, `Use the selected constructor to initialize the object, using the expression or expression-list as argument(s).', `[dcl.init]/16.6.2.1')
            LINK_TO_DONE(class_constructors_use_selected)

        YN_QUESTION_NODE(class_is_aggregate, `Is the destination type an aggregate class?', `[dcl.init]/16.6.2.2', class_aggregate_is_initializer_expr_list, class_ill_formed)

        YN_QUESTION_NODE(class_aggregate_is_initializer_expr_list, `Is the initializer a parenthesized expression-list?', `[dcl.init]/16.6.2.2', class_aggregate_paren_init_head, class_ill_formed)

        ILL_FORMED_NODE(class_ill_formed)

        subgraph class_aggregate_paren_init {
            INSTRUCTION_NODE(class_aggregate_paren_init_head, `Initialized as follows:', `[dcl.init]/16.6.2.2')
                class_aggregate_paren_init_head -> class_aggregate_paren_n_defn

            INSTRUCTION_NODE(class_aggregate_paren_n_defn, `Let n be the number of elements in the aggregate.')
                class_aggregate_paren_n_defn -> class_aggregate_paren_k_defn

            INSTRUCTION_NODE(class_aggregate_paren_k_defn, `Let k b ethe number of elements in the initializer's expression list.')
                class_aggregate_paren_k_defn -> class_aggregate_paren_is_k_gt_n

            YN_QUESTION_NODE_NO_CITE(class_aggregate_paren_is_k_gt_n, `Is k > n?', class_aggregate_paren_ill_formed, class_aggregate_paren_initialize_first_k)

            INSTRUCTION_NODE(class_aggregate_paren_initialize_first_k, `Copy-initialize the first k elements from the expression list.')
                class_aggregate_paren_initialize_first_k -> class_aggregate_paren_initialize_rest

            INSTRUCTION_NODE(class_aggregate_paren_initialize_rest, `Use default member initializer or value-initialize the remaining elements.')
                LINK_TO_DONE(class_aggregate_paren_initialize_rest)

            ILL_FORMED_NODE(class_aggregate_paren_ill_formed)
        }

        subgraph class_user_defined_conv {
            INSTRUCTION_NODE(class_user_defined_conv_head, `Initialization as follows:', `[dcl.init]/16.6.3')
                class_user_defined_conv_head -> class_user_defined_conv_overload_resolution

            INSTRUCTION_NODE(class_user_defined_conv_overload_resolution, `Use overload resolution to select the best user-defined conversion that can convert from the source type to the destination type or (when a conversion function is used) to a derived class thereof.')
                class_user_defined_conv_overload_resolution -> class_user_defined_conv_is_possible
            
            YN_QUESTION_NODE_NO_CITE(class_user_defined_conv_is_possible, `Is the conversion ambiguous or impossible?', class_user_defined_conv_ill_formed, class_user_defined_conv_do_conversion)

            INSTRUCTION_NODE(class_user_defined_conv_do_conversion, `Call the selected function with the initializer-expression as its argument.')
                class_user_defined_conv_do_conversion -> class_user_defined_conv_initialize

            INSTRUCTION_NODE(class_user_defined_conv_initialize, `Direct-initialize the destination object with the result of the conversion.')
                LINK_TO_DONE(class_user_defined_conv_initialize)

            ILL_FORMED_NODE(class_user_defined_conv_ill_formed)
        }
    }

    subgraph string_literal_initialization {
        INSTRUCTION_NODE(string_literal_initialization_head, `Initialization as follows:', `[dcl.init.string]')
            string_literal_initialization_head -> string_literal_verify_kind

        INSTRUCTION_NODE(string_literal_verify_kind, `Verify array type and literal type match.')
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

        string_literal_kind_other -> string_literal_wrong_kind
        string_literal_wrong_kind  [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        { string_literal_kind_char, string_literal_kind_char8, string_literal_kind_char16, string_literal_kind_char32, string_literal_kind_wchar } -> string_literal_initialize_first

        INSTRUCTION_NODE(string_literal_initialize_first, `Initialize the first elements of the array with successive values from the string literal.')
            string_literal_initialize_first -> string_literal_has_too_many

        YN_QUESTION_NODE_NO_CITE(string_literal_has_too_many, `Are there more initializers than array elements?', string_literal_ill_formed_too_many, string_literal_initialize_rest)

        ILL_FORMED_NODE(string_literal_ill_formed_too_many)

        string_literal_initialize_rest [label="Zero-initialize the remaining elements of the array (if any)."]
            LINK_TO_DONE(string_literal_initialize_rest)
    }

    subgraph class_source_initialization {
        INSTRUCTION_NODE(class_source_initialization_head, `Initialized as follows:', `[dcl.init]/16.7')
            class_source_initialization_head -> class_source_consider_conversion_functions

        INSTRUCTION_NODE(class_source_consider_conversion_functions, `Use overload resolution to select the best applicable conversion function.')
            class_source_consider_conversion_functions -> class_source_conversion_is_impossible

        YN_QUESTION_NODE_NO_CITE(class_source_conversion_is_impossible, `Is the conversion impossible or ambiguous?', class_source_conversion_ill_formed, class_source_initialize)

        ILL_FORMED_NODE(class_source_conversion_ill_formed)

        class_source_initialize [label="Use the result of the conversion to convert the initializer to the object being initialized."]
            LINK_TO_DONE(class_source_initialize)
    }

    subgraph standard_conv_seq_initialization {
        INSTRUCTION_NODE(standard_conv_seq_initialization_head, `The object is initialized as follows:', `[dcl.init]/6.9')
            standard_conv_seq_initialization_head -> standard_conv_seq_do_init

        INSTRUCTION_NODE(standard_conv_seq_do_init, `Initialize the object using the value of the initializer expression, using a standard conversion sequence if necessary, not considering any user-defined conversions.')
            standard_conv_seq_do_init -> standard_conv_seq_is_possible

        YN_QUESTION_NODE_NO_CITE(standard_conv_seq_is_possible, `Is the conversion possible?', standard_conv_seq_is_bitfield, standard_conv_seq_ill_formed)

        ILL_FORMED_NODE(standard_conv_seq_ill_formed)

        QUESTION_NODE(standard_conv_seq_is_bitfield, `Is the object to be initialized a bit-field?')
            standard_conv_seq_is_bitfield -> standard_conv_seq_is_bitfield_in_range [label="Yes"]
            LINK_TO_DONE(standard_conv_seq_is_bitfield, [label="No"])

        QUESTION_NODE(standard_conv_seq_is_bitfield_in_range, `Is the value representable by the bit-field?')
            standard_conv_seq_is_bitfield_in_range -> standard_conv_seq_bitfield_imp_def [label="No"]
            LINK_TO_DONE(standard_conv_seq_is_bitfield_in_range, [label="Yes"])

        INSTRUCTION_NODE(standard_conv_seq_bitfield_imp_def, `The value of the bit-field is implementation-defined.')
            LINK_TO_DONE(standard_conv_seq_bitfield_imp_def)
    }

    subgraph reference_initialization {
        INSTRUCTION_NODE(reference_initialization_head, `Reference initialization', `[dcl.init.ref]')
            reference_initialization_head -> reference_dest_type_defn
        
        INSTRUCTION_NODE(reference_dest_type_defn, `Let the destination type be \"reference to cv1 T1\".', `[dcl.init.ref]/5')
            reference_dest_type_defn -> reference_source_type_defn

        INSTRUCTION_NODE(reference_source_type_defn, `Let the source type be \"cv2 T2\".', `[dcl.init.ref]/5')
            reference_source_type_defn -> reference_is_dest_lval

        YN_QUESTION_NODE(reference_is_dest_lval, `Is the destination type an lvalue reference?', `[dcl.init.ref]/5.1', reference_dest_lval_is_source_lval, reference_dest_is_lval_non_const)

        YN_QUESTION_NODE(reference_dest_lval_is_source_lval, `Is the initializer an lvalue?', `[dcl.init.ref]/5.1', reference_lvals_is_compatible, reference_dest_lval_is_source_class)

        YN_QUESTION_NODE(reference_lvals_is_compatible, `Is cv1 T1 reference-compatibile with cv2 T2?', `[dcl.init.ref]/5.1', reference_lvals_compatible_bind, reference_dest_lval_is_source_class)

        INSTRUCTION_NODE(reference_lvals_compatible_bind, `The destination reference is bound to the initializer lvalue (or appropriate base).', `[dcl.init.ref]/5.1')
            LINK_TO_DONE(reference_lvals_compatible_bind)

        YN_QUESTION_NODE(reference_dest_lval_is_source_class, `Is T2 a class type?', `[dcl.init.ref]/5.1.2', reference_dest_lval_source_class_is_reference_related, reference_dest_is_lval_non_const)

        YN_QUESTION_NODE(reference_dest_lval_source_class_is_reference_related, `Is T1 reference-related to T2?', `[dcl.init.ref]/5.1.2', reference_dest_is_lval_non_const, reference_dest_lval_source_class_is_convertible)

        YN_QUESTION_NODE(reference_dest_lval_source_class_is_convertible, `Is T2 convertible to an lvalue of type cv3 T3 such that cv1 T1 is reference-compatible with cv3 T3?', `[dcl.init.ref]/5.1.2', reference_class_select_conversion, reference_dest_is_lval_non_const)

        INSTRUCTION_NODE(reference_class_select_conversion, `Select the best applicable conversion function.', `[dcl.init.ref]/5.1.2')
            reference_class_select_conversion -> reference_class_do_initialization

        INSTRUCTION_NODE(reference_class_do_initialization, `The destination reference is bound to the result of the conversion (or appropriate base).', `[dcl.init.ref]/5.1')
            LINK_TO_DONE(reference_class_do_initialization)

        YN_QUESTION_NODE(reference_dest_is_lval_non_const, `Is the destination an lvalue reference to a non-const type?', `[dcl.init.ref]/5.2', reference_dest_non_const_ill_formed, reference_dest_is_volatile)

        ILL_FORMED_NODE(reference_dest_non_const_ill_formed)

        YN_QUESTION_NODE(reference_dest_is_volatile, `Is the destination's referenced type volatile-qualified', `[dcl.init.ref]/5.2', reference_dest_volatile_ill_formed, reference_rval_conv_source_is_rvalue)

        ILL_FORMED_NODE(reference_dest_volatile_ill_formed)

        YN_QUESTION_NODE(reference_rval_conv_source_is_rvalue, `Is the initializer an rvalue?', `[dcl.init.ref]/5.3.1', reference_rval_conv_source_is_rvalue_bitfield, reference_rval_conv_source_is_function_lval)

        YN_QUESTION_NODE(reference_rval_conv_source_is_rvalue_bitfield, `Is the initializer a bit-field?', `[dcl.init.ref]/5.3.1', reference_rval_conv_source_is_function_lval, reference_rval_conv_source_rval_or_function_is_ref_compat)

        YN_QUESTION_NODE(reference_rval_conv_source_is_function_lval, `Is the initializer a function lvalue?', `[dcl.init.ref]/5.3.1', reference_rval_conv_source_rval_or_function_is_ref_compat, reference_rval_conv_source_is_class)

        YN_QUESTION_NODE(reference_rval_conv_source_rval_or_function_is_ref_compat, `Is cv1 T1 reference-compatible with cv2 T2?', `[dcl.init.ref]/5.3.1', reference_rval_conv_bind_direct, reference_rval_conv_source_is_class)

        YN_QUESTION_NODE(reference_rval_conv_source_is_class, `Is T2 a class type?', `[dcl.init.ref]/5.3.2', reference_rval_conv_source_class_is_ref_related, reference_temp_is_dest_class)

        YN_QUESTION_NODE(reference_rval_conv_source_class_is_ref_related, `Is T1 reference-related to T2?', `[dcl.init.ref]/5.3.2', reference_temp_is_dest_class, reference_rval_conv_source_class_convertible_target)

        YN_QUESTION_NODE(reference_rval_conv_source_class_convertible_target, `Is the initializer convertible to an rvalue or function lvalue of type \"cv3 T3\", where \"cv1 T1\" is reference-compatible with \"cv3 T3\"?', `[dcl.init.ref]/5.3.2', reference_rval_conv_bind_converted, reference_temp_is_dest_class)

        INSTRUCTION_NODE(reference_rval_conv_bind_direct, `The converted initializer is the value of the initializer.', `[dcl.init.ref]/5.3')
            reference_rval_conv_bind_direct -> reference_rval_conv_is_converted_prval

        INSTRUCTION_NODE(reference_rval_conv_bind_converted, `The converted initializer is the result of the conversion.', `[dcl.init.ref]/5.3')
            reference_rval_conv_bind_converted -> reference_rval_conv_is_converted_prval

        YN_QUESTION_NODE(reference_rval_conv_is_converted_prval, `Is the converted initializer a prvalue?', `[dcl.init.ref]/5.3', reference_rval_conv_prval_adjust_type, reference_rval_conv_bind_glval)

        INSTRUCTION_NODE(reference_rval_conv_prval_adjust_type, `Its type T4 is adjusted to \"cv1 T4\".', `[dcl.init.ref]/5.3')
            reference_rval_conv_prval_adjust_type -> reference_rval_conv_prval_materialize

        INSTRUCTION_NODE(reference_rval_conv_prval_materialize, `The prvalue is materialized.', `[dcl.init.ref]/5.3')
            reference_rval_conv_prval_materialize -> reference_rval_conv_bind_glval

        INSTRUCTION_NODE(reference_rval_conv_bind_glval, `The destination reference is bound to the resulting glvalue.', `[dcl.init.ref]/5.3')
            LINK_TO_DONE(reference_rval_conv_bind_glval)

        YN_QUESTION_NODE(reference_temp_is_dest_class, `Is T1 a class type?', `[dcl.init.ref]/5.4.1', reference_temp_is_related, reference_temp_is_source_class)

        YN_QUESTION_NODE(reference_temp_is_source_class, `Is T2 a class type?', `[dcl.init.ref]/5.4.1', reference_temp_is_related, reference_temp_implicit_conv)

        YN_QUESTION_NODE(reference_temp_is_related, `Is T1 reference-related to T2?', `[dcl.init.ref]/5.4.1', reference_temp_implicit_conv, reference_temp_user_defined_conv)

        INSTRUCTION_NODE(reference_temp_user_defined_conv, `Consider user-defined conversions for the copy-initialization of an object of type \"cv1 T1\" by user-defined-conversion.', `[dcl.init.ref]/5.4.1')
            reference_temp_user_defined_conv -> reference_temp_user_defined_conv_is_ill_formed

        YN_QUESTION_NODE(reference_temp_user_defined_conv_is_ill_formed, `Would the non-reference copy-initialization be ill-formed?', `[dcl.init.ref]/5.4.1', reference_temp_user_defined_conv_ill_formed, reference_temp_user_defined_conv_direct_initialize)

        ILL_FORMED_NODE(reference_temp_user_defined_conv_ill_formed)

        INSTRUCTION_NODE(reference_temp_user_defined_conv_direct_initialize, `The result of the call to the conversion function, as described by non-reference copy-initialization, is used to direct-initialize the reference. For the direct-initialization, user-defined conversions are not considered.', `[dcl.init.ref]/5.4.1')
            LINK_TO_DONE(reference_temp_user_defined_conv_direct_initialize)

        INSTRUCTION_NODE(reference_temp_implicit_conv, `The initializer expression is implicitly converted to a prvalue of type \"cv1 T1\".', `[dcl.init.ref]/5.4.2')
            reference_temp_implicit_conv -> reference_temp_implicit_conv_materialize

        INSTRUCTION_NODE(reference_temp_implicit_conv_materialize, `The temporary is materialized.', `[dcl.init.ref]/5.4.2')
            reference_temp_implicit_conv_materialize -> reference_temp_implicit_conv_materialize_bind

        INSTRUCTION_NODE(reference_temp_implicit_conv_materialize_bind, `The reference is bound to the result.', `[dcl.init.ref]/5.4.2')
            reference_temp_implicit_conv_materialize_bind -> reference_temp_implicit_conv_materialize_is_reference_related

        QUESTION_NODE(reference_temp_implicit_conv_materialize_is_reference_related, `Is T1 reference-related to T2?', `[dcl.init.ref]/5.4')
            reference_temp_implicit_conv_materialize_is_reference_related -> reference_temp_implicit_conv_materialize_is_cv_okay [label="Yes"]
            LINK_TO_DONE(reference_temp_implicit_conv_materialize_is_reference_related, [label="No"])

        YN_QUESTION_NODE(reference_temp_implicit_conv_materialize_is_cv_okay, `Is cv1 more qualified than cv2?', `[dcl.init.ref]/5.4.3', reference_temp_implicit_conv_materialize_is_dest_rval, reference_temp_implicit_conv_materialize_cv_ill_formed)

        ILL_FORMED_NODE(reference_temp_implicit_conv_materialize_cv_ill_formed)

        QUESTION_NODE(reference_temp_implicit_conv_materialize_is_dest_rval, `Is the destination an rvalue reference?', `[dcl.init.ref]/5.4.3')
            reference_temp_implicit_conv_materialize_is_dest_rval -> reference_temp_implicit_conv_materialize_is_source_lval [label="Yes"]
            LINK_TO_DONE(reference_temp_implicit_conv_materialize_is_dest_rval, [label="No"])

        QUESTION_NODE(reference_temp_implicit_conv_materialize_is_source_lval, `Is the initializer an lvalue?', `[dcl.init.ref]/5.4.4')
            LINK_TO_ILL_FORMED(reference_temp_implicit_conv_materialize_is_source_lval, [label="Yes"])
            LINK_TO_DONE(reference_temp_implicit_conv_materialize_is_source_lval, [label="No"])
    }

    subgraph value_initialization {
        INSTRUCTION_NODE(value_initialization_head, `Value-initialization', `[dcl.init]/8')
            value_initialization_head -> value_is_class

        YN_QUESTION_NODE(value_is_class, `Is the type a class type?', `[dcl.init]/8.1', value_has_dflt_ctor, value_is_array)

        YN_QUESTION_NODE(value_has_dflt_ctor, `Does the type have a default constructor?', `[dcl.init]/8.1.1', value_has_deleted_dflt_ctor, value_default_initialize)

        YN_QUESTION_NODE(value_has_deleted_dflt_ctor, `Does the type have a deleted default constructor?', `[dcl.init]/8.1.1', value_default_initialize, value_has_user_dflt_ctor)

        YN_QUESTION_NODE(value_has_user_dflt_ctor, `Does the type have a user-provided default constructor?', `[dcl.init]/8.1.1', value_default_initialize, value_zero_initialize_class)

        INSTRUCTION_NODE(value_zero_initialize_class, `The object is zero-initialized.')
            value_zero_initialize_class -> value_check_default

        YN_QUESTION_NODE(value_is_array, `Is the type an array type?', `[dcl.init]/8.2', value_value_initialize_elements, value_zero_initialize_fallback)

        INSTRUCTION_NODE(value_value_initialize_elements, `Each element is value-initialized.')
            LINK_TO_DONE(value_value_initialize_elements)

        INSTRUCTION_NODE(value_zero_initialize_fallback, `The object is zero-initialized.')
            LINK_TO_DONE(value_zero_initialize_fallback)

        INSTRUCTION_NODE(value_default_initialize, `The object is default-initialized.', `[dcl.init]/8.1.*')
            LINK_TO_DONE(value_default_initialize)

        INSTRUCTION_NODE(value_check_default, `The semantic constraints for default-initialization are checked.', `[dcl.init]/8.1.2')
            value_check_default -> value_has_nontrivial_dflt_ctor

        QUESTION_NODE(value_has_nontrivial_dflt_ctor, `Does the type have a non-trivial default constructor?', `[dcl.init]/8.1.2')
            value_has_nontrivial_dflt_ctor -> value_default_initialize [label="Yes"]
            LINK_TO_DONE(value_has_nontrivial_dflt_ctor, [label="No"])
    }

    subgraph list_initialization {
        INSTRUCTION_NODE(list_initialization_head, `List-initialization', `[dcl.init.list]/3]')
            list_initialization_head -> list_has_designated_initializer

        YN_QUESTION_NODE(list_has_designated_initializer, `Does the braced-init-list contain a designated-initializer-list?', `[dcl.init.list]/3.1', list_designated_initalizer_is_aggregate, list_is_aggregate_class)

        YN_QUESTION_NODE(list_designated_initalizer_is_aggregate, `Is the type an aggregate class?', `[dcl.init.list]/3.1', list_designated_initializer_are_identifiers_valid, list_designated_initalizer_nonaggregate_ill_formed)

        ILL_FORMED_NODE(list_designated_initalizer_nonaggregate_ill_formed)

        YN_QUESTION_NODE(list_designated_initializer_are_identifiers_valid, `Do the designators form a subsequence of the ordered idenitifiers in the direct non-static data members of the type?', `[dcl.init.list]/3.1', list_designated_initializer_aggregate_init, list_designated_initalizer_initializers_ill_formed)

        ILL_FORMED_NODE(list_designated_initalizer_initializers_ill_formed)

        QUESTION_NODE(list_designated_initializer_aggregate_init, `Aggregate initialization is performed.', `[dcl.init.list]/3.1')
        list_designated_initializer_aggregate_init -> aggregate_initialization_head

        YN_QUESTION_NODE(list_is_aggregate_class, `Is the type an aggregate class?', `[dcl.init.list]/3.2', list_aggregate_is_list_singleton, list_is_type_char_array)

        YN_QUESTION_NODE(list_aggregate_is_list_singleton, `Does the initializer list have a single element?', `[dcl.init.list]/3.2', list_aggregate_singleton_is_type_valid, list_is_type_char_array)

        YN_QUESTION_NODE(list_aggregate_singleton_is_type_valid, `Does the sole element have type \"cv U\", where U is the initialized type or a type derived of it?', `[dcl.init.list]/3.2', list_aggregate_singleton_type_init_type, list_is_type_char_array)

        QUESTION_NODE(list_aggregate_singleton_type_init_type, `What is the type of initialization?', `[dcl.init.list]/3.2')
            list_aggregate_singleton_type_init_type -> list_aggregate_singleton_type_copy [label="copy-list-initialization"]
            list_aggregate_singleton_type_init_type -> list_aggregate_singleton_type_direct [label="direct-list-initialization"]

        list_aggregate_singleton_type_copy [label="The object is copy-initialized from the sole element.\n[dcl.init.list]/3.2"]
            LINK_TO_DONE(list_aggregate_singleton_type_copy)

        list_aggregate_singleton_type_direct [label="The object is direct-initialized from the sole element.\n[dcl.init.list]/3.2"]
            LINK_TO_DONE(list_aggregate_singleton_type_direct)

        YN_QUESTION_NODE(list_is_type_char_array, `Is the type a character array?', `[dcl.init.list]/3.3', list_char_array_is_singleton, list_is_aggregate)

        YN_QUESTION_NODE(list_char_array_is_singleton, `Does the iniitializer list have a single element?', `[dcl.init.list/]3.3', list_char_array_singleton_is_typed, list_is_aggregate)

        list_char_array_singleton_is_typed [label="Is that element an appropriately-typed string-literal?\n[dcl.init.list]/3.3"]
            list_char_array_singleton_is_typed -> list_char_array_string_literal_init [label=""]
            list_char_array_singleton_is_typed -> list_is_aggregate [label="No"]

        list_char_array_string_literal_init [label="Initialization as in [dcl.init.string]\n[dcl.init.list]/3.3"]
            list_char_array_string_literal_init -> string_literal_initialization_head

        YN_QUESTION_NODE(list_is_aggregate, `Is the type an aggregate?', `[dcl.init.list]/3.4', list_aggregate_aggregate_initialization, list_is_list_empty)

        INSTRUCTION_NODE(list_aggregate_aggregate_initialization, `Aggregate initialization is performed.', `[dcl.init.list]/3.4')
            list_aggregate_aggregate_initialization -> aggregate_initialization_head

        YN_QUESTION_NODE(list_is_list_empty, `Is the initializer list empty?', `[dcl.init.list]/3.5', list_empty_is_class, list_dest_is_initializer_list)

        YN_QUESTION_NODE(list_empty_is_class, `Is the destination type a class type?', `[dcl.init.list]/3.5', list_empty_has_default_constructor, list_dest_is_initializer_list)

        YN_QUESTION_NODE(list_empty_has_default_constructor, `Does the class have a default constructor?', `[dcl.init.list]/3.5', list_empty_value_initialize, list_dest_is_initializer_list)

        INSTRUCTION_NODE(list_empty_value_initialize, `The object is value-initialized.', `[dcl.init.list]/3.5')
            LINK_TO_DONE(list_empty_value_initialize)

        list_dest_is_initializer_list [label="Is the type a specialization of std::initializer_list?\n[dcl.init.list]/3.6"]
            list_dest_is_initializer_list -> list_initializer_list_init [label="Yes"]
            list_dest_is_initializer_list -> list_is_class [label="No"]

        INSTRUCTION_NODE(list_initializer_list_init, `Initialized as follows:', `[dcl.init.list]/5')
            list_initializer_list_init -> list_initializer_list_n_defn

        INSTRUCTION_NODE(list_initializer_list_n_defn, `Let N be the number of elements in the initalizer list.')
            list_initializer_list_n_defn -> list_initializer_list_materialize_array

        INSTRUCTION_NODE(list_initializer_list_materialize_array, `Where type is std::initializer_list<E>, a prvalue of type \"array of N const E\" is materialized.')
            list_initializer_list_materialize_array -> list_initializer_list_init_array

        INSTRUCTION_NODE(list_initializer_list_init_array, `Each element of the array is copy-initialized with the corresponding element of the initializer list.')
            list_initializer_list_init_array -> list_initializer_list_is_narrowing

        YN_QUESTION_NODE_NO_CITE(list_initializer_list_is_narrowing, `Is a narrowing conversion required to initialize any of the elements?', list_initializer_list_narrowing_ill_formed, list_initializer_list_init_object)

        ILL_FORMED_NODE(list_initializer_list_narrowing_ill_formed)

        INSTRUCTION_NODE(list_initializer_list_init_object, `The initializer_list is constructed to refer to the materialized array.')
            LINK_TO_DONE(list_initializer_list_init_object)

        YN_QUESTION_NODE(list_is_class, `Is the type a class type?', `[dcl.init.list]/3.7', list_class_ctors, list_is_enum)

        INSTRUCTION_NODE(list_class_ctors, `Constructors are considered, and the best match is selected through overload resolution.', `[dcl.init.list]/3.7')
            list_class_ctors -> list_class_is_narrowing

        QUESTION_NODE(list_class_is_narrowing, `Is a narrowing conversion required to convert any of the arguments?', `[dcl.init.list]/3.7')
            LINK_TO_ILL_FORMED(list_class_is_narrowing, [label="Yes"])
            LINK_TO_DONE(list_class_is_narrowing, [label="No"])

        YN_QUESTION_NODE(list_is_enum, `Is the type an enumeration?', `[dcl.init.list]/3.8', list_enum_is_fixed, list_final_is_singleton)

        YN_QUESTION_NODE(list_enum_is_fixed, `Does the enumeration have fixed underlying type?', `[dcl.init.list]/3.8', list_enum_underlying_defn, list_final_is_singleton)

        INSTRUCTION_NODE(list_enum_underlying_defn, `Let U be the underlying type.', `[dcl.init.list]/3.8')
            list_enum_underlying_defn -> list_enum_is_singleton

        YN_QUESTION_NODE(list_enum_is_singleton, `Does the initializer list have a single element?', `[dcl.init.list]/3.8', list_enum_elem_defn, list_final_is_singleton)

        INSTRUCTION_NODE(list_enum_elem_defn, `Let v be that element.', `[dcl.init.list]/3.8')
            list_enum_elem_defn -> list_enum_is_convertible

        YN_QUESTION_NODE(list_enum_is_convertible, `Can v be implicitly converted to U?', `[dcl.init.list]/3.8', list_enum_is_direct, list_final_is_singleton)

        YN_QUESTION_NODE(list_enum_is_direct, `Is the initialization direct-list-initialization?', `[dcl.init.list]/3.8', list_enum_is_narrowing, list_final_is_singleton)

        YN_QUESTION_NODE(list_enum_is_narrowing, `Is a narrowing conversion required to convert v to U?', `[dcl.init.list]/3.8', list_enum_narrowing_ill_formed, list_enum_initialization)

        INSTRUCTION_NODE(list_enum_initialization, `The object is initialized with the value T(u).')
            LINK_TO_DONE(list_enum_initialization)

        ILL_FORMED_NODE(list_enum_narrowing_ill_formed)

        // Final just because I couldn't come up with a better name for it. "Final" as in "last".

        YN_QUESTION_NODE(list_final_is_singleton, `Does the initializer list have a single element?', `[dcl.init.list]/3.9', list_final_singleton_type_defn, list_ref_prvalue_is_ref)

        INSTRUCTION_NODE(list_final_singleton_type_defn, `Let E be the type of that element.', `[dcl.init.list]/3.9')
            list_final_singleton_type_defn -> list_final_singleton_is_dest_ref

        YN_QUESTION_NODE(list_final_singleton_is_dest_ref, `Is the destination type a reference?', `[dcl.init.list]/3.9', list_final_singleton_is_dest_ref_related, list_final_singleton_type)

        YN_QUESTION_NODE(list_final_singleton_is_dest_ref_related, `Is the destination type's referenced type reference-related to E?', `[dcl.init.list]/3.9', list_final_singleton_type, list_ref_prvalue_is_ref)

        list_final_singleton_type [label="What is the type of initialization?\n[dcl.init.list]/3.9"]
            list_final_singleton_type -> list_final_singleton_direct [label="direct-list-initialization"]
            list_final_singleton_type -> list_final_singleton_copy [label="copy-list-initialization"]

        INSTRUCTION_NODE(list_final_singleton_direct, `The destination is initialized by direct-initialization from the element.', `[dcl.init.list]/3.9')
            list_final_singleton_direct -> list_final_singleton_is_narrowing

        INSTRUCTION_NODE(list_final_singleton_copy, `The destination is initialized by copy-initialization from the element.', `[dcl.init.list]/3.9')
            list_final_singleton_copy -> list_final_singleton_is_narrowing

        QUESTION_NODE(list_final_singleton_is_narrowing, `Is a narrowing conversion required to convert the element to the destination type?', `[dcl.init.list]/3.9')
            LINK_TO_ILL_FORMED(list_final_singleton_is_narrowing, [label="Yes"])
            LINK_TO_DONE(list_final_singleton_is_narrowing, [label="No"])

        YN_QUESTION_NODE(list_ref_prvalue_is_ref, `Is the destination type a reference type?', `[dcl.init.list]/3.10', list_ref_prvalue_prvalue_generated, list_final_is_empty)

        INSTRUCTION_NODE(list_ref_prvalue_prvalue_generated, `A prvalue is generated.', `[dcl.init.list]/3.10')
            list_ref_prvalue_prvalue_generated -> list_ref_prvalue_type_is_unknown_bound

        YN_QUESTION_NODE(list_ref_prvalue_type_is_unknown_bound, `Is the destination type an array of unknown bound?', `[dcl.init.list]/3.10', list_ref_prvalue_type_unknown_bound, list_ref_prvalue_type_normal)

        INSTRUCTION_NODE(list_ref_prvalue_type_normal, `The type of the prvalue is the type referenced by the destination type.', `[dcl.init.list]/3.10')
            list_ref_prvalue_type_normal -> list_ref_prvalue_init_prvalue

        INSTRUCTION_NODE(list_ref_prvalue_type_unknown_bound, `The type of the prvalue is the type of x in U x[] H, where H is the initializer list.', `[dcl.init.list]/3.10')
            list_ref_prvalue_type_unknown_bound -> list_ref_prvalue_init_prvalue

        INSTRUCTION_NODE(list_ref_prvalue_init_prvalue, `The prvalue initializes a result object by copy-list-initialization.', `[dcl.init.list]/3.10')
            list_ref_prvalue_init_prvalue -> list_ref_prvalue_init_ref

        INSTRUCTION_NODE(list_ref_prvalue_init_ref, `The reference is direct-initialized by the prvalue.', `[dcl.init.list]/3.10')
            LINK_TO_DONE(list_ref_prvalue_init_ref)

        // Final, again, as in "last".
        YN_QUESTION_NODE(list_final_is_empty, `Is the initializer list empty?', `[dcl.init.list]/3.11', list_final_empty_value_init, list_nothing_else_ill_formed)

        INSTRUCTION_NODE(list_final_empty_value_init, `The object is value-initialized.', `[dcl.init.list]/3.12')
            LINK_TO_DONE(list_final_empty_value_init)

        ILL_FORMED_NODE(list_nothing_else_ill_formed)

    }
}
