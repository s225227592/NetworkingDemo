#include "splashkit.h"
#include <ctime>

using std::to_string;

int main()
{
    write_line("Client file test");
    const string hostIP = "127.0.0.1";
    const connection_type protocol = UDP;
    const unsigned short portNum = 5432;
    const string serverConnectionName = "server connection";

    connection serverConnection = open_connection(serverConnectionName, hostIP, portNum, protocol);

    for(int i = 1; i < 2; i++)
    {
        string messageSent = "";
        messageSent += "X159.15|";
        messageSent += "Y123.50|";
        messageSent += "R-90.0";
        //time_t timestamp;
        //time(&timestamp);
        send_message_to(messageSent, serverConnection);
        //delay(50);
        write_line("Message sent: " + messageSent);
    }

    close_connection(serverConnection);

    return 0;
}