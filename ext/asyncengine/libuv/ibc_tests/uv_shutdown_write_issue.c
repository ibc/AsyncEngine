/*
 * Copy this file in libuv directory and run it as follows (Linux):
 *
 * In a second terminal run:
 *   nc -l 127.0.0.1 9999
 *
 * libuv reported issue: https://github.com/joyent/libuv/issues/445
 * Status: FIXED.
 * 
 */


#include "uv.h"
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>


#define DEBUG(desc)  fprintf(stdout, "DEBUG: %s:%d:%s::  %s\n", __FILE__, __LINE__, __FUNCTION__, desc)

#define SERVER_IP  "127.0.0.1"
#define SERVER_PORT  9999


static uv_tcp_t tcp_client;
static uv_connect_t connect_req;
static uv_write_t write_req;
static uv_shutdown_t shutdown_req;
static uv_timer_t timer;

static tcp_connected = 0;

static void connect_cb(uv_connect_t* req, int status);
static void write_cb(uv_write_t* req, int status);
static void read_cb(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
static uv_buf_t alloc_cb(uv_handle_t* handle, size_t suggested_size);
static void read_cb(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
static void shutdown_cb(uv_shutdown_t* req, int status);
static void timer_cb(uv_timer_t* handle, int status);
static void close_cb(uv_handle_t* handle);


static void connect_cb(uv_connect_t* req, int status)
{
  uv_tcp_t* _uv_handle = (uv_tcp_t*)req->handle;

  if (! status) {
    DEBUG("connected");
    tcp_connected = 1;
  }
  else {
    DEBUG("connection failed, run 'nc -l SERVER_IP SERVER_PORT' first please...");
    exit(0);
  }
}


static void write_cb(uv_write_t* req, int status)
{
  if (! status)
    DEBUG("write ok");
  else
    DEBUG("write failed");
}


static uv_buf_t alloc_cb(uv_handle_t* handle, size_t suggested_size)
{
  static char buffer[1024];

  DEBUG("called");

  return uv_buf_init(buffer, sizeof(buffer));
}


static void read_cb(uv_stream_t* stream, ssize_t nread, uv_buf_t buf)
{
  if (nread >= 0)
    DEBUG("data received");
  else {
    DEBUG("disconnected, so exit()");
    exit(0);
  }
}


static void shutdown_cb(uv_shutdown_t* req, int status)
{
if (! status)
    DEBUG("shutdown ok");
  else {
    DEBUG("shutdown failed, so exit()");
    exit(0);
  }

  DEBUG("uv_close(tcp_client), will close_cb() be called?... (NOT !, WHY ?)");
  uv_close((uv_handle_t *)&tcp_client, close_cb);
}


static void timer_cb(uv_timer_t* handle, int status)
{
  int r;
  char data[5] = "HELLO";
  uv_buf_t buf;

  DEBUG("timer fires");

  if (! tcp_connected) {
    DEBUG("tcp_client is not connected when timer fires, run 'nc -l SERVER_IP SERVER_PORT' first please...");
    exit(0);
  }

  DEBUG("uv_close(timer), will close_cb() be called?... (NOT !, WHY ?)");
  uv_close((uv_handle_t *)&timer, close_cb);

  buf = uv_buf_init(data, 5);

  r = uv_write(&write_req, (uv_stream_t *)&tcp_client,
               &buf, 1, write_cb);
  assert(r == 0);

  r = uv_shutdown(&shutdown_req,
                  (uv_stream_t*)&tcp_client,
                  shutdown_cb);
  assert(r == 0);
}


static void close_cb(uv_handle_t* handle)
{
  DEBUG("handle closed");
}


int main(int argc, char* argv)
{
  uv_loop_t* loop;
  int r;

  loop = uv_default_loop();
  assert(loop != NULL);

  r = uv_tcp_init(loop, &tcp_client);
  assert(r == 0);

  r = uv_tcp_connect(&connect_req,
                     &tcp_client,
                     uv_ip4_addr(SERVER_IP, SERVER_PORT),
                     connect_cb);
  assert(r == 0);

  r = uv_read_start((uv_stream_t*)&tcp_client,
                    alloc_cb, read_cb);

  r = uv_timer_init(loop, &timer);
  assert(r == 0);

  uv_timer_start(&timer, timer_cb, 1000, 0);
  assert(r == 0);

  r = uv_run(loop);
  assert(r == 0);

  return 0;
}