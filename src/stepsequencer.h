#pragma once

#include <thread>
#include <vector>
#include <atomic>

#include <unistd.h>

#include "track.h"
#include "channel.h"

#include "parameterizable.h"

#define NUM_CHANNELS 8

class StepSequencer : public Parameterizable
{
public:
    StepSequencer()
        :_running(false), _cur_step(0)
    {
        for( int k = 0; k < NUM_CHANNELS; ++k )
        {
            _channels.push_back( nullptr );
        }

        declare_property( "bpm", 80, [=](nlohmann::json v){
            cerr << "BPM set to: " << v.get<double>() << endl;
        });

        declare_property( "bar_length", 4, [=](nlohmann::json v){
            cerr << "Bar length set to: " << v.get<double>() << endl;
        });

        declare_property( "num_bars", 4, [=](nlohmann::json v){
            cerr << "Bar length set to: " << v.get<double>() << endl;
        });

        for( int k = 0; k < _channels.size(); ++k )
        {
            Track * t = new Track(this);
            t->set_property( "steps", property("bar_length").get<double>() * property("num_bars").get<double>() );
            _tracks.push_back( t );
        }

    }

    virtual ~StepSequencer()
    {

    }

    void start()
    {
        if( !_running )
        {
            _running = true;
            _mainThread = std::thread( [&](){
                while(_running)
                {
                    // Compute step duration
                    double bpm = this->property("bpm").get<double>();
                    double bar_length = this->property("bar_length").get<double>();
                    double bps = bpm / (60.0 / bar_length);
                    std::cerr << ".";
                    for( auto t : _tracks )
                    {
                        t->step(_cur_step);
                    }
                    _cur_step++;
                    usleep( (1.0 / bps) * 1000000.0 + 5000 );
                }
            });
        }
    }

    void stop()
    {
        if(_running)
        {
            _running = false;
            _mainThread.join();
            _cur_step = 0;
        }
    }

    std::vector<Track*> tracks()
    {
        return _tracks;
    }

private:
    int _bpm;
    std::thread _mainThread;
    std::atomic<bool> _running;
    std::atomic<int> _cur_step;

    std::vector< Track* >     _tracks;
    std::vector< Channel* >   _channels;

protected:

};
