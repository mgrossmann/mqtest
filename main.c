// Copyright 2007 - 2021, Alan Antonuk and the rabbitmq-c contributors.
// SPDX-License-Identifier: mit

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>


#define SUMMARY_EVERY_US 1000000

static void send_batch(amqp_connection_state_t conn, char const *queue_name,
                       int message_count) {
  int i;
  int sent = 0;
  int previous_sent = 0;

  char message[256] = "FOO BAR\0";
  amqp_bytes_t message_bytes;

  message_bytes.len = sizeof(message);
  message_bytes.bytes = message;

  printf("sending messages %s \n", message);
  for (i = 0; i < message_count; i++) {

    printf("sending message %d\n", i);
    amqp_basic_publish(conn, 1, amqp_literal_bytes("amq.direct"),
                                    amqp_cstring_bytes(queue_name), 0, 0, NULL,
                                    message_bytes);
    sent++;

  }

  {

    printf("PRODUCER - Message count: %d\n", message_count);
  }
}

int main(int argc, char const *const *argv) {

  amqp_rpc_reply_t x;
  char const *hostname;
  int port, status;
  int message_count;
  amqp_socket_t *socket = NULL;
  amqp_connection_state_t conn;

  if (argc < 4) {
    fprintf(stderr,
            "Usage: amqp_producer host port message_count\n");
    return 1;
  }

  hostname = argv[1];
  port = atoi(argv[2]);
  message_count = atoi(argv[3]);

  conn = amqp_new_connection();

  socket = amqp_tcp_socket_new(conn);
  if (!socket) {
    fprintf(stderr, "creating TCP socket");
  }

  status = amqp_socket_open(socket, hostname, port);
  if (status) {
    fprintf(stderr, "opening TCP socket");
  }

  amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                               "guest", "guest");
  
  amqp_channel_open(conn, 1);
  
  x = amqp_get_rpc_reply(conn);
  if (x.reply_type != AMQP_RESPONSE_NORMAL) {
    fprintf(stderr, "%s\n", "get_rpc_reply failed");
  }

  send_batch(conn, "testqueue", message_count);
  send_batch(conn, "test_queue", message_count);
  fprintf(stdout, "Sending done.\n");

  amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
  amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
  amqp_destroy_connection(conn);

  return 0;
}
