
#ifndef __WANDERER_AGENT__H
#define __WANDERER_AGENT__H 

#include "enviro.h"

using namespace enviro;

//!  The state in which the searcher bee moves in a forward direction
class MovingForward : public State, public AgentInterface {
    
    public:

    //! Nothing to do when state is entered
    void entry(const Event& e) {}

    //! Bee moves in a straight line with a constant velocity
    //! It uses the Right-Hand-Rule to navigate the maze, either turning
    //! left, right, advancing forward, going straight, or turning back (if it hits a deadend)
    void during() { 
        track_velocity(3,0); 
        if ( sensor_value(1) > 15  ) {
            emit(Event(tick_name + "turn right"));
        } else if (sensor_value(0) < 15) {
            emit(Event(tick_name + "turn left"));
        } else if (sensor_value(0) <= 5 && sensor_value(1) <= 15 && sensor_value(2) <= 15) {
            emit(Event(tick_name + "turn back"));
        }
    }

    //! Nothing to do when exiting the state
    void exit(const Event& e) {}

    //! Gives a unique state change to allow
    //! the bee to change states independent of other bees
    void set_tick_name(std::string s) { tick_name = s; }

    std::string tick_name; 
};

//! Bee is in a state where it is turning right
class TurnRight : public State, public AgentInterface {
    
    public:

    // Determines the current angle the bee is facing
    void entry(const Event& e) {
        entryangle = angle();
    }

    //! Constantly rotates to the right until 90degs relative to current position when it entered this state
    void during() { 
        mtx.lock();
        track_velocity(0, 2);
        mtx.unlock();
        if (fabs(entryangle - angle()) >= 1.55 && fabs(entryangle - angle()) <= 1.58 ) {
            track_velocity(0,0);
            emit(Event(tick_name + "advance"));
        }
    }

    //! Nothing to do when exiting
    void exit(const Event& e) {}

    //! Gives a unique state change to allow
    //! the bee to change states independent of other bees
    void set_tick_name(std::string s) { tick_name = s; }

    std::string tick_name;   
    std::mutex mtx;
    double entryangle;    
};

//! Bee is in a state where it is turning left
class TurnLeft : public State, public AgentInterface {
    
    public:

    //! Determines the current angle the bee is facing
    void entry(const Event& e) {
        entryangle = angle();
    }

    //! Constantly rotates to the left until 90degs relative to current position when it entered this state
    void during() { 
        mtx.lock();
        track_velocity(0, -2);
        mtx.unlock();
       if (fabs(entryangle - angle()) >= 1.55 && fabs(entryangle - angle()) <= 1.58 ) {
            track_velocity(0,0);
            emit(Event(tick_name + "advance"));
        }
    }

    //! Nothing to do when exiting
    void exit(const Event& e) {}  

    //! Gives a unique state change to allow
    //! the bee to change states independent of other bees    
    void set_tick_name(std::string s) { tick_name = s; }
    
    std::string tick_name;       
    std::mutex mtx;
    double entryangle;
};

//! Bee is in a state where it is advancing forward (after rotating, and out of an intersection)
class Advance : public State, public AgentInterface {
    
    public:

    // Initializing the amount of time elapsed when advancing
    void entry(const Event& e) {
        count = 0;
    }

    //! Bee advances forward slowly for a certain amount of time out of the intersection 
    //! Then it goes back to moving forward
    void during() { 
        mtx.lock();
        track_velocity(2, 0); // go forward
        mtx.unlock();
        count++;
        if (count == 10 ) {
           track_velocity(0, 0);
            emit(Event(tick_name + "ping"));
        }
    }
    
    //! Nothing to do when exiting
    void exit(const Event& e) {}   

    //! Gives a unique state change to allow
    //! the bee to change states independent of other bees  
    void set_tick_name(std::string s) { tick_name = s; }
    
    std::string tick_name; 
    std::mutex mtx;
    int count;
};


//! Bee is in a state where it is turning back forward (when it hits a dead end)
class TurnBack : public State, public AgentInterface {
    
    public:

    //! Determines the current angle the bee is facing
    void entry(const Event& e) {
        entryangle = angle();
    }

    //! Constantly rotates to the right until 180degs relative to current position when it entered this state
    void during() { 
        track_velocity(0, 2); 
        if (fabs(entryangle - angle()) >= 3.10 && fabs(entryangle - angle()) <= 3.20 ) {
            emit(Event(tick_name + "ping"));
        }
    }

    //! Nothing to do when exiting
    void exit(const Event& e) {}  

    //! Gives a unique state change to allow
    //! the bee to change states independent of other bees  
    void set_tick_name(std::string s) { tick_name = s; }

