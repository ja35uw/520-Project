#ifndef __WANDERER_AGENT__H
#define __WANDERER_AGENT__H 

#include "enviro.h"
#include <mutex>

using namespace enviro;

// simple move forward behavior
// class MovingForward : public State, public AgentInterface {
//     void entry(const Event& e) {}
//     void during() {
//         track_velocity(10, 0);
//     }
//     void exit(const Event& e) {}
// };

// complex move forward behavior
class MovingForward : public State, public AgentInterface {
    public:
    void entry(const Event& e) {
       // std::cout << "entering moving forward" << "\n";
    }
    void during() { 
        track_velocity(3,0); // Go forward
        

        if ( sensor_value(1) > 15  ) {
            emit(Event("turn right"));
            std::cout << "go right - ping " << "\n";
        // } else if (sensor_value(0) > 30 && (sensor_value(2) <= 30 || sensor_value(1) <= 30)) {
        //     std::cout << "go straight - ping " << "\n";
        //} else if (sensor_value(2) > 30) {
        } else if (sensor_value(0) < 15) {
            emit(Event("turn left"));
            std::cout << "go left - ping " << "\n";
        } else if (sensor_value(0) <= 5 && sensor_value(1) <= 15 && sensor_value(2) <= 15) {
            emit(Event("turn back"));
            std::cout << "turn back - ping " << "\n";
        }

        // 1. Can I go right?
        // 2. If so, go right (call itself, go back to 1).
        // 3. If I can't go right, then can I go straight or left?
        // 4. If possible, go straight first, otherwise go left.
        // 5. Otherwise, go back.


    }



    void exit(const Event& e) {}
};



// Turning precisely could be hard. It is ok to use the angle() method to see which way you are pointing though.
// You could also imagine using move_toward() to move in small steps toward wherever you want to go.

class TurnRight : public State, public AgentInterface {
    public:

    std::mutex mtx;
    double entryangle;

    void entry(const Event& e) {
        entryangle = angle();
        std::cout << entryangle << "\n";
    }
    void during() { 
        mtx.lock();
        track_velocity(0, 2); // Rotate
        mtx.unlock();

        if (fabs(entryangle - angle()) >= 1.55 && fabs(entryangle - angle()) <= 1.58 ) {
        // if (angle() >= 1.50 && angle() <= 1.60 ) {
           // track_velocity(3, 0);
            track_velocity(0,0);
            emit(Event("advance"));
            std::cout << "rotate - ping " << "\n";
            //rot = true;
        }
      //  } else if ( sensor_value(1) <= 78 ) {
        //   } else if ( sensor_value(1) <= 3 && sensor_value(2) < 10  ) {
        //     emit(Event("ping"));
        //     std::cout << "rotate - ping " << "\n";
        //  } 

    }
    void exit(const Event& e) {}      
};

// complex rotate behavior
class TurnLeft : public State, public AgentInterface {
    public:

    std::mutex mtx;
    double entryangle;

    void entry(const Event& e) {
        //std::cout << "entering rotate " << "\n";
        entryangle = angle();
    }
    void during() { 
        mtx.lock();
        track_velocity(0, -2); // Rotate
        mtx.unlock();

       if (fabs(entryangle - angle()) >= 1.55 && fabs(entryangle - angle()) <= 1.58 ) {
           // track_velocity(3, 0);
            track_velocity(0,0);

           // move_toward((x() + 10), y());   // need 1.1 docker image
            emit(Event("advance"));
            std::cout << "left rotate - ping " << "\n";
            //rot = true;
        }
      //  } else if ( sensor_value(1) <= 78 ) {
        //   } else if ( sensor_value(1) <= 3 && sensor_value(2) < 10  ) {
        //     emit(Event("ping"));
        //     std::cout << "rotate - ping " << "\n";
        //  } 

    }
    void exit(const Event& e) {}      

};

// complex rotate behavior
class Advance : public State, public AgentInterface {
    public:

    std::mutex mtx;
    int count;

    void entry(const Event& e) {
        //std::cout << "entering rotate " << "\n";
        count = 0;
    }
    void during() { 
        mtx.lock();
        track_velocity(2, 0); // go forward
        mtx.unlock();
        count++;

       if (count == 10 ) {
           track_velocity(0, 0);
            emit(Event("ping"));
            std::cout << "adv complete - ping " << "\n";
        }
    }
    void exit(const Event& e) {}      
};

// complex rotate behavior
class TurnBack : public State, public AgentInterface {
    public:

    double entryangle;

    void entry(const Event& e) {
        //std::cout << "entering rotate " << "\n";
        entryangle = angle();
    }
    void during() { 
        track_velocity(0, 2); // Rotate
        if (fabs(entryangle - angle()) >= 3.10 && fabs(entryangle - angle()) <= 3.20 ) {
            emit(Event("ping"));
            std::cout << "turnaround complete - ping " << "\n";
        }
    }
    void exit(const Event& e) {}      
};


class WandererController : public StateMachine, public AgentInterface {

    public:
    
    // Defines states of object
    MovingForward moving_forward;
    TurnRight turn_right;
    TurnLeft turn_left;
    Advance advance;
    TurnBack turn_back;

    WandererController() : StateMachine(), AgentInterface() {
    //  WandererController() : StateMachine(), AgentInterface() {    
        set_initial(moving_forward);
        add_transition("turn right", moving_forward, turn_right); 
        add_transition("advance", turn_right, advance);
        add_transition("turn left", moving_forward, turn_left);
        add_transition("advance", turn_left, advance);
        add_transition("ping", advance, moving_forward);
        add_transition("turn back", moving_forward, turn_back);
        add_transition("ping", turn_back, moving_forward);
    }

    void init() {
        StateMachine::init();
    }
    void start() {
        StateMachine::start();
    }
    void update() {
        // if ( rand() % 100 <= 5 ) {
        //     // emit(Event("tick")); // simple
        //     emit(Event("ping")); // complex
        // }   
        StateMachine::update();        
    }
    void stop() {}

};


class Wanderer : public Agent {
    public:
    Wanderer(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    double turnang;

    private:
    WandererController c;
    string rot;
    
};

DECLARE_INTERFACE(Wanderer)

#endif