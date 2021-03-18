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



void
simplify_node ( node_t *node )
{
	if ( node->data == NULL && !is_list_node(node) &&
	     node->type != PROGRAM && node->type != BLOCK &&
	     node->type != RETURN_STATEMENT && node->type != DECLARATION){
		if ( node->n_children == 1){
			node_t *child = node->children[0];
			printf("(4.1) Simplified node of type: %s with child of type: %s\n", node_string[node->type], node_string[child->type]);
			node->data = child->data;
			node->n_children = child->n_children;
			node->children = child->children;
			node->type = child->type;
			free ( child );
		}
	}

}

void
simplify_list( node_t *node, node_t *parent)
{
	if (is_list_node(node)){
		if (is_list_node(node->children[0]) && node->n_children == 1) {
			node_t *child = node->children[0];
			printf("(4.2) Flattened node of type: %s with child of type: %s\n", node_string[node->type], node_string[child->type]);
			node->data = child->data;
			node->children = child->children;
			node->n_children = child->n_children;
			node->type = child->type;
			free ( child );

		} else if (parent != NULL && is_list_node(node) && is_list_node(parent) && node->type == parent->type){ //Hvis noden og parent noden begge er list-noder av samme typen
			int child_count = node->n_children + 1;
			printf("(4.2) Merged children of node (type: %s) and other child of parent into parent node of type: %s\n", node_string[node->type], node_string[parent->type]);
			node_t** children_list = (node_t **) malloc(child_count * sizeof(node_t *));
			for (int i = 0; i < node->n_children; i++)  {
				children_list[i] = node->children[i];
			}
			children_list[child_count - 1] = parent->children[1];	//Leg til det andre barnet til parent i slutten av children_list
			parent->children = children_list;			//Og sett children_list som nye children av parent
			parent->n_children = child_count;			//Oppdater antall barn parent har
			free ( node );
		}
	}
}

void
print_list_to_print_statement(node_t *node)
{
	if (node->type == PRINT_LIST) {
		node->type = PRINT_STATEMENT;
		printf("Changed PRINT_LIST to PRINT_STATEMENT\n");
	}
}



void
simplify_tree ( node_t *node, node_t *parent)
{
	if (!node) {
		return;
	}
	simplify_node( node );
	for (int i = 0; i < node->n_children; i++) {
		simplify_tree(node->children[i], node);
	}
	simplify_list(node, parent);
	print_list_to_print_statement(node);
}
