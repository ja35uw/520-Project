#ifndef __BULLET_AGENT__H
#define __BULLET_AGENT__H 

#include "enviro.h"
#include <math.h>

using namespace enviro;

//!  A bullet controller for a bullet process (bullet aka berries)
class BulletController : public Process, public AgentInterface {

    public:
    BulletController() : Process(), AgentInterface(), hit(false), host_id(-1), bulletcount(0) {}

    //! Runs the handler function upon collisions with agents of the given agent type
    //! Bullets can destroy hives and crazy bees, but not searcher bees
    void init() {
        notice_collisions_with("Wanderer", [&](Event &e) {
            remove_agent(id());
        });
        notice_collisions_with("Bullet", [&](Event &e) {
            remove_agent(id());
        });       
        notice_collisions_with("Hive", [&](Event &e) {
            host_id = e.value()["id"];
            Agent &host = find_agent(host_id);
            hit = true;
        });   
        notice_collisions_with("Monster", [&](Event &e) {
            remove_agent(id());
        });  
    }

    //! To start a new bullet (aka berry) simulation
    void start() {}

    //! Update method counts the number of active berries in the simulation
    //! and removes any additional berries more than two in the simulation.
    //! When the berry hits a beehive, it splits the hive open.
    void update() {
        if ( bulletcount++ > 2 ) {
            remove_agent(id());
        }
        if (hit) {  
            split_hive(); 
        }    
    }

    //! Nothing to do to stop
    void stop() {}

    //! When the hive is split open by a berry, it
    //! generates more crazy bees
    void split_hive() {
        Agent& host = find_agent(host_id);
        for ( double theta = 0; theta < 2*M_PI; theta += M_PI/8 ) {
            Agent& v = add_agent("Monster", host.x() + 47.5*cos(theta), host.y() + 47.5*sin(theta), 0, MONSTER_STYLE);
        }
        remove_agent(host_id);
        remove_agent(id());
    }

    const json MONSTER_STYLE = { 
                {"fill", "yellow"}, 
                {"stroke", "black"}, 
            };

    double bulletcount;
    const int fmax = 100.0;
    int host_id;
    bool hit;

};

//! A simulated bullet (aka berry), which moves in 1 direction for a short distance to destroy enemies
class Bullet : public Agent {
    public:
    Bullet(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    BulletController c;
};

DECLARE_INTERFACE(Bullet)

#endif
