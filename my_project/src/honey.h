#ifndef __HONEY_AGENT__H
#define __HONEY_AGENT__H 

#include "enviro.h"

using namespace enviro;

//!  A Honey controller for a Honey process
class HoneyController : public Process, public AgentInterface {

    public:
    HoneyController() : Process(), AgentInterface(), host_id(-1) {}

    //! Runs the handler function upon collisions with agents of the given agent type
    //! determines the initial spawn location, and attaches to the bear (player)
    //! If the honey collides with a bee or hive it is removed
    void init() {
        xspawn = x();
        yspawn = y();
        angspawn = angle();

        notice_collisions_with("Player", [&](Event &e) {
            host_id = e.value()["id"];
            Agent &host = find_agent(host_id);
            attach_to(host);
        }); 
        notice_collisions_with("Wanderer", [&](Event &e) {
            remove = true;
        }); 
        notice_collisions_with("Hive", [&](Event &e) {
            remove = true;
        });
        notice_collisions_with("Monster", [&](Event &e) {
            remove = true;
        });
        ignore_collisions_with("Honey");
    }

    //! Nothing to do to start
    void start() {}

    //! When the honey collides with a bee, it is removed and a new honey agent is spawned at
    //! the original spawn location
    void update() {
        if (remove) {
            Agent& v = add_agent("Honey", xspawn, yspawn, angspawn, HONEY_STYLE);
            remove_agent(id());
        }
        decorate("<line x1='-1' x2='0' y1='0.5' y2='0' stroke='black' stroke-width='0.5'></line>"
        "<line x1='1' x2='0' y1='0.5' y2='0' stroke='black' stroke-width='0.5'></line>"
        "<line x1='0' x2='0' y1='-1' y2='0' stroke='black' stroke-width='0.5'></line>");
    }

    //! Nothing to do to stop
    void stop() {}

    const json HONEY_STYLE = { 
        {"fill", "gold"}, 
        {"stroke", "black"}, 
    };

    int host_id;
    bool remove;
    double xspawn;
    double yspawn;
    double angspawn;

};

//! A simulated honey agent which can be picked up by the bear
class Honey : public Agent {
    public:
    Honey(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    HoneyController c;
};

DECLARE_INTERFACE(Honey)

#endif