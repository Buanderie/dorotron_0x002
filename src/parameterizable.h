#pragma once

#include <functional>

#include <nlohmann/json.hpp>

class Parameterizable
{
public:
  Parameterizable()
  {

  }

  virtual ~Parameterizable()
  {

  }

  nlohmann::json property( const std::string& name )
  {
    if( _properties.find( name ) != _properties.end() )
    {
        return _properties[ name ];
    }
    return nlohmann::json(nullptr);
  }

  bool set_property( const std::string& name, const nlohmann::json& value )
  {
    if( _properties.find( name ) == _properties.end() )
    {
        return false;
    }
    _properties[ name ] = value;
    if( _callbacks.find( name ) != _callbacks.end() )
    {
        _callbacks[ name ]( _properties[name] );
    }
    return true;
  }

private:
    nlohmann::json _properties;
    std::map< std::string, std::function<void(nlohmann::json)> > _callbacks;

protected:
    bool declare_property( const std::string& name,
                            const nlohmann::json& default_value,
                            std::function<void(nlohmann::json)> f = std::function<void(nlohmann::json)>([](nlohmann::json v){}) )
    {
        if( _properties.find( name ) == _properties.end() )
        {
            _properties[ name ] = default_value;
            _callbacks.insert( std::make_pair( name, f ) );
            return true;
        }
        return false;
    }
};
