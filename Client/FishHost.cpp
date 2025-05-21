/**
 * Using splashkit for GUI interface
 * Using cmath for calculating x/y changes based on fish direction
 */
#include "splashkit.h"
#include <cmath>

// To_string in case needed for debugging purposes and drawing score/lives to screen
using std::to_string;

// Networking info for connection
const string hostIP = "192.168.56.101";
const connection_type protocol = UDP;
const unsigned short portNum = 5432;
const string serverConnectionName = "server connection";

// Screen window dimensions
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

// Set the fish defaults for size, swimspeed and turn speed.
const float DEFAULT_SIZE = 1.0;
const float DEFAULT_SWIM = 1;
const float DEFAULT_TURN = 2;

// Used for setting the refresh screen rate.
const int FRAME_RATE = 30;

// Constant strings for the game timer and bitmap names
const string GAME_TIMER = "Game Timer";
const string PLAYER_BITMAP = "PlayerBMP";

// Arrays to hold the names of png files for player and npc fish images
const string playerFiles[] = {"PlayerFishMid.png", "PlayerFishLeft.png", "PlayerFishRight.png"};

// Enum to determine if a fish is currently turning left/right or swimming straight
enum turn_enum
{
    none,
    left,
    right
};

// Fish struct used in both player and NPC fish
struct fish
{
    point_2d pos; // Fishes current X/Y position on the screen
    float swim_speed; // How fast the fish moves forward
    float turn_speed; // How fast the fish rotates/turns
    double angle; // The angle the fish is currently facing (uses range of -180 to 180)
    turn_enum turn; // Turn enum for if the fish is turning or swimming straight
    float size; // Size modifier of the fish
    bitmap bmp; // The bitmap used to load the png file

    /**
     * Constructor for a fish
     * @param setNpc used to determine if it should create a player or NPC fish
     */
    fish()
    {
            // Spawn player fish in the middle of the screen
        pos.x = SCREEN_WIDTH / 2;
        pos.y = SCREEN_HEIGHT / 2;

            // Player fish spawns facing directly up, and sets the size and turn speed to be default starting values
        angle = 0;
        size = DEFAULT_SIZE;
        turn_speed = DEFAULT_TURN;

        // Both player and NPC fish will start with default swim speed value
        // Also sets the current turning status to swimming straight to begin with
        swim_speed = DEFAULT_SWIM;
        turn = none;
    }

    // Swim function that uses fish current status to determine change in X/Y position
    void swim()
    {
        // If fish is NPC, load the npc bitmap using current turn status, if player load player bitmap instead
        bmp = load_bitmap(PLAYER_BITMAP, playerFiles[turn]);

        // Create new 2d point to serve as a modifier for X/Y position changes
        point_2d swimming = {0, 0}; 

        // Calculate radians from the fish's current angle faced, used to calculate change in x/y position
        double radians = angle * (3.14159 / 180.0); 

        // Create a blank vector that holds bool status of which walls fish are currently colliding with
        vector<bool> walls = {0, 0, 0, 0};
        wall_check(bmp, walls);

        // When adding each wall value together, if result is above 1 then player is in a corner
        if((walls[0] + walls[1] + walls[2] + walls[3]) > 1) // If more than 1 wall collision is met, then result will be 2 or higher
        {
            if(walls[0]) // If one of the collided walls is the top aka wall 0
            {
                if(walls[3]) // If second wall is left wall then shift player down and right
                {
                    swimming.x = 5;
                    swimming.y = 5;
                }
                else // Otherwise shift player down left (second wall is right wall)
                {
                    swimming.x = -5;
                    swimming.y = 5;
                }
            }
            else if(walls[2]) // If one of the collided walls is the bottom aka wall 3
            {
                if(walls[3]) // If second wall is left wall then shift player up and right
                {
                    swimming.x = 5;
                    swimming.y = -5;
                }
                else // Otherwise shift player up and left (second wall is right wall)
                { 
                    swimming.x = -5;
                    swimming.y = -5;
                }
            }
            // Rotate fish angle to prevent fish becoming locked in corner
            angle += 3;
        }
        else // If fish is currently colliding with 1 or less walls
        {
            // Use a for loop to check each wall collision status 1 at a time
            for(int i = 0; i < 4; i++)
            {
                if(walls[i]) // If current selected wall collision is true
                {
                    switch (i) // Check which wall it is
                    {
                    case 0: // If its 0, meaning top of screen
                        if(angle >= -95 && angle <= 0) // While fish is facing left relative to wall
                        {
                            angle -= (turn_speed * 2); // force fish to turn away from wall
                            swimming.y = 0; // Also set change in y to 0 to prevent fish swimming past wall
                        }

                        else if (angle >= 0 && angle <= 95) // Otherwise if fish is facing right relative to wall
                        {
                            angle += (turn_speed * 2); // force fish to turn away from wall
                            swimming.y = 0; // Also set change in y to 0 to prevent fish swimming past wall
                        }
                        break;
                    case 1: // If its 1, meaning right of screen
                        if(angle >= -5 && angle <= 95) // While fish is facing left relative to wall
                        {
                            angle -= (turn_speed * 2); // force fish to turn away from wall
                            swimming.x = 0; // Also set change in x to 0 to prevent fish swimming past wall
                        }
                        else if (angle >= 95 && angle <= 185)
                        {
                            angle += (turn_speed * 2); // force fish to turn away from wall
                            swimming.x = 0; // Also set change in x to 0 to prevent fish swimming past wall
                        }
                        break;
                    case 2:
                        if(angle <= 185 && angle >= 90)
                        {
                            angle -= (turn_speed * 2); // force fish to turn away from wall
                            swimming.y = 0; // Also set change in y to 0 to prevent fish swimming past wall
                        }
                        else if (angle >= -185 && angle <= -90)
                        {
                            angle += (turn_speed * 2); // force fish to turn away from wall
                            swimming.y = 0; // Also set change in y to 0 to prevent fish swimming past wall
                        }
                        break;
                    case 3:
                        if(angle <= -95 && angle >= -180)
                        {
                            angle -= (turn_speed * 2); // force fish to turn away from wall
                            swimming.x = 0; // Also set change in x to 0 to prevent fish swimming past wall
                        }
                        else if (angle <= 5 && angle >= -90)
                        {
                            angle += (turn_speed * 2); // force fish to turn away from wall
                            swimming.x = 0; // Also set change in x to 0 to prevent fish swimming past wall
                        }
                        break;
                    default:
                        break;
                    }
                }
                else // If fish is not currently colliding with any walls
                {
                    // Calculate using trigonometry for change in x and y
                    swimming.x = (swim_speed * sin(radians));
                    swimming.y = -1 * (swim_speed * cos(radians));
                }
            }
        }

        // After change in x/y is calculated, increase fish position by the swim speed modified by the change multiplied in the swimming 2d point
        pos.x += (swimming.x * swim_speed);
        pos.y += (swimming.y * swim_speed);

        //End of swim()
    }