    std::string tick_name; 
    double entryangle;   
};

//! A state where it resets the orientation/velocity of the bee when respawning to its original spot
//! Searcher Bees will loop and teleport back to original position after a certain amount of time has elapsed
//! and repeat the search path again
class Reset : public State, public AgentInterface {
    
    public:

    //! Nothing to do when entering
    void entry(const Event& e) {}

    // Searcher bee is not moving to allow reset properly for a short amount of time before resetting
    void during() { 
        track_velocity(0, 0);
        count++;
        if (count > 1) {
            emit(Event(tick_name + "resetting"));
            count = 0;
        } 
    }

    //! Nothing to do when exiting
    void exit(const Event& e) {} 
      
    //! Gives a unique state change to allow
    //! the bee to change states independent of other bees      
    void set_tick_name(std::string s) { tick_name = s; }   

    std::string tick_name; 
    int count; 
    double entryangle;
};


//! A simulated searcher bee agent, which moves in a predictable path
//! using the right-hand-rule for maze navigation; Cannot be destroyed by the bear (player)
class WandererController : public StateMachine, public AgentInterface {

    public:
    
    // Defines states of searcher bee, and unique simulation event name to change states
    MovingForward moving_forward;
    TurnRight turn_right;
    TurnLeft turn_left;
    Advance advance;
    TurnBack turn_back;
    Reset reset;

    //! Defines the searcher bee initial state, and transition states from 
    //! moving forward, turning left, turning right, turning back, advancing or resetting.
    WandererController() : StateMachine(), AgentInterface() {   
        set_initial(moving_forward);
        tick_name = "tick_" + std::to_string(rand() % 1000); // use an agent specific generated 
                                                    // event name in case there are 
                                                    // multiple instances of this class
        add_transition(tick_name + "turn right", moving_forward, turn_right); 
        add_transition(tick_name + "advance", turn_right, advance);
        add_transition(tick_name + "turn left", moving_forward, turn_left);
        add_transition(tick_name + "advance", turn_left, advance);
        add_transition(tick_name + "ping", advance, moving_forward);
        add_transition(tick_name + "turn back", moving_forward, turn_back);
        add_transition(tick_name + "ping", turn_back, moving_forward);
        add_transition(tick_name + "reset", moving_forward, reset);
        add_transition(tick_name + "reset", turn_right, reset);
        add_transition(tick_name + "reset", turn_left, reset);
        add_transition(tick_name + "reset", advance, reset);
        add_transition(tick_name + "reset", turn_back, reset);
        add_transition(tick_name + "resetting", reset, moving_forward);

        moving_forward.set_tick_name(tick_name);
        turn_right.set_tick_name(tick_name);
        turn_left.set_tick_name(tick_name);
        advance.set_tick_name(tick_name);
        reset.set_tick_name(tick_name);
    }

    //! Runs the handler function upon collisions with agents of the given agent type
    //! Searcher bee ignores all collisions with any agent; Searcher bee spawn location & orientation is defined
    void init() {
        StateMachine::init(); 
        ignore_collisions_with("Player");    
        ignore_collisions_with("Bullet");   
        ignore_collisions_with("Wanderer"); 
        xspawn = x();
        yspawn = y();
        angspawn = angle();
        lifespan = 0;
    }

    //! Nothing to do when starting
    void start() {
        StateMachine::start();
    }

    //! Searcher Bees have a lifespan, and after a certain amount of time has passed
    //! they teleport back to their original spawn location/orientation and start searching through the maze again
    void update() { 
        StateMachine::update();   
        lifespan++;
        if (lifespan >= 1000) {
            emit(Event(tick_name + "reset"));
            teleport(xspawn, yspawn, angspawn);
            lifespan = 0;
        }  
        decorate("<rect x='-3' y='-5' width='1' height='10' stroke='black' fill='black' stroke-width='1'></rect>"
        "<rect x='0' y='-5' width='1' height='10' stroke='black' fill='black' stroke-width='1'></rect>"
        "<rect x='3' y='-5' width='1' height='10' stroke='black' fill='black' stroke-width='1'></rect>");
    }

    //! Nothing to do when stopping
    void stop() {
        StateMachine::stop();
    }

    int lifespan;
    std::string tick_name;
    double xspawn;
    double yspawn;
    double angspawn;
};

//! A simulated searcher bee agent, which navigates the maze slowly using the right-hand-rule;
//! It is indestructible!
class Wanderer : public Agent {
    public:
    Wanderer(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }

    private:
    WandererController c;
};

DECLARE_INTERFACE(Wanderer)

#endif
