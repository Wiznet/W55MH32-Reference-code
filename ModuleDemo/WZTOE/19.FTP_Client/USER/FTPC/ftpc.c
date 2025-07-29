#include "ftpc.h"

un_l2cval      remote_ip;
uint16_t       remote_port;
un_l2cval      local_ip;
uint16_t       local_port;
uint8_t        connect_state_control_ftpc = 0;
uint8_t        connect_state_data_ftpc    = 0;
uint8_t        gModeActivePassiveflag     = 0;
uint16_t       FTP_destport               = 21;
uint8_t        gMenuStart                 = 0;
uint8_t        gDataSockReady             = 0;
uint8_t        gDataPutGetStart           = 0;
static uint8_t gMsgBuf[256]               = {
    0,
};
uint8_t socket_ctrl;
uint8_t socket_data;

struct ftpc    ftpc;
struct Command Command;

void ftpc_init(uint8_t *src_ip, uint8_t sn_ctrl, uint8_t sn_data)
{
    ftpc.dsock_mode = ACTIVE_MODE;

    local_ip.cVal[0] = src_ip[0];
    local_ip.cVal[1] = src_ip[1];
    local_ip.cVal[2] = src_ip[2];
    local_ip.cVal[3] = src_ip[3];
    local_port       = 35000;
    socket_ctrl      = sn_ctrl;
    socket_data      = sn_data;
    strcpy(ftpc.workingdir, "/");
    socket(socket_ctrl, Sn_MR_TCP, FTP_destport, 0x0);
}
uint8_t ftpc_run(uint8_t *dbuf)
{
    uint16_t size = 0;
    long     ret  = 0;
#if defined(F_FILESYSTEM)
    uint32_t send_byte;
#endif
    uint32_t recv_byte;
    uint32_t blocklen;
    uint32_t remain_filesize;
    uint32_t remain_datasize;
    uint8_t  dat[50] = {
        0,
    };

    switch (getSn_SR(socket_ctrl))
    {
    case SOCK_ESTABLISHED:
        if (!connect_state_control_ftpc)
        {
            printf("%d:FTP Connected\r\n", socket_ctrl);
            strcpy(ftpc.workingdir, "/");
            connect_state_control_ftpc = 1;
        }
        if (gMenuStart)
        {
            gMenuStart = 0;
            printf("\r\n----------------------------------------\r\n");
            printf("Press menu key\r\n");
            printf("----------------------------------------\r\n");
            printf("1> View FTP Server Directory\r\n");
            printf("2> View My Directory\r\n");
            printf("3> Sets the type of file to be transferred. Current state : %s\r\n", (ftpc.type == ASCII_TYPE) ? "Ascii" : "Binary");
            printf("4> Sets Data Connection. Current state : %s\r\n", (ftpc.dsock_mode == ACTIVE_MODE) ? "Active" : "Passive");
            printf("5> Put File to Server\r\n");
            printf("6> Get File from Server\r\n");
#if defined(F_FILESYSTEM)
            printf("7> Delete My File\r\n");
#endif
            printf("----------------------------------------\r\n");
            while (1)
            {
                memset(gMsgBuf, 0, sizeof(gMsgBuf));
                scanf("%s", gMsgBuf);
                if (gMsgBuf[0] == '1')
                {
                    if (ftpc.dsock_mode == PASSIVE_MODE)
                    {
                        sprintf((char *)dat, "PASV\r\n");
                        send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                        Command.First = f_dir;
                        break;
                    }
                    else
                    {
                        wiz_NetInfo gWIZNETINFO;
                        ctlnetwork(CN_GET_NETINFO, (void *)&gWIZNETINFO);
                        sprintf((char *)dat, "PORT %d,%d,%d,%d,%d,%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3], (uint8_t)(local_port >> 8), (uint8_t)(local_port & 0x00ff));
                        send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                        Command.First = f_dir;

                        gModeActivePassiveflag = 1;
                        break;
                    }
                }
                else if (gMsgBuf[0] == '5')
                {
                    if (ftpc.dsock_mode == PASSIVE_MODE)
                    {
                        sprintf((char *)dat, "PASV\r\n");
                        send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                        Command.First = f_put;
                        break;
                    }
                    else
                    {
                        wiz_NetInfo gWIZNETINFO;
                        ctlnetwork(CN_GET_NETINFO, (void *)&gWIZNETINFO);
                        sprintf((char *)dat, "PORT %d,%d,%d,%d,%d,%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3], (uint8_t)(local_port >> 8), (uint8_t)(local_port & 0x00ff));
                        send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                        Command.First = f_put;

                        gModeActivePassiveflag = 1;
                        break;
                    }
                }
                else if (gMsgBuf[0] == '6')
                {
                    if (ftpc.dsock_mode == PASSIVE_MODE)
                    {
                        sprintf((char *)dat, "PASV\r\n");
                        send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                        Command.First = f_get;
                        break;
                    }
                    else
                    {
                        wiz_NetInfo gWIZNETINFO;
                        ctlnetwork(CN_GET_NETINFO, (void *)&gWIZNETINFO);
                        sprintf((char *)dat, "PORT %d,%d,%d,%d,%d,%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3], (uint8_t)(local_port >> 8), (uint8_t)(local_port & 0x00ff));
                        send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                        Command.First = f_get;

                        gModeActivePassiveflag = 1;
                        break;
                    }
                }
                else if (gMsgBuf[0] == '2')
                {
#if defined(F_FILESYSTEM)
                    scan_files(ftpc.workingdir, dbuf, (int *)&size);
                    printf("\r\n%s\r\n", dbuf);
#else
                    if (strncmp(ftpc.workingdir, "/$Recycle.Bin", sizeof("/$Recycle.Bin")) != 0)
                        size = sprintf((char *)dbuf, "drwxr-xr-x 1 ftp ftp 0 Dec 31 2014 $Recycle.Bin\r\n-rwxr-xr-x 1 ftp ftp 512 Dec 31 2014 test.txt\r\n");
                    printf("\r\n%s\r\n", dbuf);
#endif
                    gMenuStart = 1;
                    break;
                }
                else if (gMsgBuf[0] == '3')
                {
                    printf("1> ASCII\r\n");
                    printf("2> BINARY\r\n");
                    while (1)
                    {
                        memset(gMsgBuf, 0, sizeof(gMsgBuf));
                        scanf("%s", gMsgBuf);
                        if (gMsgBuf[0] == '1')
                        {
                            sprintf((char *)dat, "TYPE %c\r\n", TransferAscii);
                            ftpc.type = ASCII_TYPE;
                            send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                            break;
                        }
                        else if (gMsgBuf[0] == '2')
                        {
                            sprintf((char *)dat, "TYPE %c\r\n", TransferBinary);
                            ftpc.type = IMAGE_TYPE;
                            send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                            break;
                        }
                        else if (gMsgBuf[0] != 0x00)
                        {
                            printf("\r\nRetry...\r\n");
                        }
                    }
                    break;
                }
                else if (gMsgBuf[0] == '4')
                {
                    printf("1> ACTIVE\r\n");
                    printf("2> PASSIVE\r\n");
                    while (1)
                    {
                        memset(gMsgBuf, 0, sizeof(gMsgBuf));
                        scanf("%s", gMsgBuf);
                        if (gMsgBuf[0] == '1')
                        {
                            ftpc.dsock_mode = ACTIVE_MODE;
                            break;
                        }
                        else if (gMsgBuf[0] == '2')
                        {
                            ftpc.dsock_mode = PASSIVE_MODE;
                            break;
                        }
                        else if (gMsgBuf[0] != 0x00)
                        {
                            printf("\r\nRetry...\r\n");
                        }
                    }
                    gMenuStart = 1;
                    break;
                }
#if defined(F_FILESYSTEM)
                else if (msg_c == '7')
                {
                    printf(">del filename?");
                    memset(gMsgBuf, 0, sizeof(gMsgBuf));
                    scanf("%s", gMsgBuf);
                    sprintf((char *)dat, "STOR %s\r\n", gMsgBuf);
                    if (f_unlink((const char *)ftpc.filename) != 0)
                    {
                        printf("\r\nCould not delete.\r\n");
                    }
                    else
                    {
                        printf("\r\nDeleted.\r\n");
                    }
                    gMenuStart = 1;
                    break;
                }
#endif
                else if (gMsgBuf[0] != 0x00)
                {
                    printf("\r\nRetry...\r\n");
                }
            }
        }
        if (gDataSockReady)
        {
            gDataSockReady = 0;
            switch (Command.First)
            {
            case f_dir:
                sprintf((char *)dat, "LIST\r\n");
                send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                break;
            case f_put:
                printf(">put file name?");
                memset(gMsgBuf, 0, sizeof(gMsgBuf));
                scanf("%s", gMsgBuf);
                sprintf((char *)dat, "STOR %s\r\n", gMsgBuf);
                send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                break;
            case f_get:
                printf(">get file name?");
                memset(gMsgBuf, 0, sizeof(gMsgBuf));
                scanf("%s", gMsgBuf);
                sprintf((char *)dat, "RETR %s\r\n", gMsgBuf);
                send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
                break;
            default:
                printf("Command.First = default\r\n");
                break;
            }
        }
        if ((size = getSn_RX_RSR(socket_ctrl)) > 0)
        { // Don't need to check SOCKERR_BUSY because it doesn't not occur.
            memset(dbuf, 0, _MAX_SS);
            if (size > _MAX_SS)
                size = _MAX_SS - 1;
            ret       = recv(socket_ctrl, dbuf, size);
            dbuf[ret] = '\0';
            if (ret != size)
            {
                if (ret == SOCK_BUSY)
                    return 0;
                if (ret < 0)
                {
                    printf("%d:recv() error:%ld\r\n", socket_ctrl, ret);
                    close(socket_ctrl);
                    return ret;
                }
            }
            printf("Rcvd Command: %s\r\n", dbuf);
            proc_ftpc((char *)dbuf, size);
        }
        break;
    case SOCK_CLOSE_WAIT:
        printf("%d:CloseWait\r\n", socket_ctrl);
        if ((ret = disconnect(socket_ctrl)) != SOCK_OK)
            return ret;
        printf("%d:Closed\r\n", socket_ctrl);
        break;
    case SOCK_CLOSED:
        printf("%d:FTPStart\r\n", socket_ctrl);
        if ((ret = socket(socket_ctrl, Sn_MR_TCP, FTP_destport, 0x0)) != socket_ctrl)
        {
            printf("%d:socket() error:%ld\r\n", socket_ctrl, ret);
            close(socket_ctrl);
            return ret;
        }
        break;
    case SOCK_INIT:
        printf("%d:Opened\r\n", socket_ctrl);
        if ((ret = connect(socket_ctrl, local_ip.cVal, FTP_destport)) != SOCK_OK)
        {
            printf("%d:Connect error\r\n", socket_ctrl);
            return ret;
        }
        connect_state_control_ftpc = 0;
        printf("%d:Connectting...\r\n", socket_ctrl);
        break;
    default:
        break;
    }
    switch (getSn_SR(socket_data))
    {
    case SOCK_ESTABLISHED:
        if (!connect_state_data_ftpc)
        {
            printf("%d:FTP Data socket Connected\r\n", socket_data);
            connect_state_data_ftpc = 1;
        }
        if (gDataPutGetStart)
        {
            switch (Command.Second)
            {
            case s_dir:
                printf("dir waiting...\r\n");
                if ((size = getSn_RX_RSR(socket_data)) > 0)
                { // Don't need to check SOCKERR_BUSY because it doesn't not occur.
                    printf("ok\r\n");
                    memset(dbuf, 0, _MAX_SS);
                    if (size > _MAX_SS)
                        size = _MAX_SS - 1;
                    ret       = recv(socket_data, dbuf, size);
                    dbuf[ret] = '\0';
                    if (ret != size)
                    {
                        if (ret == SOCK_BUSY)
                            return 0;
                        if (ret < 0)
                        {
                            printf("%d:recv() error:%ld\r\n", socket_ctrl, ret);
                            close(socket_data);
                            return ret;
                        }
                    }
                    printf("Rcvd Data:\n\r%s\n\r", dbuf);
                    gDataPutGetStart = 0;
                    Command.Second   = s_nocmd;
                }
                break;
            case s_put:
                printf("put waiting...\r\n");
                if (strlen(ftpc.workingdir) == 1)
                    sprintf(ftpc.filename, "/%s", (uint8_t *)gMsgBuf);
                else
                    sprintf(ftpc.filename, "%s/%s", ftpc.workingdir, (uint8_t *)gMsgBuf);
#if defined(F_FILESYSTEM)
                ftpc.fr = f_open(&(ftpc.fil), (const char *)ftpc.filename, FA_READ);
                if (ftpc.fr == FR_OK)
                {
                    remain_filesize = ftpc.fil.fsize;
                    printf("f_open return FR_OK\r\n");
                    do
                    {
                        memset(dbuf, 0, _MAX_SS);
                        if (remain_filesize > _MAX_SS)
                            send_byte = _MAX_SS;
                        else
                            send_byte = remain_filesize;
                        ftpc.fr = f_read(&(ftpc.fil), (void *)dbuf, send_byte, (UINT *)&blocklen);
                        if (ftpc.fr != FR_OK)
                        {
                            break;
                        }
                        printf("#");
                        send(socket_data, dbuf, blocklen);
                        remain_filesize -= blocklen;
                    } while (remain_filesize != 0);
                    printf("\r\nFile read finished\r\n");
                    ftpc.fr = f_close(&(ftpc.fil));
                }
                else
                {
                    printf("File Open Error: %d\r\n", ftpc.fr);
                    ftpc.fr = f_close(&(ftpc.fil));
                }
#else
                remain_filesize = strlen(ftpc.filename);
                do
                {
                    memset(dbuf, 0, _MAX_SS);
                    blocklen = sprintf((char *)dbuf, "%s", ftpc.filename); // Upload file content
                    printf("########## dbuf:%s\r\n", dbuf);
                    send(socket_data, dbuf, blocklen);
                    remain_filesize -= blocklen;
                } while (remain_filesize != 0);
#endif
                gDataPutGetStart = 0;
                Command.Second   = s_nocmd;
                disconnect(socket_data);
                break;
            case s_get:
                printf("get waiting...\r\n");
                if (strlen(ftpc.workingdir) == 1)
                    sprintf(ftpc.filename, "/%s", (uint8_t *)gMsgBuf);
                else
                    sprintf(ftpc.filename, "%s/%s", ftpc.workingdir, (uint8_t *)gMsgBuf);
#if defined(F_FILESYSTEM)
                ftpc.fr = f_open(&(ftpc.fil), (const char *)ftpc.filename, FA_CREATE_ALWAYS | FA_WRITE);
                if (ftpc.fr == FR_OK)
                {
                    printf("f_open return FR_OK\r\n");
                    while (1)
                    {
                        if ((remain_datasize = getSn_RX_RSR(socket_data)) > 0)
                        {
                            while (1)
                            {
                                memset(dbuf, 0, _MAX_SS);
                                if (remain_datasize > _MAX_SS)
                                    recv_byte = _MAX_SS;
                                else
                                    recv_byte = remain_datasize;
                                ret              = recv(socket_data, dbuf, recv_byte);
                                ftpc.fr          = f_write(&(ftpc.fil), (const void *)dbuf, (UINT)ret, (UINT *)&blocklen);
                                remain_datasize -= blocklen;
                                if (ftpc.fr != FR_OK)
                                {
                                    printf("f_write failed\r\n");
                                    break;
                                }
                                if (remain_datasize <= 0)
                                    break;
                            }
                            if (ftpc.fr != FR_OK)
                            {
                                printf("f_write failed\r\n");
                                break;
                            }
                            printf("#");
                        }
                        else
                        {
                            if (getSn_SR(socket_data) != SOCK_ESTABLISHED)
                                break;
                        }
                    }
                    printf("\r\nFile write finished\r\n");
                    ftpc.fr          = f_close(&(ftpc.fil));
                    gDataPutGetStart = 0;
                }
                else
                {
                    printf("File Open Error: %d\r\n", ftpc.fr);
                }
#else
                while (1)
                {
                    if ((remain_datasize = getSn_RX_RSR(socket_data)) > 0)
                    {
                        while (1)
                        {
                            memset(dbuf, 0, _MAX_SS);
                            if (remain_datasize > _MAX_SS)
                                recv_byte = _MAX_SS;
                            else
                                recv_byte = remain_datasize;
                            ret = recv(socket_data, dbuf, recv_byte);
                            printf("########## dbuf:%s\r\n", dbuf);
                            remain_datasize -= ret;
                            if (remain_datasize <= 0)
                                break;
                        }
                    }
                    else
                    {
                        if (getSn_SR(socket_data) != SOCK_ESTABLISHED)
                            break;
                    }
                }
                gDataPutGetStart = 0;
                Command.Second   = s_nocmd;
#endif
                break;
            default:
                printf("Command.Second = default\r\n");
                break;
            }
        }
        break;
    case SOCK_CLOSE_WAIT:
        printf("%d:CloseWait\r\n", socket_data);
        if ((size = getSn_RX_RSR(socket_data)) > 0)
        { // Don't need to check SOCKERR_BUSY because it doesn't not occur.
            ret       = recv(socket_data, dbuf, size);
            dbuf[ret] = '\0';
            if (ret != size)
            {
                if (ret == SOCK_BUSY)
                    return 0;
                if (ret < 0)
                {
                    printf("%d:recv() error:%ld\r\n", socket_ctrl, ret);
                    close(socket_data);
                    return ret;
                }
            }
            printf("Rcvd Data:\n\r%s\n\r", dbuf);
        }
        if ((ret = disconnect(socket_data)) != SOCK_OK)
            return ret;
        printf("%d:Closed\r\n", socket_data);
        break;
    case SOCK_CLOSED:
        if (ftpc.dsock_state == DATASOCK_READY)
        {
            if (ftpc.dsock_mode == PASSIVE_MODE)
            {
                printf("%d:FTPDataStart, port : %d\r\n", socket_data, local_port);
                if ((ret = socket(socket_data, Sn_MR_TCP, local_port, 0x0)) != socket_data)
                {
                    printf("%d:socket() error:%ld\r\n", socket_data, ret);
                    close(socket_data);
                    return ret;
                }
                local_port++;
                if (local_port > 50000)
                    local_port = 35000;
            }
            else
            {
                printf("%d:FTPDataStart, port : %d\r\n", socket_data, local_port);
                if ((ret = socket(socket_data, Sn_MR_TCP, local_port, 0x0)) != socket_data)
                {
                    printf("%d:socket() error:%ld\r\n", socket_data, ret);
                    close(socket_data);
                    return ret;
                }
                local_port++;
                if (local_port > 50000)
                    local_port = 35000;
            }
            ftpc.dsock_state = DATASOCK_START;
        }
        break;

    case SOCK_INIT:
        printf("%d:Opened\r\n", socket_data);
        if (ftpc.dsock_mode == ACTIVE_MODE)
        {
            if ((ret = listen(socket_data)) != SOCK_OK)
            {
                printf("%d:Listen error\r\n", socket_data);
                return ret;
            }
            gDataSockReady = 1;
            printf("%d:Listen ok\r\n", socket_data);
        }
        else
        {
            if ((ret = connect(socket_data, remote_ip.cVal, remote_port)) != SOCK_OK)
            {
                printf("%d:Connect error\r\n", socket_data);
                return ret;
            }
            gDataSockReady = 1;
        }
        connect_state_data_ftpc = 0;
        break;
    default:
        break;
    }
    return 0;
}

char proc_ftpc(char *buf, uint16_t len)
{
    uint16_t Responses[3] = {0};
    uint16_t i            = 0;
    uint8_t  j            = 0;
    uint8_t  k;
    uint8_t  dat[30] = {
        0,
    };
    uint8_t buff[30];
    while (i < len)
    {
        if ((buf[i] >= '0') & (buf[i] <= '9'))
        {
            if (((buf[i] >= '0') & (buf[i] <= '9')) & ((buf[i + 1] >= '0') & (buf[i + 1] <= '9')) & ((buf[i + 2] >= '0') & (buf[i + 2] <= '9')) & (buf[i + 3] == ' '))
            {
                Responses[j] = (buf[i] - '0') * 100 + (buf[i + 1] - '0') * 10 + (buf[i + 2] - '0');
                j++;
            }
            i += 3;
        }
        else
        {
            i++;
        }
    }
    for (k = 0; k < j; k++)
    {
        switch (Responses[k])
        {
        case R_530:
            printf("\r\nLogin failed, please check your ID and Password.\r\n");
        case R_220: /* Service ready for new user. */
            printf("\r\nInput your User ID > ");
            scanf("%s", buff);
            sprintf((char *)dat, "USER %s\r\n", buff);
            send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
            break;

        case R_331: /* User name okay, need password. */
            printf("\r\nInput your Password > ");
            scanf("%s", buff);
            sprintf((char *)dat, "PASS %s\r\n", buff);
            send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
            break;
        case R_230: /* User logged in, proceed */
            printf("\r\nUser logged in, proceed\r\n");

            sprintf((char *)dat, "TYPE %c\r\n", TransferAscii);
            ftpc.type = ASCII_TYPE;
            send(socket_ctrl, (uint8_t *)dat, strlen((char *)dat));
            break;
        case R_200:
            if ((ftpc.dsock_mode == ACTIVE_MODE) && gModeActivePassiveflag)
            {
                ftpc.dsock_state       = DATASOCK_READY;
                gModeActivePassiveflag = 0;
            }
            else
            {
                gMenuStart = 1;
            }
            break;
        case R_125:
        case R_150:
            switch (Command.First)
            {
            case f_dir:
                Command.First    = f_nocmd;
                Command.Second   = s_dir;
                gDataPutGetStart = 1;
                break;
            case f_get:
                Command.First    = f_nocmd;
                Command.Second   = s_get;
                gDataPutGetStart = 1;
                break;
            case f_put:
                Command.First    = f_nocmd;
                Command.Second   = s_put;
                gDataPutGetStart = 1;
                break;
            default:
                printf("Command.First = default\r\n");
                break;
            }
            break;
        case R_226:
            gMenuStart = 1;
            break;
        case R_227:
            if (pportc(buf) == -1)
            {
                printf("Bad port syntax\r\n");
            }
            else
            {
                printf("Go Open Data Sock...\r\n ");
                ftpc.dsock_mode  = PASSIVE_MODE;
                ftpc.dsock_state = DATASOCK_READY;
            }
            break;
        case R_425:
            printf("Active mode request failed, switching to passive mode\r\n");
            ftpc.dsock_mode = PASSIVE_MODE;
            gDataSockReady  = 0;
            close(socket_data);
            gMenuStart = 1;
            break;
        default:
            printf("\r\nDefault Status = %d\r\n", (uint16_t)Responses[k]);
            gDataSockReady = 1;
            break;
        }
    }
    return 1;
}
int pportc(char *arg)
{
    int   i;
    char *tok = 0;
    strtok(arg, "(");
    for (i = 0; i < 4; i++)
    {
        if (i == 0)
            tok = strtok(NULL, ",\r\n");
        else
            tok = strtok(NULL, ",");
        remote_ip.cVal[i] = (uint8_t)atoi(tok);
        if (!tok)
        {
            printf("bad pport : %s\r\n", arg);
            return -1;
        }
    }
    remote_port = 0;
    for (i = 0; i < 2; i++)
    {
        tok           = strtok(NULL, ",\r\n");
        remote_port <<= 8;
        remote_port  += atoi(tok);
        if (!tok)
        {
            printf("bad pport : %s\r\n", arg);
            return -1;
        }
    }
    printf("ip : %d.%d.%d.%d, port : %d\r\n", remote_ip.cVal[0], remote_ip.cVal[1], remote_ip.cVal[2], remote_ip.cVal[3], remote_port);
    return 0;
}
