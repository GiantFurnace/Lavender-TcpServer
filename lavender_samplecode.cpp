/*
 * lavender tcp server sample code
 *
 * Copyright (c) 2018-2028 chenzhengqiang 642346572@qq.com 
 * All rights reserved since 2018-10-04
 *
 * Redistribution and use in source and binary forms, with or without modifica-
 * tIOn, are permitted provided that the following conditions are met:
 *
 *   1.  Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *   2.  RedistributIOns in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentatIOn and/or other materials provided with the distributIOn.
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
 * the GNU General Public License ("GPL") versIOn 2 or any later versIOn,
 * in which case the provisIOns of the GPL are applicable instead of
 * the above. If you wish to allow the use of your versIOn of this file
 * only under the terms of the GPL and not to allow others to use your
 * versIOn of this file under the BSD license, indicate your decisIOn
 * by deleting the provisIOns above and replace them with the notice
 * and other provisIOns required by the GPL. If you do not delete the
 * provisIOns above, a recipient may use your versIOn of this file under
 * either the BSD or the GPL.
 */


#include "tcpserver.h"
int main (int argc, char **argv)
{
    (void) argc;
    (void) argv;
    lavender::TcpServer tcpServer("0.0.0.0:8888");
    tcpServer.serveForever();
}
