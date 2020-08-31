#pragma once

#include <thread>
#include <vector>
#include <atomic>

#include <unistd.h>

#include "track.h"
#include "parameterizable.h"

class StepSequencer : public Parameterizable
{
public:
  StepSequencer()
      :_running(false), _cur_step(0)
  {
      declare_property( "bpm", 120, [=](nlohmann::json v){
         cerr << "BPM set to: " << v.get<double>() << endl;
      });
  }

  virtual ~StepSequencer()
  {

  }

  void addTrack( Track* t )
  {
      _tracks.push_back( t );
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
            double bps = bpm / 60.0;
            std::cerr << ".";
            for( auto t : _tracks )
            {
                t->step(_cur_step);
            }
            _cur_step++;
            usleep( (1.0 / bps) * 1000000.0 );
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

  std::vector<Track*>& tracks()
  {
      return _tracks;
  }

private:
  int _bpm;
  std::thread _mainThread;
  std::atomic<bool> _running;
  std::atomic<int> _cur_step;

  std::vector< Track* > _tracks;

protected:

};
