#pragma once

#include "channel.h"

#include <raylib.h>

class ChannelWav : public Channel
{
public:
    ChannelWav()
    {
        declare_property( "file", "chibre.wav", [&]( nlohmann::json v){
            loadWav( v.get<std::string>() );
        });
        loadWav( property("file").get<std::string>() );
    }

    virtual ~ChannelWav()
    {

    }

    virtual void trigger() override
    {
        cerr << "playing " << property("file").get<std::string>() << endl;
        PlaySound( _wav );
    }

private:
    Sound _wav;

    void loadWav( const std::string& filePath )
    {
        cerr << "loaded " << filePath << endl;
        _wav = LoadSound(filePath.c_str());
    }

protected:

};
