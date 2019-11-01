#ifndef __NET__
#define __NET__

#define L3_ROUTER (1 << 0)
#define L2_SWITCH (1 << 1)
#define IP_ADDR_SIZE 16
#define MAC_SIZE 48
#define IF_MAC(intf_ptr) (intf_ptr->intf_nw_prop.mac_add.mac)
#define IF_IP(intf_ptr) (intf_ptr->intf_nw_prop.ip_add.ip_addr)
#define NODE_LO_ADDR(node_ptr) (node_ptr->node_nw_prop.lb_addr.ip_addr)

#include "utils.h"
#include <memory.h>

typedef struct graph_ graph_t;
typedef struct node_ node_t;
typedef struct interface_ interface_t;

typedef struct intf_nw_prop_ intf_nw_prop_t;
typedef struct node_nw_prop_ node_nw_prop_t;
typedef struct ip_add_ ip_add_t;
typedef struct mac_add_ mac_add_t;

struct ip_add_ {
    char ip_addr[IP_ADDR_SIZE];
};

struct mac_add_ {
    unsigned char mac[MAC_SIZE];
};

struct intf_nw_prop_ {
    mac_add_t mac_add;
    bool_t is_ip_add_config;
    ip_add_t ip_add;
    char mask;
};

struct node_nw_prop_ {
    unsigned int flags;
    bool_t is_lb_addr_config;
    ip_add_t lb_addr;
};

static inline void init_node_nw_prop(node_nw_prop_t *node_nw_prop) {
    node_nw_prop->flags = 0;
    node_nw_prop->is_lb_addr_config = FALSE;
    memset(node_nw_prop->lb_addr.ip_addr, 0, IP_ADDR_SIZE);
}

static inline void init_intf_nw_prop(intf_nw_prop_t *intf_nw_prop) {
    memset(intf_nw_prop->mac_add.mac , 0 , MAC_SIZE);
    intf_nw_prop->is_ip_add_config = FALSE;
    memset(intf_nw_prop->ip_add.ip_addr, 0, IP_ADDR_SIZE);
    intf_nw_prop->mask = 0;
}

bool_t node_set_device_type(node_t *node, unsigned int F);
bool_t node_set_loopback_address(node_t *node, char *ip_addr);
bool_t node_set_intf_ip_address(node_t *node, char *local_if, char *ip_addr, char mask);
void interface_assign_mac_address(interface_t *interface);
void dump_nw_graph(graph_t *graph);
void dump_nw_node(node_t *node);
void dump_nw_intf(interface_t *interface);

#endif