#include "graph.h"
#include "CommandParser/libcli.h"
#include <unistd.h>
#include <stdio.h>

extern graph_t *build_first_topo();
extern void nw_init_cli();
extern int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);

graph_t *topo = NULL;

int main(int argc, char **argv){
    
    // Initialize commands
    nw_init_cli();
    
    // Build network topology
    topo = build_first_topo();
    printf("Start console...\n");
    
    // Give some time for packet receiver thread to start
    sleep(2);

    // Start simulated switch/router console
    start_shell();

    return 0;
}
