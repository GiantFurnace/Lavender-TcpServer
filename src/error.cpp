/*
@filename:lavender.cpp
@author:chenzhengqiang
@date:2018-10-02
@email:642346572@qq.com
*/

#include "error.h"

namespace error
{
    /*
    @desc:
     '''
      error message defines here
     '''
    */
    static const char * ErrorMessage[]=
    {
	"success",
        "address format invalid"
    };

    static const int ErrorMessageSize = sizeof( ErrorMessage );

    const char * getErrorMessage( const int errorno )
    {
        if ( errorno < 0 || errorno >= ErrorMessageSize )
	{
	    return 0;
	}

	return ErrorMessage[errorno];
   }
};
