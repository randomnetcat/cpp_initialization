#define TOKEN_CAT_(X, Y) X##Y
#define TOKEN_CAT(X, Y) TOKEN_CAT_(X, Y)

#define DONE_NODE(id) { id [label="Done.", style=filled, fillcolor=green, shape=box, color=green, fontcolor=white]; }
#define ILL_FORMED_NODE(id) { id [label="The program is ill-formed.", shape=box, style=filled, color=red, fontcolor=white]; }
#define RECURSE_NODE(id) { id [label="Recurse with the new initialization.", shape=box, style=filled, color=blue, fontcolor=white]}

#define NEW_DONE() { DONE_NODE(TOKEN_CAT(__local_done_, __COUNTER__)); }
#define NEW_ILL_FORMED() { ILL_FORMED_NODE(TOKEN_CAT(__local_ill_formed_, __COUNTER__)); }
#define NEW_RECURSE() { RECURSE_NODE(TOKEN_CAT(__local_recurse, __COUNTER__)); }
#define INTERNALLY_RECURSES(id) { id -> { RECURSE_NODE(TOKEN_CAT(id, __recurse)); } [style="dotted"]; }

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

#define INIT_AS_FOLLOWS(id, ...) { INSTRUCTION_NODE(id, "The object is initialized as follows:", __VA_ARGS__); }

#define LOOP_BACKEDGE(source, loop_header) { source -> loop_header [style="dashed", constraint="true"]; }
