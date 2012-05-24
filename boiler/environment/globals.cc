#include <boiler/globals.h>

boiler::globals_t::globals_t() : main_thread_(0), free_logs_(false), respawn_(false), terminate_(false)
{
}

boiler::globals_t::~globals_t()
{
    for(server_map_t::iterator i=servers_.begin();i!=servers_.end();i++)
        delete i->second;
    for(reactor_map_t::iterator i=reactors_.begin();i!=reactors_.end();i++)
        delete i->second;
    for(server_map_t::iterator i=servers_.begin();i!=servers_.end();i++)
        delete i->second;
}
