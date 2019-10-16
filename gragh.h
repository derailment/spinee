#ifndef __GRAPH__
#define __GRAPH__

#include "gluethread/glthread.h"

#define TOPO_NAME_SIZE    32
#define NODE_NAME_SIZE    16
#define IF_NAME_SIZE      16
#define MAX_INTF_PER_NODE 10


typedef struct gragh_ gragh_t;
typedef struct node_ node_t;
typedef struct interface_ interface_t;
typedef struct link_ link_t;

typedef struct graph_ {
    char topo_name[TOPO_NAME_SIZE];
    glthread_t node_list;
};

struct node_ {
    char node_name[NODE_NAME_SIZE];
    interface_t *intf[MAX_INTF_PER_NODE];
    glthread_t gragh_glue;
};

typedef struct interface_ {
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

#endif