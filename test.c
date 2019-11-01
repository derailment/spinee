#include "graph.h"
#include "CommandParser/libcli.h"
#include <unistd.h>

extern graph_t *build_first_topo();
extern void nw_init_cli();
extern int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);

graph_t *topo = NULL;

int main(int argc, char **argv){
    
    // Initialize commands
    nw_init_cli();
    
    // Build network topology
    topo = build_first_topo();

    // Give some time for packet receiver thread to start
    sleep(2);
    
    char msg[] = "Hello world!";
    node_t *node = get_node_by_node_name(topo, "R0_re");
    interface_t *oif = get_node_if_by_name(node, "eth0/0");
    send_pkt_out(msg, sizeof(msg), oif);
    scanf("\n");

    dump_nw_graph(topo);
    start_shell();

    return 0;
}
