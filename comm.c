#include "comm.h"
#include "graph.h"
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

static unsigned int udp_port_number = 40000;
static char recv_buffer[MAX_PACKET_BUFFER_SIZE];
static char send_buffer[MAX_PACKET_BUFFER_SIZE];

static int _send_pkt_out(int sock_fd, char *pkt_data, unsigned int pkt_size, unsigned int dst_udp_port_no){

    int rc;
    struct sockaddr_in dest_addr;
   
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dst_udp_port_no);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    rc = sendto(sock_fd, pkt_data, pkt_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    return rc;
}

static void _pkt_receive(node_t *receving_node, char *pkt_with_aux_data, unsigned int pkt_size){

    char *recv_intf_name = pkt_with_aux_data;
    interface_t *recv_intf = get_node_if_by_name(receving_node, recv_intf_name);

    if(!recv_intf){
        printf("Error : Pkt recvd on unknown interface %s on node %s\n", recv_intf->if_name, receving_node->node_name);
        return;
    }

    pkt_receive(receving_node, recv_intf, pkt_with_aux_data + IF_NAME_SIZE, pkt_size - IF_NAME_SIZE);
}

static void *_network_start_pkt_receiver_thread(void *arg){

    node_t *node;
    glthread_t *curr;
    
    fd_set active_sock_fd_set,
           backup_sock_fd_set;
    
    int sock_max_fd = 0;
    int bytes_recvd = 0;
    
    graph_t *topo = (void *)arg;

    FD_ZERO(&active_sock_fd_set);
    FD_ZERO(&backup_sock_fd_set);
    
    struct sockaddr_in sender_addr;
    int sender_addr_len = sizeof(sender_addr);

    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){

        node = graph_glue_to_node(curr);
        
        if(!node->udp_sock_fd) 
            continue;

        if(node->udp_sock_fd > sock_max_fd)
            sock_max_fd = node->udp_sock_fd;

        FD_SET(node->udp_sock_fd, &backup_sock_fd_set);
            
    } ITERATE_GLTHREAD_END(&topo->node_list, curr);

    while(1){

        memcpy(&active_sock_fd_set, &backup_sock_fd_set, sizeof(backup_sock_fd_set));

        select(sock_max_fd + 1, &active_sock_fd_set, NULL, NULL, NULL);

        ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){

            node = graph_glue_to_node(curr);

            if(FD_ISSET(node->udp_sock_fd, &active_sock_fd_set)){
                memset(recv_buffer, 0, MAX_PACKET_BUFFER_SIZE);
                bytes_recvd = recvfrom(node->udp_sock_fd, (char *)recv_buffer, MAX_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&sender_addr, &sender_addr_len);
                _pkt_receive(node, recv_buffer, bytes_recvd);
            }
            
        } ITERATE_GLTHREAD_END(&topo->node_list, curr);
    }
}

int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface){

    int rc = 0;

    node_t *sending_node = interface->att_node;
    node_t *nbr_node = get_nbr_node(interface);
    
    if(!nbr_node)
        return -1;

    unsigned int dst_udp_port_no = nbr_node->udp_port_number;
    
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sock < 0){
        printf("Error : Sending socket Creation failed , errno = %d", errno);
        return -1;
    }
    
    interface_t *other_interface = &interface->link->intf1 == interface ? \
                                    &interface->link->intf2 : &interface->link->intf1;

    memset(send_buffer, 0, MAX_PACKET_BUFFER_SIZE);

    char *pkt_with_aux_data = send_buffer;

    strncpy(pkt_with_aux_data, other_interface->if_name, IF_NAME_SIZE);

    pkt_with_aux_data[IF_NAME_SIZE] = '\0';

    memcpy(pkt_with_aux_data + IF_NAME_SIZE, pkt, pkt_size);

    rc = _send_pkt_out(sock, pkt_with_aux_data, IF_NAME_SIZE + pkt_size, dst_udp_port_no);

    close(sock);
    return rc;
}

int pkt_receive(node_t *node, interface_t *interface, char *pkt, unsigned int pkt_size){

    /*Do further processing of the pkt here*/
    printf("Msg Recvd : Rcv Node %s, Intf : %s, data recvd : %s, pkt size : %u\n", node->node_name, interface->if_name, pkt, pkt_size);

    return 0;
}

void network_start_pkt_receiver_thread(graph_t *topo){

    pthread_attr_t attr;
    pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&recv_pkt_thread, &attr, _network_start_pkt_receiver_thread, (void *)topo);
}

static unsigned int get_next_udp_port_number(){
    return udp_port_number++;
}

void init_udp_socket(node_t *node){

    if(node->udp_port_number)
        return;
    
    node->udp_port_number = get_next_udp_port_number();
     
    int udp_sock_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(udp_sock_fd == -1){
        printf("Socket Creation Failed for node %s\n", node->node_name);
        return;   
    }

    struct sockaddr_in node_addr;
    node_addr.sin_family      = AF_INET;
    node_addr.sin_port        = htons(node->udp_port_number);
    node_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udp_sock_fd, (struct sockaddr *)&node_addr, sizeof(node_addr)) == -1) {
        printf("Error : socket bind failed for Node %s\n", node->node_name);
        return;
    }

    node->udp_sock_fd = udp_sock_fd;
}



