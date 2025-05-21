#include "splashkit.h"
#include <ctime>

using std::to_string;

int main()
{
    write_line("Server file test");

    // server_socket serverTest;
    // const connection_type protocol = UDP;
    // const unsigned short portNum = 5432;
    // const string serverName = "testing server";

    // serverTest = create_server(serverName, portNum, protocol);

    // while(!has_messages(serverName))
    // {
    //     check_network_activity();
    //     write_line("Waiting for messages...");
    //     delay(50);
    // }
    // check_network_activity();

    // string receivedMessage = "";
    // while(has_messages())
    // {
    //     receivedMessage = read_message_data(serverTest);
    //     time_t timestamp;
    //     time(&timestamp);
    //     write_line("Received message: " + receivedMessage + " | rcvd and processed at: " + ctime(&timestamp));
    //     //delay(100);
    //     check_network_activity();
    // }

    string receivedMessage = "X159.15|Y123.50|R-90.0|";

    double x = 0.0;
    double y = 0.0;
    double angle = 0.0; 
    string currVal = "E"; // E for empty
    string toConvert = "";

    for(char a: receivedMessage)
    {
        string temp = "";
        temp += a;

        if(currVal == "E")
        {
            currVal = a;
            write("Setting currVal to: ");
            write_line(temp);
            continue;
        }
        else if(is_double(temp) || a == '.' | a == '-')
        {
            toConvert += a;
            write_line("toConvert: " + toConvert);
        }
        else if(a == '|')
        {
            write("Break met currVal is: ");
            write_line(currVal);
            if(currVal == "X")
                x = std::stod(toConvert);
            else if(currVal == "Y")
                y = std::stod(toConvert);
            else if(currVal == "R")
                angle = std::stod(toConvert);
            else
                write_line("Error with assigning variable: ");

            write_line("Setting " + currVal + " to " + toConvert);
            toConvert = "";
            currVal = "E";
        }
        else
        {
            write("False not adding: ");
            write_line(a);
        }
    }

    write_line("X: " + to_string(x));
    write_line("Y: " + to_string(y));
    write_line("R: " + to_string(angle));

    //close_server(serverName);
}