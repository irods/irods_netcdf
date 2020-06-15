#ifndef myjansson_h__
#define myjansson_h__


#ifdef __cplusplus
#  include <memory> 
#  include <vector> 
#  include <cstdlib>
#  ifndef LEGACY_JANSSON
#    include "json.hpp"
#  endif  // LEGACY_JANSSON
#endif // __cplusplus

#ifdef LEGACY_JANSSON
#include <jansson.h>
#else
  /* If your compiler supports the `long long` type and the strtoll() library
     function, JSON_INTEGER_IS_LONG_LONG is defined to 1, otherwise to 0. 
   */
#define JSON_INTEGER_IS_LONG_LONG 1
#endif

#undef GET
#undef PTR

#ifndef LEGACY_JANSSON

#  define __Json_string_value_support__   	  		\
     std::vector<std::string> dummy_vec;			\
     auto Json_string_value = [&dummy_vec] (Json_t* j) { 	\
       dummy_vec.push_back( *j );				\
       return dummy_vec.back().c_str();				\
     }

   using Json_t = nlohmann::json;
   using Json_type_t = int;

#  define PTR(type) std::shared_ptr<type>
#  define GET(ptr) ((ptr).get())

   void Json_decref (PTR(Json_t) p);

#else // LEGACY_JANSSON

#  define __Json_string_value_support__

   typedef json_t  Json_t;
   typedef  json_t* Json_t_ptr;

#  define PTR(type) type ## _ptr
#  define GET(ptr) (ptr)

#endif // LEGACY_JANSSON

//------------------
// functions / types

#ifndef LEGACY_JANSSON

char* Json_dumps (Json_t*,size_t);
Json_t* Json_real      (double);
Json_t* Json_integer   (long);
Json_t* Json_true ();
Json_t* Json_false ();

int Json_is_boolean  (Json_t*);
int Json_is_string   (Json_t*);
int Json_is_object   (Json_t*);
int Json_is_array    (Json_t*);
int Json_is_integer  (Json_t*);
int Json_is_real     (Json_t*);
int Json_is_null     (Json_t*);
int Json_is_true     (Json_t*);
int Json_is_false    (Json_t*);

Json_type_t Json_typeof (Json_t* p);

Json_t* Json_deep_copy(Json_t*);
Json_t* Json_string(const char *s) ;
Json_t* Json_array () ;
Json_t* Json_object () ;
Json_t* Json_null () ;

int	Json_object_set ( Json_t * obj , const char* key , Json_t* value) ;
int	Json_object_set_new ( Json_t * obj , const char* key, PTR(Json_t) value ) ;
Json_t* Json_pack( const char* fmt , ...) ;


    enum {
        JSON__OBJECT,
        JSON__ARRAY,
        JSON__STRING,
        JSON__INTEGER,
        JSON__REAL,
        JSON__TRUE,
        JSON__FALSE,
        JSON__NULL
    };

    using Json_object_iter_type = decltype( std::declval<Json_t>().begin() );

#undef GOOD
#define GOOD( object, iter ) ( (iter) != (*(object)).end() )

    Json_object_iter_type	Json_object_iter( Json_t * p );
    const char *         	Json_object_iter_key (Json_object_iter_type);
    Json_t*		 	Json_object_iter_value (Json_object_iter_type);
    Json_object_iter_type	Json_object_iter_next (Json_t* p, Json_object_iter_type it0);

    Json_type_t Json_typeof (Json_t*) ;

#if JSON_INTEGER_IS_LONG_LONG
using json_int_t = long long;
#else
using json_int_t = long;
#endif

    json_int_t Json_integer_value (Json_t*);
    double Json_real_value( Json_t *); 
    Json_t* Json_string(const char*);
    Json_t* Json_array ();
    Json_t* Json_object ();
    struct Json_error_t { char text[160]; };
    Json_t* Json_loads(const char *, ssize_t, Json_error_t *);

    inline Json_t*	Json_object_get( Json_t *p , const char *n)
    {
      return &((*p)[n]);
    }
    inline Json_t*	Json_array_get( Json_t *p , size_t n) 
    {
      return &((*p)[n]);
    }
    inline size_t	Json_array_size( const Json_t *p) 
    {
      return (*p).size();
    }

// ....

#else // LEGACY_JANSSON

#undef GOOD
#define GOOD( object, iter ) ((iter) != nullptr)

// -- Jansson methods 

#define Json_array_get		json_array_get
#define Json_array_size		json_array_size
#define Json_decref		json_decref
#define Json_deep_copy          json_deep_copy
#define Json_dumps		json_dumps
#define Json_error_t		json_error_t
#define Json_false		json_false
#define Json_integer		json_integer
#define Json_integer_value	json_integer_value
#define Json_is_array		json_is_array
#define Json_is_false		json_is_false
#define Json_is_integer		json_is_integer
#define Json_is_null		json_is_null
#define Json_is_object		json_is_object
#define Json_is_real		json_is_real
#define Json_is_string		json_is_string
#define Json_is_true		json_is_true
#define Json_loads		json_loads
#define Json_object		json_object
#define Json_object_get		json_object_get
#define Json_object_iter	json_object_iter
#define Json_object_iter_key	json_object_iter_key
#define Json_object_iter_next	json_object_iter_next
#define Json_object_iter_value	json_object_iter_value
#define Json_object_set_new	json_object_set_new
#define Json_pack		json_pack
#define Json_real		json_real
#define Json_real_value		json_real_value
#define Json_string		json_string
#define Json_string_value	json_string_value
#define Json_true		json_true
#define Json_type		json_type
#define Json_typeof		json_typeof

#define	JSON__ARRAY		JSON_ARRAY
#define	JSON__FALSE		JSON_FALSE
#define	JSON__INTEGER		JSON_INTEGER
#define	JSON__INTEGER_IS_LONG_LONG	JSON_INTEGER_IS_LONG_LONG
#define	JSON__OBJECT		JSON_OBJECT
#define	JSON__REAL		JSON_REAL
#define	JSON__STRING		JSON_STRING
#define	JSON__TRUE		JSON_TRUE

#endif // ! LEGACY_JANSSON

#endif //myjansson_h__
