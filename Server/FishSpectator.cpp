#include "splashkit.h"
#include <ctime>

using std::to_string;

const string PLAYER_BITMAP = "PlayerBMP";
const string playerFiles[] = {"PlayerFishMid.png", "PlayerFishLeft.png", "PlayerFishRight.png"};

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const int FRAME_RATE = 60;

enum turn_enum
{
    none,
    left,
    right
};

struct fish
{
    double x;
    double y;
    double angle;
    float size;
    bitmap bmp;
    turn_enum turn;

    void setVars(string received)
    {
        string currVal = "E"; // E for empty
        string toConvert = "";

        for(char a: received)
        {
            string temp = "";
            temp += a;

            if(currVal == "E")
            {
                currVal = a;
                continue;
            }
            else if(is_double(temp) || a == '.' | a == '-')
            {
                toConvert += a;
            }
            else if(a == '|')
            {
                if(currVal == "X")
                    x = std::stod(toConvert);
                else if(currVal == "Y")
                    y = std::stod(toConvert);
                else if(currVal == "R")
                    angle = std::stod(toConvert);
                else if(currVal == "S")
                    size = std::stof(toConvert);
                else if(currVal == "T")
                {
                    switch (std::stoi(toConvert))
                    {
                    case 0:
                        turn = none;
                        break;
                    case 1:
                        turn = left;
                        break;
                    case 2:
                        turn = right;
                        break;
                    default:
                        turn = none;
                        break;
                    }
                }
                else
                    write_line("Error with assigning variable: ");

                toConvert = "";
                currVal = "E";
            }
            else
            {
                write_line("False not adding: ");
            }
        }
        // write_line("X: " + to_string(x));
        // write_line("Y: " + to_string(y));
        // write_line("R: " + to_string(angle));
        // write_line("S: " + to_string(size));
        // write_line("T: " + to_string(turn));
    }

    void draw()
    {
        drawing_options drawOptions = option_defaults();

        //fill_circle(color_blue_violet(), pos.x, pos.y, (24 * size) - 4); //Used in testing for checking player/npc hitbox circles
        bmp = load_bitmap(PLAYER_BITMAP, playerFiles[turn]); // Uses current turn status to load correct png

        // Rotates the image based on the direction fish is currently facing
        drawOptions = option_rotate_bmp(angle);

        // Scales x and y by current fish size
        drawOptions.scale_x = size;
        drawOptions.scale_y = size;

        // Draw the bitmap to screen, modifies position to account for the image dimensions
        // Was unable to work out a better way other than modifying where image is drawn for better collission detection
        draw_bitmap(bmp, x - (1 * 13), y - (1 * 24), drawOptions);
    }
};

int main()
{
    server_socket serverTest;
    const connection_type protocol = UDP;
    const unsigned short portNum = 5432;
    const string serverName = "testing server";

    serverTest = create_server(serverName, portNum, protocol);

    while(!has_messages(serverTest))
    {
        write_line("Waiting for connection...");
        delay(200);
        check_network_activity();
    }

    write_line("Game connected...loading game");
    double x = 0.0;
    double y = 0.0;
    double angle = 0.0; 
    string receivedMessage = "";
    open_window("Server BigFish", SCREEN_WIDTH, SCREEN_HEIGHT);
    fish networkFish;

    //while(has_messages(serverTest))
    while(!quit_requested())
    {
        process_events();
        if(has_messages(serverTest))
        {
            receivedMessage = read_message_data(serverTest);
            networkFish.setVars(receivedMessage);
        }

        clear_screen(color_cornflower_blue());
        
        networkFish.draw();
        free_all_bitmaps();
        refresh_screen(FRAME_RATE); // Set refresh rate to default game FRAME_RATE
        if(key_down(ESCAPE_KEY))
        {
            break;
        }
        check_network_activity();


    }

    close_server(serverName);
}