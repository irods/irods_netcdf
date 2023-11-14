#ifndef NETCDF_PORT_HPP_
#define NETCDF_PORT_HPP_

#include <functional>
#include <irods/apiHandler.hpp>

// _-_-_-_-_-_- Helpers for apidef_t definitions in 4.2.x

#ifdef RODS_SERVER
#  define RODS_SERVER_ENABLE(fn) fn
#else
#  define RODS_SERVER_ENABLE(fn) nullptr
#endif

#define CPP_14_FUNCTION( Func ) std::function<decltype(Func)>{ Func }

#define CPP_14_NOOPFUNC(...) \
  std::function<int (__VA_ARGS__)> { [] (__VA_ARGS__)->int{return 0;} }

namespace irods {

  namespace netcdf {

    template <typename ...T>
    int api_call_wrapper (irods::api_entry* api_, rsComm_t* comm_, T... t)
    {
        return api_->call_handler( comm_ , t...);
    }

  }
}


#define REPEAT_PARAM(n,s) REPEAT_ ## n (s)
#define REPEAT_1(n) n
#define REPEAT_2(n) REPEAT_1(n), n
#define REPEAT_3(n) REPEAT_2(n), n
#define REPEAT_4(n) REPEAT_3(n), n
#define REPEAT_5(n) REPEAT_4(n), n
#define REPEAT_6(n) REPEAT_5(n), n
#define REPEAT_7(n) REPEAT_6(n), n
#define REPEAT_8(n) REPEAT_7(n), n
#define REPEAT_9(n) REPEAT_8(n), n

#define ADD_IRODS_MS_TABLE_ENTRY( N, func )     \
  irods::ms_table_entry* msvc = new irods::ms_table_entry( N ); \
  msvc->add_operation       <           \
    REPEAT_PARAM(N,msParam_t*),         \
    ruleExecInfo_t*         >           \
  (#func,                               \
    std::function<                      \
      int(REPEAT_PARAM(N,msParam_t*),   \
      ruleExecInfo_t*)>(func)           \
  );                                    \
  return msvc

#endif // NETCDF_PORT_HPP_
