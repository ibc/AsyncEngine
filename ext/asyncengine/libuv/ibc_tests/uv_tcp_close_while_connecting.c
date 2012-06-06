/*
 * libuv reported issue: https://github.com/joyent/libuv/issues/448
 * Status: PENDING.
 * 
 */


#include "uv.h"
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define DEBUG(desc)  fprintf(stdout, "DEBUG: %s:%d:%s::  %s\n", __FILE__, __LINE__, __FUNCTION__, desc)


static uv_loop_t* loop;
static uv_tcp_t tcp_client;
static uv_connect_t connect_req;
static uv_timer_t timer1;
static uv_timer_t timer2;

static void connect_cb(uv_connect_t* req, int status);
static void timer1_cb(uv_timer_t* handle, int status);
static void timer2_cb(uv_timer_t* handle, int status);

static void tcp_close_cb(uv_handle_t* handle) { DEBUG("tcp handle closed"); }
static void timer1_close_cb(uv_handle_t* handle) { DEBUG("timer 1 handle closed"); }
static void timer2_close_cb(uv_handle_t* handle) { DEBUG("timer 2 handle closed"); }


static void connect_cb(uv_connect_t* req, int status)
{
  uv_tcp_t* _uv_handle = (uv_tcp_t*)req->handle;

  if (! status) {
    DEBUG("connected to 1.2.3.4:9999? really???");
    exit(1);
  }
  else {
    DEBUG("connection failed or interrupted");
    printf("... uv last error code: %d\n", uv_last_error(loop).code);
  }
}


/* timer 1 calls to uv_close() for the TCP handle, which SHOULD provoke the
 * connect_cb() with error status right now.
 */
static void timer1_cb(uv_timer_t* handle, int status)
{
  DEBUG("timer 1 fires, calling uv_close() for timer 1 and for tcp handle, connect_cb(EINTR) should be called now !!!");
  uv_close((uv_handle_t *)&timer1, timer1_close_cb);
  uv_close((uv_handle_t *)&tcp_client, tcp_close_cb);
}


static void timer2_cb(uv_timer_t* handle, int status)
{
  DEBUG("timer 2 fires, calling uv_close() for timer 2... OPPS, connect_cb(ENITR) is called now (so late...), WHY ???");
  uv_close((uv_handle_t *)&timer2, timer2_close_cb);
}


int main(int argc, char* argv)
{
  int r;

  loop = uv_default_loop();
  assert(loop != NULL);

  r = uv_tcp_init(loop, &tcp_client);
  assert(r == 0);

  DEBUG("connecting to 1.2.3.4:9999...");
  r = uv_tcp_connect(&connect_req,
                     &tcp_client,
                     uv_ip4_addr("1.2.3.4", 9999),
                     connect_cb);
  assert(r == 0);

  r = uv_timer_init(loop, &timer1);
  assert(r == 0);

  DEBUG("timer 1 will fire in 3 seconds and will call uv_close(tcp handle)...");
  uv_timer_start(&timer1, timer1_cb, 3000, 0);
  assert(r == 0);

  r = uv_timer_init(loop, &timer2);
  assert(r == 0);

  DEBUG("timer 2 will fire in 6 seconds and will do nothing but demonstrate the bug...");
  uv_timer_start(&timer2, timer2_cb, 6000, 0);
  assert(r == 0);

  DEBUG("loop started");
  r = uv_run(loop);
  assert(r == 0);

  return 0;
}