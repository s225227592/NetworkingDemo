#include "splashkit.h"

using std::to_string;

int main()
{
    write_line("Server file test");

    server_socket serverTest;
    const connection_type protocol = UDP;
    const unsigned short portNum = 5432;
    const string serverName = "testing server";


    serverTest = create_server(serverName, portNum, protocol);

    while(!has_messages(serverName))
    {
        write_line("Waiting for messages...");
        delay(1000);
    }

    string receivedMessage = read_message_data(serverName);
    write_line("Received message: " + receivedMessage);

    close_server(serverName);
}