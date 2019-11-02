#include "graph.h"
#include <stdio.h>
#include "CommandParser/libcli.h"
#include "CommandParser/cmdtlv.h"
#include "cmdcodes.h"
#include "comm.h"

extern graph_t *topo;

// Command Handlers
static int show_nw_topology_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable){

    int CMDCODE = -1;
    CMDCODE = EXTRACT_CMD_CODE(tlv_buf);

    switch(CMDCODE){

        case CMDCODE_SHOW_NW_TOPOLOGY:
            dump_nw_graph(topo);
            break;
        default:
            ;
    }
}

static int send_udp_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable){

    int CMDCODE = -1;
    char *msg = "Hello world!";
    int counter = 5;
    node_t *node;
    interface_t *oif;
    char *node_name;
    char *intf_name;

    tlv_struct_t *tlv = NULL;
    TLV_LOOP_BEGIN(tlv_buf, tlv){
        if     (strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;
        else if(strncmp(tlv->leaf_id, "intf-name", strlen("intf-name")) ==0)
            intf_name = tlv->value;
        else
            assert(0);
    }TLV_LOOP_END;
    
    node = get_node_by_node_name(topo, node_name);
    oif = get_node_if_by_name(node, intf_name);
    if(!oif) {
        printf("%s does not exist on %s!\n", intf_name, node_name);
        return;
    }

    CMDCODE = EXTRACT_CMD_CODE(tlv_buf);
    switch(CMDCODE){
        case CMDCODE_SEND_UDP:
            do {
                send_pkt_out(msg, strlen(msg), oif);
                sleep(2);
                counter--;
            } while(counter > 0);
            printf("\nI'm tired...o<=\n");
            break;
        default:
            ;
    }
}

// Display functions when user presses "?"
void display_graph_nodes(param_t *param, ser_buff_t *tlv_buf){

    node_t *node;
    glthread_t *curr;

    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){
        node = graph_glue_to_node(curr);
        printf("%s\n", node->node_name);
    } ITERATE_GLTHREAD_END(&topo->node_list, curr);
}

void display_node_intfs(param_t *param, ser_buff_t *tlv_buf){

    node_t *node;
    char *node_name;

    tlv_struct_t *tlv = NULL;
    TLV_LOOP_BEGIN(tlv_buf, tlv){
        if     (strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;
        else
            assert(0);
    }TLV_LOOP_END;

    node = get_node_by_node_name(topo, node_name);

    for (int i = 0; i < MAX_INTF_PER_NODE; i++) {
        interface_t *intf = node->intf[i];
        if(intf == NULL) break;
        printf("%s\n", intf->if_name);
    }

}

// Validation functions
int validate_node_extistence(char *node_name){

    node_t *node = get_node_by_node_name(topo, node_name);
    if(node)
        return VALIDATION_SUCCESS;
    printf("Error : Node %s do not exist\n", node_name);
    return VALIDATION_FAILED;
}

void nw_init_cli(){

    init_libcli();

    param_t *show   = libcli_get_show_hook();
    param_t *debug  = libcli_get_debug_hook();
    param_t *config = libcli_get_config_hook();
    param_t *run    = libcli_get_run_hook();
    param_t *debug_show = libcli_get_debug_show_hook();
    param_t *root = libcli_get_root();

    {
        /*show topology*/
         static param_t topology;
         init_param(&topology, CMD, "topology", show_nw_topology_handler, 0, INVALID, 0, "Dump Complete Network Topology");
         libcli_register_param(show, &topology);
         set_param_cmd_code(&topology, CMDCODE_SHOW_NW_TOPOLOGY);   
    }

    {
        /*run node*/
        static param_t node;
        init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
        libcli_register_param(run, &node);
        libcli_register_display_callback(&node, display_graph_nodes);
        {
            /*run node <node-name>*/
            static param_t node_name;
            init_param(&node_name, LEAF, 0, 0, validate_node_extistence, STRING, "node-name", "Node Name");
            libcli_register_param(&node, &node_name);
            {
                /*run node <node-name> intf*/
                static param_t intf;
                init_param(&intf, CMD, "intf" , 0, 0, INVALID, 0, "\"intf\" keyword");
                libcli_register_param(&node_name, &intf);
                libcli_register_display_callback(&intf, display_node_intfs);
                {
                    /*run node <node-name> intf <intf-name>*/    
                    static param_t intf_name;
                    init_param(&intf_name, LEAF, 0, 0, 0, STRING, "intf-name", "Interface Name");
                    libcli_register_param(&intf, &intf_name);
                    {
                        /*run node <node-name> intf <intf-name> send_udp*/
                        static param_t send_udp;
                        init_param(&send_udp, CMD, "send_udp" , send_udp_handler, 0, INVALID, 0, "\"send_udp\" keyword");
                        libcli_register_param(&intf_name, &send_udp);
                        set_param_cmd_code(&send_udp, CMDCODE_SEND_UDP);
                    }
                }
            }
        }
    }

    support_cmd_negation(config);
    /*Do not add any param here*/
}
