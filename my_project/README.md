EEP520 Final Project
===
OVERVIEW
---
The goal of the project is to create an adventure mini-game in which the user controls a player using the control keys to navigate from the start to the end while avoiding/destroying enemies along the way.
The simulation consists of a controller player (aka bear sprite) which is controlled using the keyboard commands. The player must navigate through the maze (static elements), and avoid the searcher bees (which are indestructible)
and destroy the hive/crazy bees using berries as ammo to make it to the end location. The bear must collect the last bit of honey in the hive, and get out of the maze safely.

===
DESCRIPTION OF KEY CHALLENGES
---
1. The first challenge was that it took a lot of time to create, scale, and draw blocks for the static elements of the simulation. This challenge was less cumbersome when I implemented a grid pattern, using a 30x30pixel maze to make the coordinates a lot faster and easier to calculate. However this was a very challenging and time consuming task at the beginning of the project development.

2. The second challenge that I spent most of my time on was trying to make the searcher bees to automatically navigate the maze. I was mostly using V1.0 which had very limited functionality and it was difficult for me to implement a reliable algorithm. Through trial and error I modified and fine tuned the physics parameters (collision, friction, etc) so that the searche bee bots could reliable navigate the maze at any location it is placed in the maze. There are still some issues depending on how fast your computer is drawing the images and any lag may cause the searcher bee not clear the intersection correctly. This is also mitigated by implementing an advance state which moves the bees forward to try to clear the intersection and I also increased the size of the walls from 1 px to 5px so that the searcher bees could clear the intersections. I simplified my turn logic and added a few other states to make the transitions cleaner.

3. The third challenge was that I was having issues generating more enemies while trying to remove an agent. I kept crashing the simulation with the following error code:  'what():  Cannot add shapes and bodies to space when it is updating'. This was resolved by reviewing my remove_agent and add_agent functions in the bullet.h file to make sure that the shapes (hives and crazy bees) I were drawing were not overlapping. Furthermore it was important the order in which the removal and addition of the agents occurred so that I wasn't removing the hive first before I could determine how many crazy bee agents to draw/add.

===
HOW TO INSTALL & RUN
---
Assuming the user has docker installed, perform the following steps to install and run the program.

1. Go to Github Repository and download all the files to a folder path on your local drive (you will need to remember the directory path):
https://github.com/ja35uw/520-Project 

2. Open a web browser and enter the following link: http://localhost 
    *NOTE: It should show no connection

3. Run the following sequence of commands:

3a. To start a Docker container with ENVIRO pre-loaded into it, do:

    ```
    docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.6 bash
    ```
*NOTE 1: $PWD should be the local directory on your host. This command will mount that directory and the files into your compiler.
*NOTE 2: It should say "Error: Failed to fetch. Is the server running?" at this point. 
*NOTE 3: If it is not connecting then, you may have to open VirtualBox (which comes with the docker container), and add the following network 'rules' to the image:
    Settings -> Network -> Adapter 1 -> Advanced -> Port Forwarding

    Rule 1 / TCP (Protocol) / Host IP 0.0.0.0 / Host Port 80 / Guest IP 192.168.99.100 / Guest Port 80

    Rule 2 / TCP (Protocol) / Host IP 0.0.0.0 / Host Port 8765 / Guest IP 192.168.99.100 / Guest Port 8765

- The `-p80:80 option maps *port* 80 from the Docker container to ports on your host computer. This should allow you to go to http://localhost with your web browser and see the ENVIRO client.
- The `-p8765:8765` option allows the ENVIRO client to communicate with the `enviro` server (not yet running) via a *WebSocket*.

3b. Then run the command below which uses the ENVIRO setup Manager to start a web server from which your browser can access the ENVIRO client.

    ---
    esm start
    ---

4. Compile the C++ Code, using the command: make

5. Once the code has compiled, use the command: enviro

6. Simulation should then start in the web browser and all user inputs are allowed
    *NOTE: Press `Ctrl-C` to stop enviro.

OPTIONAL

7. The user can edit the config.json file in order to add new agents that have already been defined: wanderer, monster, hive, etc into the simulation. 


===
HOW TO RUN & USE THE PROJECT
---
User can perform the following actions when the simulation is running:
    A. Navigate the bear sprite using the directional keys
    B. Make the bear sprite shoot berries to destroy enemies

    1. 'A' - rotate left (counter-clockwise)
    2. 'W' - move forward
    3. 'S' - move backward
    4. 'D' - rotate right (clockwise) 
    2. Spacebar - shoots berries to defeat some enemies (hives and crazy bees only)


===
ACKNOWLEDGEMENTS
---
I hereby acknowledge the use of code from, but not limited to, Professor Klavin's ENVIRO, ELMA, and all other Klavin's C++ related repositories in the making of the this project.

/***************************************************************************************
*    Title: Enviro
*    Author: Eric Klavins
*    Date: 3-14-2020
*    Code version: V1.6
*    Availability: https://github.com/klavinslab/enviro 
*
***************************************************************************************/
/***************************************************************************************
*    Title: Right Hand Rule
*    Author: Kevin Atienza
*    Date: 	7-06-2016
*    Code version: N/A
*    Availability: https://www.codechef.com/SNCKEL16/problems/MAZETRAV
*
***************************************************************************************/