    /**
     * Uses current fish postion to determine if fish is currently colliding 
     * 
     * @param bmp Passes the loaded bitmap of the fish used to check for collisions
     * @param walls A reference to passed vector used to store 4 boolean values for each wall
     */
    void wall_check(bitmap bmp, vector<bool> &walls)
    {
        // Create a 2d point from the current fish position, modify position to account for bitmap size
        // Also creates a default rectangle that will be modified for each of the walls
        point_2d p = {pos.x - (1 * 13), pos.y - (1 * 24)};
        rectangle r;
        r = {0 - 20, 0 - 20, SCREEN_WIDTH + 40, 25}; // Top wall aka 0
        if(bitmap_rectangle_collision(bmp, p, r)) // If collision is detected with wall then set appropriate wall in walls vector to true
            walls[0] = 1;

        r = {0 - 20, SCREEN_HEIGHT - 5, SCREEN_WIDTH + 40, 25}; // Bottom wall aka 2
        if(bitmap_rectangle_collision(bmp, p, r)) // If collision is detected with wall then set appropriate wall in walls vector to true
            walls[2] = 1;
        
        r = {SCREEN_WIDTH - 10, 0, 45, SCREEN_HEIGHT + 25}; // Right wall aka 1 
        //r = {SCREEN_WIDTH - 5, -5, 40, SCREEN_HEIGHT + 25}; // Right wall aka 1
        if(bitmap_rectangle_collision(bmp, p, r)) // If collision is detected with wall then set appropriate wall in walls vector to true
            walls[1] = 1;
        
        r = {5, 0, 5, SCREEN_HEIGHT}; // Left wall aka 3
        if(bitmap_rectangle_collision(bmp, p, r)) // If collision is detected with wall then set appropriate wall in walls vector to true
        {
            walls[3] = 1;
        }

        // End of wall_check()
    }

    /**
     * Draws the current fish based on current values
     */
    void draw()
    {
        // Loads default drawing options used in draw_bitmap function
        drawing_options drawOptions = option_defaults();

        bmp = load_bitmap(PLAYER_BITMAP, playerFiles[turn]); // Uses current turn status to load correct png

        // Rotates the image based on the direction fish is currently facing
        drawOptions = option_rotate_bmp(angle);

        // Scales x and y by current fish size
        drawOptions.scale_x = size;
        drawOptions.scale_y = size;

        // Draw the bitmap to screen, modifies position to account for the image dimensions
        // Was unable to work out a better way other than modifying where image is drawn for better collission detection
        draw_bitmap(bmp, pos.x - (1 * 13), pos.y - (1 * 24), drawOptions);
    }
};

//Struct for the player handling
struct player
{
    fish playerFish; // Fish struct for the player
    int lives; // Lives remaining (for later updates with game over functionality)
    bool shrink; // If the player currently is being shrunk (after reaching size limit)

    /**
     * Default constructor for a player
     */
    player()
    {
        playerFish = fish(); // Create a new player fish
        lives = 1; // Set lives to 1, currently unused
        shrink = false; // Set shrinking status to false
    }

