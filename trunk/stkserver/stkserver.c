/* 
 * File: stkserver.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

stk_client *clients;

int main(int argc, char argv[])
{
    int server_fd;
    int conn_fd;
    int bytes;
    //int conn_fd[STK_MAX_CLIENTS];
    char buf[STK_MAX_PACKET_SIZE];
    int data_len;
    unsigned short cmd;

    if ((server_fd = stk_server_socket()) == -1){
        printf("create stkserver socket error!exiting....\n");
        exit(0);
    }

    printf("==========================================\n");
    printf("========= waiting for stk client  ========\n");
    printf("==========================================\n");

    while(1){
        if((conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }

        memset(buf, 0, sizeof(buf));
        bytes = recv(conn_fd, buf, STK_MAX_PACKET_SIZE, 0);
        if (bytes == -1){
            printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        if (bytes > STK_MAX_PACKET_SIZE){
            printf("receive packet is too large, drop it.");
            continue;
        }

        data_len = stk_parse_packet(buf, bytes);
        if (data_len < 0){
            printf("bad stkp message, continue.");
            continue;
        }

        switch (state) {
        case STKP_CMD_REQ_LOGIN:
            stk_reqlogin_ack();
            break;
        case STKP_CMD_LOGIN:
            stk_login_ack();
            break;
        case STKP_CMD_KEEPALIVE:
            stk_keepalive_ack();
            break;
        case STKP_CMD_LOGOUT:
            /* do something */
            break;
        case STKP_CMD_GET_USER:
            stk_getuser_ack();
            break;
        case STKP_CMD_GET_ONLINE_USER:
            stk_getonlineuser_ack();
            break;
        case STKP_CMD_USER_INFO:
            stk_getinfo_ack();
            break;
        case STKP_CMD_SEND_MSG:
            stk_sendmsg_ack();
            break;
        default:
            printf("unknow stkp cmd, drop it.")
        }

    }

    close(listenfd);

    return 0;
}
