#ifndef __GRAPH__
#define __GRAPH_

#include "gluethread/glthread.h"

#define TOPO_NAME_SIZE    32
#define NODE_NAME_SIZE    16
#define IF_NAME_SIZE      16
#define MAX_INTF_PER_NODE 10

typedef struct graph_ graph_t;
typedef struct node_ node_t;
typedef struct interface_ interface_t;
typedef struct link_ link_t;

struct graph_ {
    char topo_name[TOPO_NAME_SIZE];
    glthread_t node_list;
};

struct node_ {
    char node_name[NODE_NAME_SIZE];
    interface_t *intf[MAX_INTF_PER_NODE];
    glthread_t graph_glue;
};

struct interface_ {
    char if_name[IF_NAME_SIZE];
    struct link_ *link;
    struct node_ *att_node;
};

struct link_ {
    interface_t intf1;
    interface_t intf2;
    unsigned int cost;
};

static inline node_t *get_nbr_node(interface_t *interface) {
    link_t *link = interface->link;
    if (&link->intf1 != interface) {
        return (&link->intf1)->att_node;
    } else {
        return (&link->intf2)->att_node;
    }
}

static inline int get_node_intf_available_slot(node_t *node){
    for (int i = 0; i < MAX_INTF_PER_NODE; i++) {
        if (node->intf[i]) {
            continue;
        }
        return i;
    }
    return -1;
}

GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue);

void insert_link_between_two_nodes(node_t *node1, node_t *node2, char *from_if_name, char *to_if_name, unsigned int cost);
graph_t *create_new_graph(char *topo_name);
node_t *create_graph_node(graph_t *graph, char *node_name);
void dump_graph(graph_t *graph);
void dump_node(node_t *node);
void dump_interface(interface_t *interface);

#endif