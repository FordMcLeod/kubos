/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @defgroup PubSub Pub Sub Interface
 * @addtogroup PubSub
 * @{
 */

#ifndef PUBSUB_H
#define PUBSUB_H

#include <csp/csp.h>
#include <csp/interfaces/csp_if_socket.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * PubSub connection structure ...
 */
typedef struct
{
    /**
     * Raw network connection handle - today this is a csp connection 
     */
    csp_conn_t * conn_handle;
    /**
     * CSP socket interface
     */
    csp_iface_t csp_socket_if;
    /**
     * CSP socket handle
     */
    csp_socket_handle_t socket_driver;
} pubsub_conn;

/**
 * Performs the necessary setup for the telemetry server to begin
 * receiving subscriber connections.
 * @param[in] port port to bind server to
 * @param[in] num_connections number of connections for server to listen to
 * @return csp_socket_t * pointer to created socket handle
 */
csp_socket_t * kprv_server_setup(uint8_t port, uint8_t num_connections);

/**
 * Attempts to accept a subscriber connection.
 * @param[in,out] socket pointer to socket handle
 * @param[in,out] conn pointer to pubsub_conn where connection info will be stored 
 * @return bool true if successful, otherwise false
 */
bool kprv_server_accept(csp_socket_t * socket, pubsub_conn * conn);

/**
 * Attempts to accept a subscriber connection over a tcp socket.
 * @param[in,out] socket pointer to csp socket handle.
 * @param[in,out] conn pointer to pubsub_conn where connection info will be stored
 * @return bool true if successful, otherwise false
 */
bool kprv_server_socket_accept(csp_socket_t * socket, pubsub_conn * conn);

/**
 * Performs shutdown and cleanup of tcp socket based connections.
 * @param conn pointer to pubsub_conn where connection info is stored
 */
void kprv_subscriber_socket_close(pubsub_conn * conn);

/**
 * Used by a telemetry subscriber to connect to the publishing server.
 * @param[in,out] conn pointer to pubsub_conn where connection info will be stored
 * @param[in] address address of server
 * @param[in] port port of server
 * @return bool true if successful, otherwise false
 */
bool kprv_subscriber_connect(pubsub_conn * conn, uint8_t address, uint8_t port);

/**
 * Used by a client to connect to a server using a tcp socket.
 * @param[in,out] conn pointer to pubsub_conn where connection info will be stored
 * @param[in] address address of server
 * @param[in] port port of server
 * @return bool true if successful, otherwise false
 */
bool kprv_subscriber_socket_connect(pubsub_conn * conn, uint8_t address, uint8_t port);

/**
 * Attempts to receive data over the specified pubsub_conn
 * @param[in] conn pubsub_conn to receive from
 * @param[out] buffer buffer to store data in
 * @param[in] buffer_size expected size of buffer
 * @param[in] port expected port for data to come in on
 * @return bool true if successful, otherwise false
 */
bool kprv_publisher_read(const pubsub_conn * conn, void * buffer, int buffer_size, uint8_t port);

/**
 * Attempts to receive data over the specified pubsub_conn
 * @param[in] conn pubsub_conn to receive from
 * @param[out] buffer buffer to store data in
 * @param[in] buffer_size expected size of buffer
 * @param[in] port expected port for data to come in on
 * @return bool true if successful, otherwise false
 */
bool kprv_subscriber_read(const pubsub_conn * conn, void * buffer, int buffer_size, uint8_t port);

/**
 * Wrapper function for sending data via a csp connection
 * @param[in] conn pubsub_conn containing a valid csp_conn_t *
 * @param[in] data void pointer to data to be sent
 * @param[in] length length of the data to be sent
 * @return bool true if successful, otherwise false
 */
bool kprv_send_csp(const pubsub_conn * conn, const void * data, uint16_t length);

#endif

/* @} */
