#ifndef __FOLLOWER_AGENT__H
#define __FOLLOWER_AGENT__H 

#include "enviro.h"

using namespace enviro;

class Following : public State, public AgentInterface {
    public:
    void entry(const Event& e) {
        //std::cout << "entering rotate " << "\n";
    }
    void during() { 
        track_velocity(3,0);
        position();
        // if ( sensor_value(0) > 140 ) {
        //     emit(Event("ping"));
        //     std::cout << "rotate - ping " << "\n";
        // } 
    }
    void exit(const Event& e) {}      
};

class FollowerController : public StateMachine, public AgentInterface {

    public:

    // Defines states of object
    Following following;

    FollowerController() : StateMachine(), AgentInterface() {

        set_initial(following);
        //add_transition("ping", moving_forward, rotating); // complex
        //add_transition("ping", rotating, moving_forward); // complex
    }

    void init() {
        StateMachine::init();
    }
    void start() {
        StateMachine::start();
    }
    void update() {
        StateMachine::update(); 
    }
    void stop() {}

};

class Follower : public Agent {
    public:
    Follower(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    FollowerController c;
};

DECLARE_INTERFACE(Follower)

#endif