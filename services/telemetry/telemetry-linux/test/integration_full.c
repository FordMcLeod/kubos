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

#include <cmocka.h>
#include <csp/csp.h>
#include <ipc/csp.h>
#include <ipc/socket.h>
#include <telemetry-linux/server.h>
#include <telemetry/telemetry.h>
#include <tinycbor/cbor.h>

#define TEST_INT_PORT 10
#define TEST_EXT_PORT 20
#define TEST_NUM_CON 5
#define TEST_ADDRESS 1
#define TEST_SOCKET_PORT 8888

static bool test_running = true;
static csp_thread_handle_t server_task_handle;
static telemetry_packet out_pkt = {
    .source.topic_id = 12,
    .source.subsystem_id = 11,
    .source.data_type = TELEMETRY_TYPE_INT,
    .data.i = 99
};

CSP_DEFINE_TASK(server_task)
{
    socket_conn server_conn;
    socket_conn conn;

    assert_true(kprv_socket_server_setup(&server_conn, TELEMETRY_SOCKET_PORT, TELEMETRY_SUBSCRIBERS_MAX_NUM));

    while (test_running && server_conn.is_active)
    {
        while (!kprv_socket_server_accept(&server_conn, &conn))
        {
            continue;
        }

        assert_true(conn.socket_handle > 0);
        assert_true(conn.is_active);

        subscriber_list_item * sub = kprv_subscriber_init(conn);
        if (sub != NULL)
        {
            csp_thread_create(client_handler, NULL, 1000, sub, 0, &(sub->rx_thread));
            kprv_subscriber_add(sub);
        }
    }

    csp_thread_exit();
}

static int setup(void ** arg)
{
    test_running = true;

    csp_thread_create(server_task, "SERVER", 1024, NULL, 0, &server_task_handle);

    return 0;
}

static int teardown(void ** arg)
{
    printf("teardown\r\n");
    test_running = false;

    csp_sleep_ms(100);

    csp_thread_kill(server_task_handle);

    kprv_delete_all_subscribers();

    return 0;
}

static void test_subscriber(void ** arg)
{
    socket_conn conn;
    telemetry_packet in_packet;

    csp_sleep_ms(50);

    assert_true(telemetry_connect(&conn));
    assert_true(telemetry_subscribe(&conn, out_pkt.source.topic_id));

    csp_sleep_ms(10);

    assert_true(telemetry_publish(out_pkt));

    csp_sleep_ms(10);

    assert_true(telemetry_read(&conn, &in_packet));

    assert_int_equal(in_packet.source.topic_id, out_pkt.source.topic_id);
    assert_int_equal(in_packet.data.i, out_pkt.data.i);

    assert_true(telemetry_disconnect(&conn));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_subscriber, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}