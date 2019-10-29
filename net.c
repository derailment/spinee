#include "net.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void interface_assign_mac_address(interface_t *interface){
    
    memset(IF_MAC(interface), 0, MAC_SIZE);
    for(int i = 0; i < 6; i++) {
        IF_MAC(interface)[i] = rand() % 256;
    }
}


bool_t node_set_device_type(node_t *node, unsigned int flag) {

    SET_BIT(node->node_nw_prop.flags, flag);
    
    return TRUE;
}

bool_t node_set_loopback_address(node_t *node, char *ip_addr) {

    if (!IS_BIT_SET(node->node_nw_prop.flags, L3_ROUTER)) return FALSE;

    node->node_nw_prop.is_lb_addr_config = TRUE;
    strncpy(NODE_LO_ADDR(node), ip_addr, IP_ADDR_SIZE);
    NODE_LO_ADDR(node)[IP_ADDR_SIZE] = '\0';
    node->node_nw_prop.is_lb_addr_config = TRUE;
    
    return TRUE;
}

bool_t node_set_intf_ip_address(node_t *node, char *local_if, char *ip_addr, char mask) {
    
    interface_t *interface = get_node_if_by_name(node, local_if);
    strncpy(IF_IP(interface), ip_addr, IP_ADDR_SIZE);
    IF_IP(interface)[IP_ADDR_SIZE] = '\0';
    interface->intf_nw_prop.mask = mask; 
    interface->intf_nw_prop.is_ip_add_config = TRUE;
    return TRUE;

}

void dump_nw_graph(graph_t *graph){

    node_t *node;
    glthread_t *curr;
    interface_t *interface;
    unsigned int i;
    
    printf("Topology Name = %s\n", graph->topo_name);

    ITERATE_GLTHREAD_BEGIN(&graph->node_list, curr){

        node = graph_glue_to_node(curr);
        dump_nw_node(node);
        for( i = 0; i < MAX_INTF_PER_NODE; i++){
            interface = node->intf[i];
            if(!interface) break;
            dump_nw_intf(interface);
        }
    } ITERATE_GLTHREAD_END(&graph->node_list, curr);

}

void dump_nw_node(node_t *node){

    printf("\nNode Name = %s\n", node->node_name);
    printf("\t node flags : %u", node->node_nw_prop.flags);
    if(node->node_nw_prop.is_lb_addr_config){
        printf("\t  lo addr : %s/32\n", NODE_LO_ADDR(node));
    }
}

void dump_nw_intf(interface_t *interface){

    dump_interface(interface);

    if(interface->intf_nw_prop.is_ip_add_config){
        printf("\t IP Addr = %s/%u", IF_IP(interface), interface->intf_nw_prop.mask);
    }
    else{
         printf("\t IP Addr = %s/%u", "Nil", 0);
    }

    printf("\t MAC : %02x:%02x:%02x:%02x:%02x:%02x\n", 
        IF_MAC(interface)[0],
        IF_MAC(interface)[1],
        IF_MAC(interface)[2],
        IF_MAC(interface)[3],
        IF_MAC(interface)[4],
        IF_MAC(interface)[5]
    );

}

