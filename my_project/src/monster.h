#ifndef __MONSTER_AGENT__H
#define __MONSTER_AGENT__H 

#include "enviro.h"
#include <math.h>

using namespace enviro;

//!  The state in which the crazy bee moves in a forward direction
    class MovingForward : public State, public AgentInterface {
        
        public:

        //! generates a random velocity
        void entry(const Event& e) {
            rate = rand() % 10 + 50; 
        }

        //! Bee moves in a straight line for a random amount of time with the given velocity
        //! If it detects another bee or is next to another object
        //! the bee will go into the rotate state
        void during() { 
            track_velocity(rate,0);
            count++;
            num = rand() % 20 + 5;
            if ( (sensor_value(0) < 50 || sensor_value(0) == 0) || count > num ) {
                emit(Event(tick_name));
            }    
        }

        //! Nothing to do to exit
        void exit(const Event& e) {}

        //! Gives a unique state change to allow
        //! the bee to change states independent of other bees
        void set_tick_name(std::string s) { tick_name = s; }
        
        std::string tick_name;
        double rate;
        int count;
        int num;
    };

//! The state in which the crazy bee rotates
    class Rotating : public State, public AgentInterface {
       
        public:

        //! Defines the rotation rate of the crazy bee,
        //! a random angle to rotate, and current angle
        void entry(const Event& e) { 
            rate = 50; 
            rotate = ((double) rand() / (RAND_MAX));
            entryangle = angle();
        }

        //! Crazy bee will keep rotating until the designated angle is reached
        //! or if the bee is stuck, in which it will go back to the forward state
        void during() { 
            track_velocity(0,rate); 
            if ( fabs(entryangle - angle()) >= 2*M_PI*rotate || sensor_value(0) == 0 ) {
                emit(Event(tick_name));
            }              
        }

        //! Nothing to do to exit
        void exit(const Event& e) {}

        //! Gives a unique state change to allow
        //! the bee to change states independent of other bees
        void set_tick_name(std::string s) { tick_name = s; }
        
        std::string tick_name; 
        double rate;
        double rotate;
        double entryangle;
       
    };

//! A simulated crazy bee agent, which moves randomly and can destroy the bear
class monsterController : public StateMachine, public AgentInterface {

    public:

    //! Defines the states for the crazy bee, and unique simulation event name to change states
    MovingForward moving_forward;
    Rotating rotating;
    std::string tick_name;

    //! Defines the crazy bee initial state, and transition states from 
    //! moving forward to rotating or vice versa.
    monsterController() : StateMachine(), AgentInterface() {        
        set_initial(moving_forward);
        tick_name = "tick_" + std::to_string(rand()%5000); // use an agent specific generated 
                                                            // event name in case there are 
                                                            // multiple instances of this class
        add_transition(tick_name, moving_forward, rotating);
        add_transition(tick_name, rotating, moving_forward);
        moving_forward.set_tick_name(tick_name);
        rotating.set_tick_name(tick_name);
    }

    //! Runs the handler function upon collisions with agents of the given agent type
    //! Removes the crazy bee when hit by a berry
    void init() {
        notice_collisions_with("Bullet", [&](Event &e) {
            remove_agent(id());
        });
        decorate("<rect x='-4' y='-6' width='1' height='12' stroke='black' fill='black' stroke-width='1'></rect>"
        "<rect x='0' y='-7' width='1' height='14' stroke='black' fill='black' stroke-width='1'></rect>"
        "<rect x='-7' y='-1' width='1' height='2' stroke='black' fill='black' stroke-width='1'></rect>"
        "<rect x='4' y='-6' width='1' height='12' stroke='black' fill='black' stroke-width='1'></rect>");
        StateMachine::init();
    }

    //! Start the state machine when simulation occurs
    void start() {
        StateMachine::start();
    }

    //! Update the state machine during the simulation
    void update() {
        StateMachine::update();
    }

    //! Stop the state machine when simulation ends
    void stop() {
        StateMachine::stop();
    }
};

//! A simulated crazy bee agent, which moves randomly and can destroy the bear
class monster : public Agent {
    public:
    monster(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    monsterController c;
};

DECLARE_INTERFACE(monster)

#endif

  


