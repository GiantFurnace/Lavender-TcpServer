/*
 * lavender tcp server native API header
 *
 * Copyright (c) 2018-2028 chenzhengqiang 642346572@qq.com
 * All rights reserved since 2018-10-22
 *
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 *
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MER-
 * CHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTH-
 * ERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License ("GPL") version 2 or any later version,
 * in which case the provisions of the GPL are applicable instead of
 * the above. If you wish to allow the use of your version of this file
 * only under the terms of the GPL and not to allow others to use your
 * version of this file under the BSD license, indicate your decision
 * by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete the
 * provisions above, a recipient may use your version of this file under
 * either the BSD or the GPL.
 */

#include "tcpserver.h"
#include "error.h"
#include "utils.h"
#include "types.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <errno.h>
extern int errno;
using logging::Nana;

namespace lavender
{

    /*
     * @note:usefull macros for logging
     */

    #define DEBUG (Nana::HAPPY)
    #define INFO (Nana::PEACE)
    #define ERROR (Nana::COMPLAIN)

    #define _EXIT_AND_LOG(); \
    nana_->say(Nana::COMPLAIN, __func__, __LINE__, "Fatal error:",strerror(errno));\
    nana_->die();\
    exit(EXIT_FAILURE);

    #define _LOG_OUTPUT(LEVEL, FORMAT, MESSAGE);\
    nana_->say(LEVEL, __func__, __LINE__, FORMAT, MESSAGE)

    TcpServer::TcpServer(const std::string & address ) throw():
    address_(address), sockfd_(-1), processors_(1), nana_(0), threadIdx_(0)
    {
        int status = utils::net::checkAddress(address_);
        if (status != error::code::SUCCESS)
        {
            std::cerr<<"error:"<<error::getErrorMessage(status)<<std::endl;
            exit(EXIT_FAILURE);
        }
        
        nana_ = Nana::born("./log/lavender.log", Nana::HAPPY);
        nana_->asNormal();
        processors_ = utils::sys::getSysProcessors();
        _LOG_OUTPUT(DEBUG, "%d processors checked in this machine", processors_);
        _initThreadPool();
        std::size_t found = address_.find_first_of(":");
        ip_ = address_.substr(0, found);
        port_ = static_cast<uint16_t>(atoi(address_.substr(found+1, address_.length()-found).c_str()));
    }

    void TcpServer::operator() ( const cactus::EventSon & son )
    {
        (void) son;
    }
    
    void TcpServer::_register() throw()
    {
        int reuse = 1;
        int backlog = 10;
        struct sockaddr_in serverAddr;
        if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        {
            _EXIT_AND_LOG();
        }
        
        if ( setsockopt( sockfd_, SOL_SOCKET, reuse, &reuse, sizeof(reuse)) != 0 )
        {
            _EXIT_AND_LOG();
        }
        bzero( serverAddr.sin_zero, sizeof(serverAddr.sin_zero) );
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = static_cast<uint16_t> (htons( port_ ));
        serverAddr.sin_addr.s_addr = inet_addr( ip_.c_str() );
        if (bind(sockfd_, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <0)
        {
            _EXIT_AND_LOG();
        }

        if (listen(sockfd_, backlog) < 0)
        {
            _EXIT_AND_LOG();
        }
    }

    void TcpServer::_accept(const cactus::EventSon & son)
    {
        struct sockaddr_in clientAddr;
        socklen_t socklen = sizeof(clientAddr);
        int clientSockFd = accept4( son.fd,  (struct sockaddr*)&clientAddr, &socklen, SOCK_NONBLOCK|SOCK_CLOEXEC );
        _LOG_OUTPUT(DEBUG, "Client established from %s", utils::net::getRemoteIPv4(clientSockFd).c_str());

        threads_[threadIdx_].async->send(&clientSockFd, sizeof(clientSockFd));
        ++threadIdx_;
        if (static_cast<long int>(threadIdx_)  >= processors_)
        {
            threadIdx_ = 0;
        }
    }

    void TcpServer::_initThreadPool() throw()
    {
	for ( int index = 0; index < processors_; ++index )
	{
            Thread thread;
	    if (pthread_create( &thread.tid, NULL, _worker, this ) != 0)
	    {
                _LOG_OUTPUT(ERROR, "Warning:thread created error:%s", strerror(errno));
	        continue;
	    }

	    if (pthread_detach( thread.tid ))
	    {
                _LOG_OUTPUT(ERROR, "Warning:thread detached error:%s", strerror(errno));
	        continue;
	    }
            
            thread.async  = new cactus::Async<TcpServer>;
            if ( 0 == thread.async )
            {
                _EXIT_AND_LOG();
            }
            thread.async->set(this, &TcpServer::_asyncRead);
            threads_.push_back(thread);
	}

	if ( threads_.size() <=0 )
	{
            _EXIT_AND_LOG();
	}
    } 


    void TcpServer::serveForever() throw()
    {
        _LOG_OUTPUT(DEBUG, "Register a tcp server on %s", address_.c_str());
        _register();
        acceptIO_.set(sockfd_, types::events::READ, this, &lavender::TcpServer::_accept);
        _LOG_OUTPUT(DEBUG, "%s", "Lavender tcp server is running...");
        acceptIO_.join(mainEventPool_);
        mainEventPool_.run();
    }
    
    void TcpServer::_asyncRead(const cactus::EventSon & son)
    {
       int fd;
       utils::net::readBuffer(son.fd, &fd, sizeof(fd));
       _LOG_OUTPUT(DEBUG, "Receive the socket descroptor:%d from main event loop", fd);
    }

    void * TcpServer::_worker(void *arg) throw()
    {
        TcpServer * server = (TcpServer *)arg;
        std::vector<Thread>::iterator iter = (server->threads_).begin();
        while (iter != (server->threads_).end())
        {
            if (pthread_equal((*iter).tid, pthread_self()))
            {
                break;
            }
            ++iter;
        }
        
        if (iter != (server->threads_).end())
        {
            cactus::EventsPool eventPool;
            (*iter).async->join(eventPool);
            eventPool.run(0);
        }
        return 0;
    }
}
