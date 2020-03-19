#ifndef __HIVE_AGENT__H
#define __HIVE_AGENT__H 

#include "enviro.h"
#include <math.h>

using namespace enviro;

//!  A hive controller for a hive process
class hiveController : public Process, public AgentInterface {

    public:
    hiveController() : Process(), AgentInterface() {}

    //! Runs the handler function upon collisions with agents of the given agent type
    void init() {
        ignore_collisions_with("Wanderer"); 
        ignore_collisions_with("Monster"); 
    }

    //! Nothing to do to start
    void start() {}

    //! Hive moves randomly in the x & y direction 
    void update() {
        omni_apply_force(
            1.5*((rand() % fmax) - fmax/2), 
            1.5*((rand() % fmax) - fmax/2)
        );
    }

    //! Nothing to do to stop
    void stop() {}

    const int fmax = 100.0;
};

//! A simulated hive agent, which moves randomly and can destroy the bear
class hive : public Agent {
    public:
    hive(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    hiveController c;
};

DECLARE_INTERFACE(hive)

#endif