#include <vslc.h>


void
node_print ( node_t *root, int nesting )
{
    if ( root != NULL )
    {
        printf ( "%*c%s", nesting, ' ', node_string[root->type] );
        if ( root->type == IDENTIFIER_DATA ||
             root->type == STRING_DATA ||
             root->type == RELATION ||
             root->type == EXPRESSION ) 
            printf ( "(%s)", (char *) root->data );
        else if ( root->type == NUMBER_DATA )
            printf ( "(%ld)", *((int64_t *)root->data) );
        putchar ( '\n' );
        for ( int64_t i=0; i<root->n_children; i++ )
            node_print ( root->children[i], nesting+1 );
    }
    else
        printf ( "%*c%p\n", nesting, ' ', root );
}


void
node_init (node_t *nd, node_index_t type, void *data, uint64_t n_children, ...)
{
    va_list child_list;
    *nd = (node_t) {
        .type = type,
        .data = data,
        .entry = NULL,
        .n_children = n_children,
        .children = (node_t **) malloc ( n_children * sizeof(node_t *) )
    };
    va_start ( child_list, n_children );
    for ( uint64_t i=0; i<n_children; i++ )
        nd->children[i] = va_arg ( child_list, node_t * );
    va_end ( child_list );
}


void
node_finalize ( node_t *discard )
{
    if ( discard != NULL )
    {
        free ( discard->data );
        free ( discard->children );
        free ( discard );
    }
}


void
destroy_subtree ( node_t *discard ){
    if ( discard != NULL )
    {
        for ( uint64_t i=0; i<discard->n_children; i++ )
            destroy_subtree ( discard->children[i] );
        node_finalize ( discard );
    }
}


void
simplify_tree ( node_t **simplified, node_t *root )
{
    *simplified = root;
	if (!root) {
		return;
	}
	simplify_node(
}

void
simplify_node ( node_t *node)
{
	if ( node->data == NULL && !is_list_node(node) &&
	     node->type != PROGRAM && node->type != BLOCK &&
	     node->type != RETURN_STATEMENT && node->type != DECLARATION){
		if ( node->n_children == 1){
			node->data == node->children[0]->data;
			node->children == node->children[0]->children;
			node->n_children == node->children[0]->n_children;
			node->type == node->children[0]->type;
		}
	}

}


void
simplify_list( node_t *node, node_t *parent)
{
	if (is_list(node)){
		if (is_list(node->children[0]) && node->n_children == 1) {
			node->data = node->children[0]->data;
			node->children = node->children[0]->children;
			node->n_children = node->children[0]->n_children;
			node->type = node->children[0]->type;

		} else if (parent != NULL && is_list_node(node) && is_list_node(parent) && node->type == parent->type){
			int child_count = node->n_children + 1;
			node_t** children_list = (node_t **) malloc(child_count * sizeof(node_t *));
			for (int i = 0; node->n_children; i++)  {
				children_list[i] = node->children[i];
			}
			children_list[child_count - 1] = parent_node->children[1];
			parent_node->children = children_list;
			parent_node->n_children = child_count;
		}
	}
}

void
print_list_to_print_statement(node_t *node)
{
	if (node->type == PRINT_LIST) {
		node->type = PRINT_STATEMENT;
	}
}

int
is_list_node(node_t *node)
{
	return (node->type == GLOBAL_LIST ||
		node->type == STATEMENT_LIST ||
		node->type == PRINT_LIST ||
		node->type == EXPRESSION_LIST ||
		node->type == VARIABLE_LIST ||
		node->type == ARGUMENT_LIST ||
		node->type == PARAMETER_LIST ||
		node->type == DECLARATION_LIST);
}
