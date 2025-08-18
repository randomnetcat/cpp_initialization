#define TOKEN_CAT_(X, Y) X##Y
#define TOKEN_CAT(X, Y) TOKEN_CAT_(X, Y)

#define COLOR_GREEN "#008000"

#define DONE_NODE(id) { id [label="Done.", style=filled, fillcolor=COLOR_GREEN, shape=box, color=COLOR_GREEN, fontcolor=white]; }
#define ILL_FORMED_NODE(id) { id [label="The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]; }
#define RECURSE_SUBOBJECT_NODE(id) { id [label="Partially recurse.", shape=box, style=filled, color=lightblue, fontcolor=black]}
#define RECURSE_NODE(id) { id [label="Recurse.", shape=box, style=filled, color=blue, fontcolor=white]}
#define YES_NODE(id, text) { id [label=text, shape=box, color=COLOR_GREEN, fontcolor=black]; }
#define NO_NODE(id, text) { id [label=text, shape=box, color=red, fontcolor=black]; }

#define NEW_DONE() { DONE_NODE(TOKEN_CAT(__local_done_, __COUNTER__)); }
#define NEW_ILL_FORMED() { ILL_FORMED_NODE(TOKEN_CAT(__local_ill_formed_, __COUNTER__)); }
#define NEW_RECURSE() { RECURSE_NODE(TOKEN_CAT(__local_recurse, __COUNTER__)); }
#define NEW_RECURSE_SUBOBJECT() { RECURSE_SUBOBJECT_NODE(TOKEN_CAT(__local_recurse_subobject, __COUNTER__)); }
#define NEW_YES(label) { YES_NODE(TOKEN_CAT(__local_yes_, __COUNTER__), label); }
#define NEW_NO(label) { NO_NODE(TOKEN_CAT(__local_no_, __COUNTER__), label); }

#define INTERNALLY_RECURSES_SUBOBJECT(id) { id -> { RECURSE_SUBOBJECT_NODE(TOKEN_CAT(id, __recurse)); } [style="dotted"]; }

// Arguments: name of source node, optional edge attrs.
#define LINK_TO_DONE(id, ...) { id -> { DONE_NODE(TOKEN_CAT(id, __generated_done)); } __VA_ARGS__; }
#define LINK_TO_ILL_FORMED(id, ...) { id -> { ILL_FORMED_NODE(TOKEN_CAT(id, __generated_ill_formed)); } __VA_ARGS__; }

// Arguments: name, description (string literal), optional citation.
#define QUESTION_NODE(id, text, ...) { id [shape=diamond, label=text __VA_OPT__(+ "\n" +) __VA_ARGS__]; }
#define INSTRUCTION_NODE(id, text, ...) { id [shape=box, label=text __VA_OPT__(+ "\n" +) __VA_ARGS__]; }

// Arguments: name, description (string literal), citation (string literal), yes node ID, no node ID.
#define YN_QUESTION_NODE(id, text, cite, yes_id, no_id) { \
	QUESTION_NODE(id, text, cite); \
	id -> yes_id [label="Yes"]; \
	id -> no_id [label="No"]; \
}

// Arguments: name, description (string literal), citation (string literal), yes node ID, no node ID.
#define YN_QUESTION_NODE_NO_CITE(id, text, yes_id, no_id) { \
	QUESTION_NODE(id, text); \
	id -> yes_id [label="Yes"]; \
	id -> no_id [label="No"]; \
}

#define INIT_AS_FOLLOWS(id, ...) { INSTRUCTION_NODE(id, "Initialize the object as follows:", __VA_ARGS__); }

#define LOOP_BACKEDGE(source, loop_header) { source -> loop_header [style="dashed", constraint="true"]; }
