/**
 * Matthew Geri - 225227592
 * BigFish networking demonstration SIT102 HD project
 * Spectator program file
 * Last updated 23/05/2025
 */


// Using splashkit for GUI
#include "splashkit.h"

// Using to_string for some debugging purposes, can be removed when finalised
using std::to_string;

// Constant strings for the game timer and bitmap names
const string PLAYER_BITMAP = "PlayerBMP";
// Array containing names of files for the fish images
const string playerFiles[] = {"PlayerFishMid.png", "PlayerFishLeft.png", "PlayerFishRight.png"};

// Constanst values for window size and refresh rate info
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const int FRAME_RATE = 30;

// Enum to determine if a fish is currently turning left/right or swimming straight
enum turn_enum
{
    none,
    left,
    right
};

// Fish struct used for player
struct fish
{
    double x; // X position
    double y; // Y position
    double angle; // Angle fish is currently facing
    float size; // Size of the fish
    bitmap bmp; // Bitmap for loading image
    turn_enum turn; // Current turn status of left/right/none

    /**
     * Extracts data from the received network message and sets fish values accordingly
     * 
     * @param received The message received over the network containing all data
     */
    void setVars(string received)
    {
        // Default label value E for empty, for when a new value label needs to be set
        string currVal = "E"; // E for empty
        // String to convert to appropriate number data type, each character will be added until |
        string toConvert = "";

        // Checks message string character by character to determine how to process
        for(char a: received)
        {
            // Temporary string for checking character info
            string temp = "";
            temp += a;

            // If current label is E
            if(currVal == "E")
            {
                // Set label as current character which should be X, Y, R, T, S etc
                currVal = a;
                // Move to next char in for each loop
                continue;
            }
            // If current character is a number character, . or -, then it is added to convertable string
            else if(is_double(temp) || a == '.' || a == '-')
            {
                toConvert += a;
            }
            // If character is | this signifies end of value,
            // so value should be converted to appropriate data type and assigned to fish value based on label
            else if(a == '|')
            {
                
                if(currVal == "X") // If label is X, assign to fish's x position
                    x = std::stod(toConvert);
                else if(currVal == "Y") // If label is Y, assign to fish's y position
                    y = std::stod(toConvert);
                else if(currVal == "R") // If label is R, assign to fish's angle of rotation
                    angle = std::stod(toConvert);
                else if(currVal == "S") // If label is S, assign to fish's size value
                    size = std::stof(toConvert);
                else if(currVal == "T") // If label is T, set turn status accordingly
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
                // Once value is assigned, reset convert string as empty
                toConvert = "";
                // And reset label to empty so next character will be the new value label
                currVal = "E";
            }
            else
            {
                // Else statement in case of errors
                write_line("False not adding: ");
            }
        }
    }

    // Draws fish to screen based on current set values
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
        // Was unable to work out a better way other than modifying where image is drawn for better collision detection
        draw_bitmap(bmp, x - (1 * 13), y - (1 * 24), drawOptions);
    }
};

int main()
{
    // Create a server with default info
    server_socket Spectator;
    // Server protocol will use UDP
    const connection_type protocol = UDP;
    // Receiving port will be 5432, but can be changed to any available port on device
    const unsigned short portNum = 5432;
    // String representation for the server
    const string serverName = "Spectator";

    // Create the server using selected values
    Spectator = create_server(serverName, portNum, protocol);

    // Wait for the server to receive the first message
    while(!has_messages(Spectator))
    {
        write_line("Waiting for connection...");
        delay(200);
        // After delay check again for new messages
        check_network_activity();
    }

    // Message has been received so begin game setup
    write_line("Game connected...loading game");

    // Start with an empty string in case of message errors
    string receivedMessage = "";
    // Open the window based on global size values
    open_window("Spectator BigFish", SCREEN_WIDTH, SCREEN_HEIGHT);
    // Create a fish with default values.
    fish networkFish;

    // Keep looping until player closes the window, or player presses esc key
    while(!quit_requested())
    {
        // Check for player input
        process_events();

        // If there is currently a message waiting to be processed
        if(has_messages(Spectator))
        {
            // Get the message and assign to the message string
            receivedMessage = read_message_data(Spectator);
            // Pass the message string to extract data and assign to fish's fields
            networkFish.setVars(receivedMessage);
        }

        // Reset screen with blue background
        clear_screen(color_cornflower_blue());
        
        // Draw the fish to the screen.
        networkFish.draw();
        // Clear bitmap from memory so new images can be used (probably a better way to do this)
        free_all_bitmaps();

        //Set refresh rate
        refresh_screen(FRAME_RATE); // Set refresh rate to default game FRAME_RATE

        // If player presses escape key, end game while loop
        if(key_down(ESCAPE_KEY))
        {
            break;
        }

        // Check again for new messages
        check_network_activity();
    }

    // Once game has ended, close the server.
    close_server(serverName);
}