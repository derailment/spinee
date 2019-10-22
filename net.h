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
    char mac[MAC_SIZE];
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

bool_t node_set_device_type(node_t *node, unsigned int F);
bool_t node_set_loopback_address(node_t *node, char *ip_addr);
bool_t node_set_intf_ip_address(node_t *node, char *local_if, char *ip_addr, char mask);
void dump_nw_graph(graph_t *graph);
void dump_nw_node(node_t *node);
void dump_nw_intf(interface_t *interface);

#endif