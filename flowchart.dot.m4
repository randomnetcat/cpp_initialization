define(`DEFINE_DONE', `$1 [label="Done", style=filled, fillcolor=green, shape=box, color=green, fontcolor=white]
')
define(`LINK_TO_DONE', `$1 -> $1'`__generated_done '`$2'`
DEFINE_DONE(`$1'`__generated_done')
')

digraph initialization {
    start [label="So you want to initialize something?\n[dcl.init]/16", style=filled, fillcolor=green, shape=box, color=green, fontcolor=white]
        start -> is_braced

    is_braced [label="Is the initializer in braces?\n[dcl.init]/16.1", shape=diamond]
        is_braced -> list_initialization_head [label="Yes"]
        is_braced -> is_dest_reference [label="No"]
    
    is_dest_reference [label="Is the destination type a reference type?\n[dcl.init]/16.2", shape=diamond]
        is_dest_reference -> reference_initialization_head [label="Yes"]
        is_dest_reference -> is_char_arr_init [label="No"]
    
    is_char_arr_init [label="Is the destination type a char[] or a char*_t[]?\n[dcl.init]/16.3", shape=diamond]
        is_char_arr_init -> is_char_arr_literal_init [label="Yes"]
    
    is_char_arr_literal_init [label="Is the initializer a string literal?\n[dcl.init]/16.3", shape=diamond]
        is_char_arr_literal_init -> string_literal_initialization_head [label="Yes"]
        is_char_arr_literal_init -> is_initializer_empty_parens [label="No"]

    is_initializer_empty_parens [label="Is the initializer \"()\"?\n[dcl.init]/16.4", shape=diamond]
        is_initializer_empty_parens -> value_initialization_head [label="Yes"]
        is_initializer_empty_parens -> is_dest_array [label="No"]

    is_dest_array[label="Is the destination type an array?\n[dcl.init]/16.5", shape=diamond]
        is_dest_array -> array_initialization_head [label="Yes"]
        is_dest_array -> is_dest_class_type [label="No"]

    subgraph array_initialization {
        array_initialization_head [label="Initialization as follows:\n[dcl.init]/16.5", shape=box]
            array_initialization_head -> array_k_definition
        
        array_k_definition [label="Let k be the number of elements in the initializer's expression list.", shape=box]
            array_k_definition -> array_is_unsized

        array_is_unsized [label = "Is destination type an array of unknown bound?", shape=diamond]
            array_is_unsized -> array_unsized_n_defn [label = "Yes"]
            array_is_unsized -> array_sized_n_defn [label = "No"]
        
        array_unsized_n_defn [label = "Let n be k.", shape=box]
            array_unsized_n_defn -> array_initialize_first_k

        array_sized_n_defn [label = "Let n be the array size of the destination type.", shape=box]
            array_sized_n_defn -> array_k_gt_n

        array_k_gt_n [label = "Is k > n?", shape=diamond]
            array_k_gt_n -> array_k_gt_n_ill_formed [label = "Yes"]
            array_k_gt_n -> array_initialize_first_k [label = "No"]    

        array_k_gt_n_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        array_initialize_first_k [label = "Copy-initialize the first k array elements from the expressions in the initailizer.", shape=box]
            array_initialize_first_k -> array_initialize_rest

        array_initialize_rest [label = "Value-initialize the remaining elements.", shape=box]
            array_initialize_rest -> done
    }

    is_dest_class_type [label="Is the destination type a class type?\n[dcl.init]/16.6", shape=diamond]
        is_dest_class_type -> class_dest_initialization_head [label="Yes"]
        is_dest_class_type -> is_source_class_type [label="No"]

    is_source_class_type [label="Is the source type a class type?\n[dcl.init]/16.7", shape=diamond]
        is_source_class_type -> class_source_initialization_head [label="Yes"]
        is_source_class_type -> is_direct_init_for_nullptr [label="No"]

    is_direct_init_for_nullptr [label="Is the initialization direct-initialization?\n[dcl.init]/16.8", shape=diamond]
        is_direct_init_for_nullptr -> is_source_type_nullptr [label="Yes"]
        is_direct_init_for_nullptr -> standard_conv_seq_initialization_head [label="No"]    

    is_source_type_nullptr [label="Is the source type std::nullptr_t?\n[dcl.init]/16.8", shape=diamond]
        is_source_type_nullptr -> is_dest_type_bool_for_nullptr [label="Yes"]
        is_source_type_nullptr -> standard_conv_seq_initialization_head [label="No"]

    is_dest_type_bool_for_nullptr [label="Is the destination type bool?\n[dcl.init]/16.8", shape=diamond]
        is_dest_type_bool_for_nullptr -> nullptr_to_bool_init [label="Yes"]
        is_dest_type_bool_for_nullptr -> standard_conv_seq_initialization_head [label="No"]

    nullptr_to_bool_init [label="The bool is initialized to false.\n[dcl.init]/16.8", shape=box]
        nullptr_to_bool_init -> done

    subgraph class_dest_initialization {
        class_dest_initialization_head [label="Initialization as follows:\n[dcl.init]/16.6", shape=box]
            class_dest_initialization_head -> class_is_initializer_prvalue

        class_is_initializer_prvalue [label="Is the initializer a prvalue?\n[dcl.init]/16.6.1", shape=diamond]
            class_is_initializer_prvalue -> class_is_initializer_prvalue_same_class [label="Yes"]
            class_is_initializer_prvalue -> class_is_copy_init [label="No"]

        class_is_initializer_prvalue_same_class [label="Is the source type the same as the destination type (up to cv-qualification)?\n[dcl.init]/16.6.1", shape=diamond]
            class_is_initializer_prvalue_same_class -> class_initialize_by_prvalue [label="Yes"]
            class_is_initializer_prvalue_same_class -> class_is_copy_init [label="No"]

        class_initialize_by_prvalue [label="Use the prvalue to initialize the destination object.\n[dcl.init]/16.6.1", shape=box]
            class_initialize_by_prvalue -> done

        class_is_copy_init [label="Is the initialization copy-initialization?\n[dcl.init]/16.6.2", shape=diamond]
            class_is_copy_init -> class_is_copy_init_same_class [label="Yes"]
            class_is_copy_init -> class_is_direct_init [label="No"]
        
        class_is_copy_init_same_class [label="Is the source type the same class as the destination type (up to cv qualification)?\n[dcl.init]/16.6.2", shape=box]
            class_is_copy_init_same_class -> class_consider_constructors [label="Yes"]
            class_is_copy_init_same_class -> class_is_copy_init_derived_class [label="No"]

        class_is_copy_init_derived_class [label="Is the source type a derived class of the destination type?\n[dcl.init]/16.6.2", shape=box]
            class_is_copy_init_derived_class -> class_consider_constructors [label="Yes"]
            class_is_copy_init_derived_class -> class_user_defined_conv_head [label="No"]

        class_is_direct_init [label="The initialization is direct-initialization.\n[dcl.init]/16.6.2", shape=box]
            class_is_direct_init -> class_consider_constructors

        class_consider_constructors [label="Enumerate constructors and select best through overload resolution.\n[dcl.init]/16.6.2", shape=box]
            class_consider_constructors -> class_constructors_is_resolution_successful

        class_constructors_is_resolution_successful [label="Is overload resolution succesful?\n[dcl.init]/16.6.2", shape=diamond]
            class_constructors_is_resolution_successful -> class_constructors_use_selected [label="Yes"]
            class_constructors_is_resolution_successful -> class_is_aggregate [label="No"]

        class_constructors_use_selected [label="Use the selected constructor to initialize the object, using the expression or expression-list as argument(s).\n[dcl.init]/16.6.2.1", shape=box]
            class_constructors_use_selected -> done

        class_is_aggregate [label="Is the destination type an aggregate class?\n[dcl.init]/16.6.2.2", shape=diamond]
            class_is_aggregate -> class_aggregate_is_initializer_expr_list [label="Yes"]
            class_is_aggregate -> class_ill_formed [label="No"]

        class_aggregate_is_initializer_expr_list [label="Is the initializer a parenthesized expression-list?\n[dcl.init]/16.6.2.2", shape=diamond]
            class_aggregate_is_initializer_expr_list -> class_aggregate_paren_init_head [label="Yes"]
            class_aggregate_is_initializer_expr_list -> class_ill_formed [label="No"]

        class_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        subgraph class_aggregate_paren_init {
            class_aggregate_paren_init_head [label="Initialized as follows:\n[dcl.init]/16.6.2.2", shape=box]
                class_aggregate_paren_init_head -> class_aggregate_paren_n_defn

            class_aggregate_paren_n_defn [label="Let n be the number of elements in the aggregate.", shape=box]
                class_aggregate_paren_n_defn -> class_aggregate_paren_k_defn

            class_aggregate_paren_k_defn [label="Let k b ethe number of elements in the initializer's expression list.", shape=box]
                class_aggregate_paren_k_defn -> class_aggregate_paren_is_k_gt_n

            class_aggregate_paren_is_k_gt_n [label="Is k > n?", shape=diamond]
                class_aggregate_paren_is_k_gt_n -> class_aggregate_paren_ill_formed [label="Yes"]
                class_aggregate_paren_is_k_gt_n -> class_aggregate_paren_initialize_first_k [label="No"]

            class_aggregate_paren_initialize_first_k [label="Copy-initialize the first k elements from the expression list.", shape=box]
                class_aggregate_paren_initialize_first_k -> class_aggregate_paren_initialize_rest

            class_aggregate_paren_initialize_rest [label="Use default member initializer or value-initialize the remaining elements.", shape=box]
                class_aggregate_paren_initialize_rest -> done

            class_aggregate_paren_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]
        }

        subgraph class_user_defined_conv {
            class_user_defined_conv_head [label = "Initialization as follows:\n[dcl.init]/16.6.3", shape=box]
                class_user_defined_conv_head -> class_user_defined_conv_overload_resolution

            class_user_defined_conv_overload_resolution [label="Use overload resolution to select the best user-defined conversion that can convert from the source type to the destination type or (when a conversion function is used) to a derived class thereof.", shape=box]
                class_user_defined_conv_overload_resolution -> class_user_defined_conv_is_possible
            
            class_user_defined_conv_is_possible [label="Is the conversion ambiguous or impossible?", shape=diamond]
                class_user_defined_conv_is_possible -> class_user_defined_conv_ill_formed [label="Yes"]
                class_user_defined_conv_is_possible -> class_user_defined_conv_do_conversion [label="No"]

            class_user_defined_conv_do_conversion [label = "Call the selected function with the initializer-expression as its argument.", shape=box]
                class_user_defined_conv_do_conversion -> class_user_defined_conv_initialize

            class_user_defined_conv_initialize [label="Direct-initialize the destination object with the result of the conversion.", shape=box]
                class_user_defined_conv_initialize -> done

            class_user_defined_conv_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]
        }
    }

    subgraph string_literal_initialization {
        string_literal_initialization_head [label="Initialization as follows:\n[dcl.init.string]", shape=box]
            string_literal_initialization_head -> string_literal_verify_kind

        string_literal_verify_kind [label="Verify array type and literal type match.", shape=box]
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

        string_literal_initialize_first [label="Initialize the first elements of the array with successive values from the string literal.", shape=box]
            string_literal_initialize_first -> string_literal_has_too_many

        string_literal_has_too_many [label="Are there more initializers than array elements?", shape=diamond]
            string_literal_has_too_many -> string_literal_ill_formed_too_many [label="Yes"]
            string_literal_has_too_many -> string_literal_initialize_rest [label="No"]

        string_literal_ill_formed_too_many [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        string_literal_initialize_rest [label="Zero-initialize the remaining elements of the array (if any)."]
            string_literal_initialize_rest -> done
    }

    subgraph class_source_initialization {
        class_source_initialization_head [label="Initialized as follows:\n[dcl.init]/16.7", shape=box]
            class_source_initialization_head -> class_source_consider_conversion_functions

        class_source_consider_conversion_functions [label="Use overload resolution to select the best applicable conversion function.", shape=box]
            class_source_consider_conversion_functions -> class_source_conversion_is_impossible

        class_source_conversion_is_impossible [label="Is the conversion impossible or ambiguous?", shape=diamond]
            class_source_conversion_is_impossible -> class_source_conversion_ill_formed [label="Yes"]
            class_source_conversion_is_impossible -> class_source_initialize [label="No"]

        class_source_conversion_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        class_source_initialize [label="Use the result of the conversion to convert the initializer to the object being initialized."]
            class_source_initialize -> done
    }

    subgraph standard_conv_seq_initialization {
        standard_conv_seq_initialization_head [label="The object is initialized as follows:\n[dcl.init]/6.9", shape=box]
            standard_conv_seq_initialization_head -> standard_conv_seq_do_init

        standard_conv_seq_do_init [label="Initialize the object using the value of the initializer expression, using a standard conversion sequence if necessary, not considering any user-defined conversions.", shape=box]
            standard_conv_seq_do_init -> standard_conv_seq_is_possible

        standard_conv_seq_is_possible [label="Is the conversion possible?", shape=diamond]
            standard_conv_seq_is_possible -> standard_conv_seq_ill_formed [label="No"]
            standard_conv_seq_is_possible -> standard_conv_seq_is_bitfield [label="Yes"]

        standard_conv_seq_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        standard_conv_seq_is_bitfield [label="Is the object to be initialized a bit-field?", shape=diamond]
            standard_conv_seq_is_bitfield -> standard_conv_seq_is_bitfield_in_range [label="Yes"]
            standard_conv_seq_is_bitfield -> done [label="No"]

        standard_conv_seq_is_bitfield_in_range [label="Is the value representable by the bit-field?", shape=diamond]
            standard_conv_seq_is_bitfield_in_range -> standard_conv_seq_bitfield_imp_def [label="No"]
            standard_conv_seq_is_bitfield_in_range -> done [label="Yes"]

        standard_conv_seq_bitfield_imp_def [label="The value of the bit-field is implementation-defined.", shape=box]
            standard_conv_seq_bitfield_imp_def -> done
    }

    subgraph reference_initialization {
        reference_initialization_head [label="Reference initialization\n[dcl.init.ref]", shape=box]
            reference_initialization_head -> reference_dest_type_defn
        
        reference_dest_type_defn [label="Let the destination type be \"reference to cv1 T1\".\n[dcl.init.ref]/5", shape=box]
            reference_dest_type_defn -> reference_source_type_defn

        reference_source_type_defn [label="Let the source type be \"cv2 T2\".\n[dcl.init.ref]/5", shape=box]
            reference_source_type_defn -> reference_is_dest_lval

        reference_is_dest_lval [label="Is the destination type an lvalue reference?\n[dcl.init.ref]/5.1", shape=diamond]
            reference_is_dest_lval -> reference_dest_lval_is_source_lval [label="Yes"]
            reference_is_dest_lval -> reference_dest_is_lval_non_const [label="No"]

        reference_dest_lval_is_source_lval [label="Is the initializer an lvalue?\n[dcl.init.ref]/5.1", shape=diamond]
            reference_dest_lval_is_source_lval -> reference_lvals_is_compatible [label="Yes"]
            reference_dest_lval_is_source_lval -> reference_dest_lval_is_source_class [label="No"]

        reference_lvals_is_compatible [label="Is cv1 T1 reference-compatibile with cv2 T2?\n[dcl.init.ref]/5.1", shape=diamond]
            reference_lvals_is_compatible -> reference_lvals_compatible_bind [label="Yes"]
            reference_lvals_is_compatible -> reference_dest_lval_is_source_class [label="No"]

        reference_lvals_compatible_bind [label="The destination reference is bound to the initializer lvalue (or appropriate base).\n[dcl.init.ref]/5.1", shape=box]
            reference_lvals_compatible_bind -> done

        reference_dest_lval_is_source_class [label="Is T2 a class type?\n[dcl.init.ref]/5.1.2", shape=diamond]
            reference_dest_lval_is_source_class -> reference_dest_lval_source_class_is_reference_related [label="Yes"]
            reference_dest_lval_is_source_class -> reference_dest_is_lval_non_const [label="No"]

        reference_dest_lval_source_class_is_reference_related [label="Is T1 reference-related to T2?\n[dcl.init.ref]/5.1.2", shape=diamond]
            reference_dest_lval_source_class_is_reference_related -> reference_dest_lval_source_class_is_convertible [label="No"]
            reference_dest_lval_source_class_is_reference_related -> reference_dest_is_lval_non_const [label="Yes"]

        reference_dest_lval_source_class_is_convertible [label="Is T2 convertible to an lvalue of type cv3 T3 such that cv1 T1 is reference-compatible with cv3 T3?\n[dcl.init.ref]/5.1.2", shape=diamond]
            reference_dest_lval_source_class_is_convertible -> reference_class_select_conversion [label="Yes"]
            reference_dest_lval_source_class_is_convertible -> reference_dest_is_lval_non_const [label="No"]

        reference_class_select_conversion [label="Select the best applicable conversion function.\n[dcl.init.ref]/5.1.2", shape=box]
            reference_class_select_conversion -> reference_class_do_initialization

        reference_class_do_initialization [label="The destination reference is bound to the result of the conversion (or appropriate base).\n[dcl.init.ref]/5.1", shape=box]
            reference_class_do_initialization -> done

        reference_dest_is_lval_non_const [label="Is the destination an lvalue reference to a non-const type?\n[dcl.init.ref]/5.2", shape=diamond]
            reference_dest_is_lval_non_const -> reference_dest_non_const_ill_formed [label="Yes"]
            reference_dest_is_lval_non_const -> reference_dest_is_volatile [label="No"]

        reference_dest_non_const_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        reference_dest_is_volatile [label="Is the destination's referenced type volatile-qualified\n[dcl.init.ref]/5.2", shape=diamond]
            reference_dest_is_volatile -> reference_dest_volatile_ill_formed [label="Yes"]
            reference_dest_is_volatile -> reference_rval_conv_source_is_rvalue [label="No"]

        reference_dest_volatile_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        reference_rval_conv_source_is_rvalue [label="Is the initializer an rvalue?\n[dcl.init.ref]/5.3.1", shape=diamond]
            reference_rval_conv_source_is_rvalue -> reference_rval_conv_source_is_rvalue_bitfield [label="Yes"]
            reference_rval_conv_source_is_rvalue -> reference_rval_conv_source_is_function_lval [label="No"]

        reference_rval_conv_source_is_rvalue_bitfield [label="Is the initializer a bit-field?\n[dcl.init.ref]/5.3.1", shape=diamond]
            reference_rval_conv_source_is_rvalue_bitfield -> reference_rval_conv_source_rval_or_function_is_ref_compat [label="No"]
            reference_rval_conv_source_is_rvalue_bitfield -> reference_rval_conv_source_is_function_lval [label="Yes"]

        reference_rval_conv_source_is_function_lval [label="Is the initializer a function lvalue?\n[dcl.init.ref]/5.3.1", shape=diamond]
            reference_rval_conv_source_is_function_lval -> reference_rval_conv_source_rval_or_function_is_ref_compat [label="Yes"]
            reference_rval_conv_source_is_function_lval -> reference_rval_conv_source_is_class [label="No"]

        reference_rval_conv_source_rval_or_function_is_ref_compat [label="Is cv1 T1 reference-compatible with cv2 T2?\n[dcl.init.ref]/5.3.1", shape=diamond]
            reference_rval_conv_source_rval_or_function_is_ref_compat -> reference_rval_conv_bind_direct [label="Yes"]
            reference_rval_conv_source_rval_or_function_is_ref_compat -> reference_rval_conv_source_is_class [label="No"]

        reference_rval_conv_source_is_class [label="Is T2 a class type?\n[dcl.init.ref]/5.3.2", shape=diamond]
            reference_rval_conv_source_is_class -> reference_rval_conv_source_class_is_ref_related [label="Yes"]
            reference_rval_conv_source_is_class -> reference_temp_is_dest_class [label="No"]

        reference_rval_conv_source_class_is_ref_related [label="Is T1 reference-related to T2?\n[dcl.init.ref]/5.3.2", shape=diamond]
            reference_rval_conv_source_class_is_ref_related -> reference_rval_conv_source_class_convertible_target [label="No"]
            reference_rval_conv_source_class_is_ref_related -> reference_temp_is_dest_class [label="Yes"]

        reference_rval_conv_source_class_convertible_target [label="Is the initializer convertible to an rvalue or function lvalue of type \"cv3 T3\", where \"cv1 T1\" is reference-compatible with \"cv3 T3\"?\n[dcl.init.ref]/5.3.2", shape=diamond]
            reference_rval_conv_source_class_convertible_target -> reference_rval_conv_bind_converted [label="Yes"]
            reference_rval_conv_source_class_convertible_target -> reference_temp_is_dest_class [label="No"]

        reference_rval_conv_bind_direct [label="The converted initializer is the value of the initializer.\n[dcl.init.ref]/5.3", shape=box]
            reference_rval_conv_bind_direct -> reference_rval_conv_is_converted_prval

        reference_rval_conv_bind_converted [label="The converted initializer is the result of the conversion.\n[dcl.init.ref]/5.3", shape=box]
            reference_rval_conv_bind_converted -> reference_rval_conv_is_converted_prval

        reference_rval_conv_is_converted_prval [label="Is the converted initializer a prvalue?\n[dcl.init.ref]/5.3", shape=diamond]
            reference_rval_conv_is_converted_prval -> reference_rval_conv_prval_adjust_type [label="Yes"]
        reference_rval_conv_is_converted_prval -> reference_rval_conv_bind_glval [label="No"]

        reference_rval_conv_prval_adjust_type [label="Its type T4 is adjusted to \"cv1 T4\".\n[dcl.init.ref]/5.3", shape=box]
            reference_rval_conv_prval_adjust_type -> reference_rval_conv_prval_materialize

        reference_rval_conv_prval_materialize [label="The prvalue is materialized.\n[dcl.init.ref]/5.3", shape=box]
            reference_rval_conv_prval_materialize -> reference_rval_conv_bind_glval

        reference_rval_conv_bind_glval [label="The destination reference is bound to the resulting glvalue.\n[dcl.init.ref]/5.3", shape=box]
            reference_rval_conv_bind_glval -> done

        reference_temp_is_dest_class [label="Is T1 a class type?\n[dcl.init.ref]/5.4.1", shape=diamond]
            reference_temp_is_dest_class -> reference_temp_is_related [label="Yes"]
            reference_temp_is_dest_class -> reference_temp_is_source_class [label="No"]

        reference_temp_is_source_class [label="Is T2 a class type?\n[dcl.init.ref]/5.4.1", shape=diamond]
            reference_temp_is_source_class -> reference_temp_is_related [label="Yes"]
            reference_temp_is_source_class -> reference_temp_implicit_conv [label="No"]

        reference_temp_is_related [label="Is T1 reference-related to T2?\n[dcl.init.ref]/5.4.1", shape=diamond]
            reference_temp_is_related -> reference_temp_user_defined_conv [label="No"]
            reference_temp_is_related -> reference_temp_implicit_conv [label="Yes"]

        reference_temp_user_defined_conv [label="Consider user-defined conversions for the copy-initialization of an object of type \"cv1 T1\" by user-defined-conversion.\n[dcl.init.ref]/5.4.1", shape=box]
            reference_temp_user_defined_conv -> reference_temp_user_defined_conv_is_ill_formed

        reference_temp_user_defined_conv_is_ill_formed [label="Would the non-reference copy-initialization be ill-formed?\n[dcl.init.ref]/5.4.1", shape=diamond]
            reference_temp_user_defined_conv_is_ill_formed -> reference_temp_user_defined_conv_ill_formed [label="Yes"]
            reference_temp_user_defined_conv_is_ill_formed -> reference_temp_user_defined_conv_direct_initialize [label="No"]

        reference_temp_user_defined_conv_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        reference_temp_user_defined_conv_direct_initialize [label="The result of the call to the conversion function, as described by non-reference copy-initialization, is used to direct-initialize the reference. For the direct-initialization, user-defined conversions are not considered.\n[dcl.init.ref]/5.4.1", shape=box]
            reference_temp_user_defined_conv_direct_initialize -> done

        reference_temp_implicit_conv [label="The initializer expression is implicitly converted to a prvalue of type \"cv1 T1\".\n[dcl.init.ref]/5.4.2", shape=box]
            reference_temp_implicit_conv -> reference_temp_implicit_conv_materialize

        reference_temp_implicit_conv_materialize [label="The temporary is materialized.\n[dcl.init.ref]/5.4.2", shape=box]
            reference_temp_implicit_conv_materialize -> reference_temp_implicit_conv_materialize_bind

        reference_temp_implicit_conv_materialize_bind [label="The reference is bound to the result.\n[dcl.init.ref]/5.4.2", shape=box]
            reference_temp_implicit_conv_materialize_bind -> reference_temp_implicit_conv_materialize_is_reference_related

        reference_temp_implicit_conv_materialize_is_reference_related [label="Is T1 reference-related to T2?\n[dcl.init.ref]/5.4", shape=diamond]
            reference_temp_implicit_conv_materialize_is_reference_related -> reference_temp_implicit_conv_materialize_is_cv_okay [label="Yes"]
            reference_temp_implicit_conv_materialize_is_reference_related -> done [label="No"]

        reference_temp_implicit_conv_materialize_is_cv_okay [label="Is cv1 more qualified than cv2?\n[dcl.init.ref]/5.4.3", shape=diamond]
            reference_temp_implicit_conv_materialize_is_cv_okay -> reference_temp_implicit_conv_materialize_is_dest_rval [label="Yes"]
            reference_temp_implicit_conv_materialize_is_cv_okay -> reference_temp_implicit_conv_materialize_cv_ill_formed [label="No"]

        reference_temp_implicit_conv_materialize_cv_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        reference_temp_implicit_conv_materialize_is_dest_rval [label="Is the destination an rvalue reference?\n[dcl.init.ref]/5.4.3", shape=diamond]
            reference_temp_implicit_conv_materialize_is_dest_rval -> reference_temp_implicit_conv_materialize_is_source_lval [label="Yes"]
            reference_temp_implicit_conv_materialize_is_dest_rval -> done [label="No"]

        reference_temp_implicit_conv_materialize_is_source_lval [label="Is the initializer an lvalue?\n[dcl.init.ref]/5.4.4", shape=diamond]
            reference_temp_implicit_conv_materialize_is_source_lval -> reference_temp_implicit_conv_materialize_source_lval_ill_formed [label="Yes"]
            reference_temp_implicit_conv_materialize_is_source_lval -> done [label="No"]

        reference_temp_implicit_conv_materialize_source_lval_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]
    }

    subgraph value_initialization {
        value_initialization_head [label="Value-initialization\n[dcl.init]/8", shape=box]
            value_initialization_head -> value_is_class

        value_is_class [label="Is the type a class type?\n[dcl.init]/8.1", shape=diamond]
            value_is_class -> value_has_dflt_ctor [label="Yes"]
            value_is_class -> value_is_array [label="No"]

        value_has_dflt_ctor [label="Does the type have a default constructor?\n[dcl.init]/8.1.1", shape=diamond]
            value_has_dflt_ctor -> value_default_initialize [label="No"]
            value_has_dflt_ctor -> value_has_deleted_dflt_ctor [label="Yes"]

        value_has_deleted_dflt_ctor [label="Does the type have a deleted default constructor?\n[dcl.init]/8.1.1", shape=diamond]
            value_has_deleted_dflt_ctor -> value_default_initialize [label="Yes"]
            value_has_deleted_dflt_ctor -> value_has_user_dflt_ctor [label="No"]

        value_has_user_dflt_ctor [label="Does the type have a user-provided default constructor?\n[dcl.init]/8.1.1", shape=diamond]
            value_has_user_dflt_ctor -> value_default_initialize [label="Yes"]
            value_has_user_dflt_ctor -> value_zero_initialize_class [label="No"]

        value_zero_initialize_class [label="The object is zero-initialized.", shape=box]
            value_zero_initialize_class -> value_check_default

        value_is_array [label="Is the type an array type?\n[dcl.init]/8.2", shape=diamond]
            value_is_array -> value_value_initialize_elements [label="Yes"]
            value_is_array -> value_zero_initialize_fallback [label="No"]

        value_value_initialize_elements [label="Each element is value-initialized.", shape=box]
            value_value_initialize_elements -> done

        value_zero_initialize_fallback [label="The object is zero-initialized.", shape=box]
            value_zero_initialize_fallback -> done

        value_default_initialize [label="The object is default-initialized.\n[dcl.init]/8.1.*", shape=box]
            value_default_initialize -> done

        value_check_default [label="The semantic constraints for default-initialization are checked.\n[dcl.init]/8.1.2", shape=box]
            value_check_default -> value_has_nontrivial_dflt_ctor

        value_has_nontrivial_dflt_ctor [label="Does the type have a non-trivial default constructor?\n[dcl.init]/8.1.2", shape=diamond]
            value_has_nontrivial_dflt_ctor -> value_default_initialize [label="Yes"]
            value_has_nontrivial_dflt_ctor -> done [label="No"]
    }

    subgraph list_initialization {
        list_initialization_head [label="List-initialization\n[dcl.init.list]/3]", shape=box]
            list_initialization_head -> list_has_designated_initializer

        list_has_designated_initializer [label="Does the braced-init-list contain a designated-initializer-list?\n[dcl.init.list]/3.1", shape=diamond]
            list_has_designated_initializer -> list_designated_initalizer_is_aggregate [label="Yes"]
            list_has_designated_initializer -> list_is_aggregate_class [label="No"]

        list_designated_initalizer_is_aggregate [label="Is the type an aggregate class?\n[dcl.init.list]/3.1", shape=diamond]
            list_designated_initalizer_is_aggregate -> list_designated_initializer_are_identifiers_valid [label="Yes"]
            list_designated_initalizer_is_aggregate -> list_designated_initalizer_nonaggregate_ill_formed [label="No"]

        list_designated_initalizer_nonaggregate_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        list_designated_initializer_are_identifiers_valid [label="Do the designators form a subsequence of the ordered idenitifiers in the direct non-static data members of the type?\n[dcl.init.list]/3.1", shape=diamond]
            list_designated_initializer_are_identifiers_valid -> list_designated_initializer_aggregate_init [label="Yes"]
            list_designated_initializer_are_identifiers_valid ->  list_designated_initalizer_initializers_ill_formed [label="No"]

        list_designated_initalizer_initializers_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        list_designated_initializer_aggregate_init [label="Aggregate initialization is performed.\n[dcl.init.list]/3.1", shape=diamond]
        list_designated_initializer_aggregate_init -> aggregate_initialization_head

        list_is_aggregate_class [label="Is the type an aggregate class?\n[dcl.init.list]/3.2", shape=diamond]
            list_is_aggregate_class -> list_aggregate_is_list_singleton [label="Yes"]
            list_is_aggregate_class -> list_is_type_char_array [label="No"]

        list_aggregate_is_list_singleton [label="Does the initializer list have a single element?\n[dcl.init.list]/3.2", shape=diamond]
            list_aggregate_is_list_singleton -> list_aggregate_singleton_is_type_valid [label="Yes"]
            list_aggregate_is_list_singleton -> list_is_type_char_array [label="No"]

        list_aggregate_singleton_is_type_valid [label="Does the sole element have type \"cv U\", where U is the initialized type or a type derived of it?\n[dcl.init.list]/3.2", shape=diamond]
            list_aggregate_singleton_is_type_valid -> list_aggregate_singleton_type_init_type [label="Yes"]
            list_aggregate_singleton_is_type_valid -> list_is_type_char_array [label="No"]

        list_aggregate_singleton_type_init_type [label="What is the type of initialization?\n[dcl.init.list]/3.2", shape=diamond]
            list_aggregate_singleton_type_init_type -> list_aggregate_singleton_type_copy [label="copy-list-initialization"]
            list_aggregate_singleton_type_init_type -> list_aggregate_singleton_type_direct [label="direct-list-initialization"]

        list_aggregate_singleton_type_copy [label="The object is copy-initialized from the sole element.\n[dcl.init.list]/3.2"]
            list_aggregate_singleton_type_copy -> done

        list_aggregate_singleton_type_direct [label="The object is direct-initialized from the sole element.\n[dcl.init.list]/3.2"]
            list_aggregate_singleton_type_direct -> done

        list_is_type_char_array [label="Is the type a character array?\n[dcl.init.list]/3.3", shape=diamond]
            list_is_type_char_array -> list_char_array_is_singleton [label="Yes"]
            list_is_type_char_array -> list_is_aggregate [label="No"]

        list_char_array_is_singleton [label="Does the iniitializer list have a single element?\n[dcl.init.list/]3.3", shape=diamond]
            list_char_array_is_singleton -> list_char_array_singleton_is_typed [label="Yes"]
            list_char_array_is_singleton -> list_is_aggregate [label="No"]

        list_char_array_singleton_is_typed [label="Is that element an appropriately-typed string-literal?\n[dcl.init.list]/3.3"]
            list_char_array_singleton_is_typed -> list_char_array_string_literal_init [label=""]
            list_char_array_singleton_is_typed -> list_is_aggregate [label="No"]

        list_char_array_string_literal_init [label="Initialization as in [dcl.init.string]\n[dcl.init.list]/3.3"]
            list_char_array_string_literal_init -> string_literal_initialization_head

        list_is_aggregate [label="Is the type an aggregate?\n[dcl.init.list]/3.4", shape=diamond]
            list_is_aggregate -> list_aggregate_aggregate_initialization [label="Yes"]
            list_is_aggregate -> list_is_list_empty [label="No"]

        list_aggregate_aggregate_initialization [label="Aggregate initialization is performed.\n[dcl.init.list]/3.4", shape=box]
            list_aggregate_aggregate_initialization -> aggregate_initialization_head

        list_is_list_empty [label="Is the initializer list empty?\n[dcl.init.list]/3.5", shape=diamond]
            list_is_list_empty -> list_empty_is_class [label="Yes"]
            list_is_list_empty -> list_dest_is_initializer_list [label="No"]

        list_empty_is_class [label="Is the destination type a class type?\n[dcl.init.list]/3.5", shape=diamond]
            list_empty_is_class -> list_empty_has_default_constructor [label="Yes"]
            list_empty_is_class -> list_dest_is_initializer_list [label="No"]

        list_empty_has_default_constructor [label="Does the class have a default constructor?\n[dcl.init.list]/3.5", shape=diamond]
            list_empty_has_default_constructor -> list_empty_value_initialize [label="Yes"]
            list_empty_has_default_constructor -> list_dest_is_initializer_list [label="No"]

        list_empty_value_initialize [label="The object is value-initialized.\n[dcl.init.list]/3.5", shape=box]
            list_empty_value_initialize -> done

        list_dest_is_initializer_list [label="Is the type a specialization of std::initializer_list?\n[dcl.init.list]/3.6"]
            list_dest_is_initializer_list -> list_initializer_list_init [label="Yes"]
            list_dest_is_initializer_list -> list_is_class [label="No"]

        list_initializer_list_init [label="Initialized as follows:\n[dcl.init.list]/5", shape=box]
            list_initializer_list_init -> list_initializer_list_n_defn

        list_initializer_list_n_defn [label="Let N be the number of elements in the initalizer list.", shape=box]
            list_initializer_list_n_defn -> list_initializer_list_materialize_array

        list_initializer_list_materialize_array [label="Where type is std::initializer_list<E>, a prvalue of type \"array of N const E\" is materialized.", shape=box]
            list_initializer_list_materialize_array -> list_initializer_list_init_array

        list_initializer_list_init_array [label="Each element of the array is copy-initialized with the corresponding element of the initializer list.", shape=box]
            list_initializer_list_init_array -> list_initializer_list_is_narrowing

        list_initializer_list_is_narrowing [label="Is a narrowing conversion required to initialize any of the elements?", shape=diamond]
            list_initializer_list_is_narrowing -> list_initializer_list_narrowing_ill_formed [label="Yes"]
            list_initializer_list_is_narrowing -> list_initializer_list_init_object [label="No"]

        list_initializer_list_narrowing_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        list_initializer_list_init_object [label="The initializer_list is constructed to refer to the materialized array.", shape=box]
            list_initializer_list_init_object -> done

        list_is_class [label="Is the type a class type?\n[dcl.init.list]/3.7", shape=diamond]
            list_is_class -> list_class_ctors [label="Yes"]
            list_is_class -> list_is_enum [label="No"]

        list_class_ctors [label="Constructors are considered, and the best match is selected through overload resolution.\n[dcl.init.list]/3.7", shape=box]
            list_class_ctors -> list_class_is_narrowing

        list_class_is_narrowing [label="Is a narrowing conversion required to convert any of the arguments?\n[dcl.init.list]/3.7", shape=diamond]
            list_class_is_narrowing -> list_class_narrowing_ill_formed [label="Yes"]
            list_class_is_narrowing -> done [label="No"]

        list_class_narrowing_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        list_is_enum [label="Is the type an enumeration?\n[dcl.init.list]/3.8", shape=diamond]
            list_is_enum -> list_enum_is_fixed [label="Yes"]
            list_is_enum -> list_final_is_singleton [label="No"]

        list_enum_is_fixed [label="Does the enumeration have fixed underlying type?\n[dcl.init.list]/3.8", shape=diamond]
            list_enum_is_fixed -> list_enum_underlying_defn [label="Yes"]
            list_enum_is_fixed -> list_final_is_singleton [label="No"]

        list_enum_underlying_defn [label="Let U be the underlying type.\n[dcl.init.list]/3.8", shape=box]
            list_enum_underlying_defn -> list_enum_is_singleton

        list_enum_is_singleton [label="Does the initializer list have a single element?\n[dcl.init.list]/3.8", shape=diamond]
            list_enum_is_singleton -> list_enum_elem_defn [label="Yes"]
            list_enum_is_singleton -> list_final_is_singleton [label="No"]

        list_enum_elem_defn [label="Let v be that element.\n[dcl.init.list]/3.8", shape=box]
            list_enum_elem_defn -> list_enum_is_convertible

        list_enum_is_convertible [label="Can v be implicitly converted to U?\n[dcl.init.list]/3.8", shape=diamond]
            list_enum_is_convertible -> list_enum_is_direct [label="Yes"]
            list_enum_is_convertible -> list_final_is_singleton [label="No"]

        list_enum_is_direct [label="Is the initialization direct-list-initialization?\n[dcl.init.list]/3.8", shape=diamond]
            list_enum_is_direct -> list_enum_is_narrowing [label="Yes"]
            list_enum_is_direct -> list_final_is_singleton [label="No"]

        list_enum_is_narrowing [label="Is a narrowing conversion required to convert v to U?\n[dcl.init.list]/3.8"]
            list_enum_is_narrowing -> list_enum_narrowing_ill_formed [label="Yes"]
            list_enum_is_narrowing -> list_enum_initialization [label="No"]

        list_enum_initialization [label="The object is initialized with the value T(u).", shape=box]
            list_enum_initialization -> done

        list_enum_narrowing_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        // Final just because I couldn't come up with a better name for it. "Final" as in "last".

        list_final_is_singleton [label="Does the initializer list have a single element?\n[dcl.init.list]/3.9", shape=diamond]
            list_final_is_singleton -> list_final_singleton_type_defn [label="Yes"]
            list_final_is_singleton -> list_ref_prvalue_is_ref [label="No"]

        list_final_singleton_type_defn [label="Let E be the type of that element.\n[dcl.init.list]/3.9", shape=box]
            list_final_singleton_type_defn -> list_final_singleton_is_dest_ref

        list_final_singleton_is_dest_ref [label="Is the destination type a reference?\n[dcl.init.list]/3.9", shape=diamond]
            list_final_singleton_is_dest_ref -> list_final_singleton_is_dest_ref_related [label="Yes"]
            list_final_singleton_is_dest_ref -> list_final_singleton_type [label="No"]

        list_final_singleton_is_dest_ref_related [label="Is the destination type's referenced type reference-related to E?\n[dcl.init.list]/3.9", shape=diamond]
            list_final_singleton_is_dest_ref_related -> list_final_singleton_type [label="Yes"]
            list_final_singleton_is_dest_ref_related -> list_ref_prvalue_is_ref [label="No"]

        list_final_singleton_type [label="What is the type of initialization?\n[dcl.init.list]/3.9"]
            list_final_singleton_type -> list_final_singleton_direct [label="direct-list-initialization"]
            list_final_singleton_type -> list_final_singleton_copy [label="copy-list-initialization"]

        list_final_singleton_direct [label="The destination is initialized by direct-initialization from the element.\n[dcl.init.list]/3.9", shape=box]
            list_final_singleton_direct -> list_final_singleton_is_narrowing

        list_final_singleton_copy [label="The destination is initialized by copy-initialization from the element.\n[dcl.init.list]/3.9", shape=box]
            list_final_singleton_copy -> list_final_singleton_is_narrowing

        list_final_singleton_is_narrowing [label="Is a narrowing conversion required to convert the element to the destination type?\n[dcl.init.list]/3.9", shape=diamond]
            list_final_singleton_is_narrowing -> done [label="No"]
            list_final_singleton_is_narrowing -> list_final_singleton_narrowing_ill_formed [label="Yes"]

        list_final_singleton_narrowing_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

        list_ref_prvalue_is_ref [label="Is the destination type a reference type?\n[dcl.init.list]/3.10", shape=diamond]
            list_ref_prvalue_is_ref -> list_ref_prvalue_prvalue_generated [label="Yes"]
            list_ref_prvalue_is_ref -> list_final_is_empty [label="No"]

        list_ref_prvalue_prvalue_generated [label="A prvalue is generated.\n[dcl.init.list]/3.10", shape=box]
            list_ref_prvalue_prvalue_generated -> list_ref_prvalue_type_is_unknown_bound

        list_ref_prvalue_type_is_unknown_bound [label="Is the destination type an array of unknown bound?\n[dcl.init.list]/3.10", shape=diamond]
            list_ref_prvalue_type_is_unknown_bound -> list_ref_prvalue_type_unknown_bound [label="Yes"]
            list_ref_prvalue_type_is_unknown_bound -> list_ref_prvalue_type_normal [label="No"]

        list_ref_prvalue_type_normal [label="The type of the prvalue is the type referenced by the destination type.\n[dcl.init.list]/3.10", shape=box]
            list_ref_prvalue_type_normal -> list_ref_prvalue_init_prvalue

        list_ref_prvalue_type_unknown_bound [label="The type of the prvalue is the type of x in U x[] H, where H is the initializer list.\n[dcl.init.list]/3.10", shape=box]
            list_ref_prvalue_type_unknown_bound -> list_ref_prvalue_init_prvalue

        list_ref_prvalue_init_prvalue [label="The prvalue initializes a result object by copy-list-initialization.\n[dcl.init.list]/3.10", shape=box]
            list_ref_prvalue_init_prvalue -> list_ref_prvalue_init_ref

        list_ref_prvalue_init_ref [label="The reference is direct-initialized by the prvalue.\n[dcl.init.list]/3.10", shape=box]
            list_ref_prvalue_init_ref -> done 

        // Final, again, as in "last".
        list_final_is_empty [label="Is the initializer list empty?\n[dcl.init.list]/3.11", shape=diamond]
            list_final_is_empty -> list_final_empty_value_init [label="Yes"]
            list_final_is_empty -> list_nothing_else_ill_formed [label="No"]

        list_final_empty_value_init [label="The object is value-initialized.\n[dcl.init.list]/3.12", shape=box]
            list_final_empty_value_init -> done

        list_nothing_else_ill_formed [label = "The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]

    }
}
