#ifndef __PLAYER_AGENT__H
#define __PLAYER_AGENT__H 

#include "enviro.h"

using namespace enviro;

//!  A Player (aka bear) controller for a player agent
class PlayerController : public Process, public AgentInterface {

    public:
    PlayerController() : Process(), AgentInterface(), f(0), tau(0), firing(false) {}

    //! Allows the user to shoot bullets with the 'spacebar', navigate with 
    //! 'A' - left, 'W' - forward, 'S' - backward, 'D' - right keys
    void init() {
        watch("keydown", [&](Event &e) {
            auto k = e.value()["key"].get<std::string>();
            if ( k == " " && !firing ) {
                  Agent& bullet = add_agent("Bullet", 
                    x() + 17*cos(angle()), 
                    y() + 17*sin(angle()), 
                    angle(), 
                    BULLET_STYLE);    
                    bullet.apply_force(10,0);
                  firing = true;
            } else if ( k == "w" ) {
                  f = magnitude;              
            } else if ( k == "s" ) {
                  f = -magnitude;  
            } else if ( k == "a" ) {
                  tau = -magnitude;
            } else if ( k == "d" ) {
                  tau = magnitude;
            } 
        });        

        //! Depressing the control keys will stop the player input movement/command
        watch("keyup", [&](Event &e) {
            auto k = e.value()["key"].get<std::string>();
            if ( k == " " ) {
                firing = false;
            } else if ( k == "w" || k == "s" ) {
                  f = 0;               
            } else if ( k == "a" ) {
                  tau = 0;
            } else if ( k == "d" ) {
                  tau = 0;
            } 
        });
        center(x(), y());
        zoom(2);

        //! Runs the handler function upon collisions with agents of the given agent type
        //! Reset the player's position to the starting point when the player collides with an enemy
        ignore_collisions_with("Wanderer"); 
        notice_collisions_with("Wanderer", [&](Event &e) {
            teleport(195,-240, -1.5707963268);
        });   
        notice_collisions_with("Hive", [&](Event &e) { 
            teleport(195,-240, -1.5707963268);
        });   
        notice_collisions_with("Monster", [&](Event &e) {
            teleport(195,-240, -1.5707963268);
        });   

        decorate("<rect x='1' y='-2' width='1' height='1' stroke='black' fill='black' stroke-width='1'></rect>"
        "<rect x='1' y='1' width='1' height='1' stroke='black' fill='black' stroke-width='1'></rect>");
    }

    //! Nothing to do to start
    void start() { }

    //! Applies a force in the x & y direction to move the player when key is depressed
    void update() {
        apply_force(f,tau);
        center(x(), y());
    }

    //! Nothing to do to stop
    void stop() {}

    double f, tau;
    double const magnitude = 100;
    bool firing;
    const json BULLET_STYLE = { 
                   {"fill", "red"}, 
                   {"stroke", "#888"}, 
               };

};

//! A simulated player (bear) agent, which moves and shoots bullets (berries) per the user input
class Player : public Agent {
    public:
    Player(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    PlayerController c;
};

DECLARE_INTERFACE(Player)

#endif