    /**
     * Handles modifying fish based on keyboard input
     */
    void input_handler()
    {
        // Checker to make sure fish doesnt turn beyond angle limits
        if(playerFish.angle > 180)
            playerFish.angle -= 360;
        else if(playerFish.angle < -180)
            playerFish.angle += 360;

        
        if ((key_down(UP_KEY) || key_down(W_KEY)) && playerFish.swim_speed <= 2) // While up arrow or w key is pressed continue iterating fish speed up to 2
            playerFish.swim_speed += 0.1;
        else if (playerFish.swim_speed > 1) // If W or up arrow key is not pressed, decrement speed until it returns to default
            playerFish.swim_speed -= 0.1;

        // Safety check in case both left and right keys are pressed, ensuring player just swims straight
        if((key_down(RIGHT_KEY) || key_down(D_KEY)) && (key_down(LEFT_KEY) || key_down(A_KEY)))
            playerFish.turn = none;
        // If right arrow or D key is pressed, turn fish right
        else if (key_down(RIGHT_KEY) || key_down(D_KEY))
        {   
            playerFish.turn = right;
            playerFish.angle += playerFish.turn_speed;
        }
        // If left arrow or A key is pressed, turn fish left
        else if (key_down(LEFT_KEY) || key_down(A_KEY))
        {
            playerFish.turn = left;
            playerFish.angle -= playerFish.turn_speed;
        }
        // If no valid controls are pressed, keep turn status as none (swimming straight)
        else
        {
            playerFish.turn = none;
        }

        // For testing purposes, increase fish size when space is held
        // Decrease fish size when right alt is held
        if(key_down(SPACE_KEY))
            playerFish.size += 0.1;
        if(key_down(RIGHT_ALT_KEY))
            playerFish.size -= 0.1;

        // End of player input_handler()
    }
};

// Struct for the game status
struct game
{
    player Player; // Current player of the game
    float messageTime;

    // Default constructor of game, creates a player and sets score to 0
    game()
    {
        Player = player();
        messageTime = 1000;
    }

    /**
     * Draw game info to screen
     */
    void draw()
    {
        clear_screen(color_cornflower_blue()); // Set background to blue
        rectangle r; // Create rectangle for drawing the edges of the screen

        r = {0 - 20, 0 - 20, SCREEN_WIDTH + 40, 5 + 20}; // Top wall aka 0
        fill_rectangle(color_aquamarine(), r); // Draw top wall

        //r = {SCREEN_WIDTH - 5, 0, 5, SCREEN_HEIGHT}; // Right wall 1
        r = {SCREEN_WIDTH - 5, -5, 40, SCREEN_HEIGHT + 25}; // Right wall aka 1
        fill_rectangle(color_aquamarine(), r); // Draw right wall

        //r = {0, SCREEN_HEIGHT - 5, SCREEN_WIDTH, 5}; // Bottom wall 2
        r = {0 - 20, SCREEN_HEIGHT - 5, SCREEN_WIDTH + 40, 25}; // Bottom wall aka 2
        fill_rectangle(color_aquamarine(), r); // Draw bottom wall

        //r = {0, 0, 5, SCREEN_HEIGHT}; // Left wall 3
        r = {5 - 20, 0 - 5, 20, SCREEN_HEIGHT + 25}; // Left wall aka 3
        fill_rectangle(color_aquamarine(), r); // Draw left wall
        
        Player.playerFish.draw(); //  Draw the player's fish to the screen
        //fill_circle(color_bright_green(), SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2, 3); Used to test where the center of screen is
        refresh_screen(FRAME_RATE); // Set refresh rate to default game FRAME_RATE
        free_all_bitmaps();
    }

    // Update the game information
    void update()
    {
        Player.input_handler(); // Call player input handler for checking inputs
        Player.playerFish.swim(); // Move player fish based on fish status
    }

    string constructMessage()
    {
        string message = "";
        message += "X" + to_string(Player.playerFish.pos.x) + "|";
        message += "Y" + to_string(Player.playerFish.pos.y) + "|";
        message += "R" + to_string(Player.playerFish.angle) + "|";
        message += "S" + to_string(Player.playerFish.size) + "|";
        message += "T" + to_string(Player.playerFish.turn) + "|";
        //write_line(message);

        return message;
    }
};

// Main function for fish game
int main()
{   
    game Game; // Create default new game struct

    // Create the window and set size
    open_window("Host BigFish", SCREEN_WIDTH, SCREEN_HEIGHT);

    // Create the connection to server program
    connection serverConnection = open_connection(serverConnectionName, hostIP, portNum, protocol);
    
    // Create and start the timer
    create_timer(GAME_TIMER);
    start_timer(GAME_TIMER);

    string message = "";

    // Continue loop until player chooses to quit
    while (!quit_requested())
    {
        process_events(); // Process the current keyboard/mouse input status

        Game.update(); // Update game values
        Game.draw(); // Draw game values to screen

        
        if(Game.messageTime <= timer_ticks(GAME_TIMER))
        {
            message = Game.constructMessage();
            send_message_to(message, serverConnection);
            Game.messageTime = timer_ticks(GAME_TIMER) + 50;
        }
        //write_line("Message sent: " + message);
    }
    //End of main()
}