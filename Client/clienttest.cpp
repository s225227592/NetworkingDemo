#include "splashkit.h"

int main()
{
    write_line("Client file test");
    const string hostIP = "127.0.0.1";
    const connection_type protocol = UDP;
    const unsigned short portNum = 5432;
    const string serverConnectionName = "server connection";

    connection serverConnection = open_connection(serverConnectionName, hostIP, portNum, protocol);

    // for(int i = 0; i < 20; i++)
    // {
        send_message_to("Hello this is client", serverConnection);
        //delay(1000);
    // }    

    close_connection(serverConnection);

    return 0;
}