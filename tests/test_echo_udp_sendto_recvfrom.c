#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "config.h"
#include "torture.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

static void setup_echo_srv_udp_ipv4(void **state)
{
	torture_setup_echo_srv_udp_ipv4(state);
}

static void setup_echo_srv_udp_ipv6(void **state)
{
	torture_setup_echo_srv_udp_ipv6(state);
}

static void teardown(void **state)
{
	torture_teardown_echo_srv(state);
}

static void test_sendto_recvfrom_ipv4(void **state)
{
	struct sockaddr_in sin;
	socklen_t slen = sizeof(struct sockaddr_in);
	ssize_t ret;
	int rc;
	int i;
	int s;

	(void) state; /* unused */

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	assert_int_not_equal(s, -1);

	ZERO_STRUCT(sin);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(TORTURE_ECHO_SRV_PORT);

	rc = inet_aton(TORTURE_ECHO_SRV_IPV4, &sin.sin_addr);
	assert_int_equal(rc, 1);

	for (i = 0; i < 10; i++) {
		char send_buf[64] = {0};
		char recv_buf[64] = {0};
		struct sockaddr_in cli_in;
		socklen_t clen = sizeof(cli_in);

		snprintf(send_buf, sizeof(send_buf), "packet.%d", i);

		ret = sendto(s,
			     send_buf,
			     sizeof(send_buf),
			     0,
			     (struct sockaddr *)(void *)&sin,
			     slen);
		assert_int_not_equal(ret, -1);

		ZERO_STRUCT(cli_in);

		ret = recvfrom(s,
			       recv_buf,
			       sizeof(recv_buf),
			       0,
			       (struct sockaddr *)&cli_in,
			       &clen);

		assert_memory_equal(send_buf, recv_buf, sizeof(send_buf));
	}
}

#ifdef HAVE_IPV6
static void test_sendto_recvfrom_ipv6(void **state)
{
	struct sockaddr_in6 sin6;
	socklen_t slen = sizeof(struct sockaddr_in6);
	ssize_t ret;
	int rc;
	int i;
	int s;

	(void) state; /* unused */

	s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	assert_int_not_equal(s, -1);

	ZERO_STRUCT(sin6);
	sin6.sin6_family = AF_INET6;
	sin6.sin6_port = htons(TORTURE_ECHO_SRV_PORT);

	rc = inet_pton(AF_INET6, TORTURE_ECHO_SRV_IPV6, &sin6.sin6_addr);
	assert_int_equal(rc, 1);

	for (i = 0; i < 10; i++) {
		char send_buf[64] = {0};
		char recv_buf[64] = {0};
		struct sockaddr_in6 cli_in6;
		socklen_t clen = sizeof(cli_in6);

		snprintf(send_buf, sizeof(send_buf), "packet.%d", i);

		ret = sendto(s,
			     send_buf,
			     sizeof(send_buf),
			     0,
			     (struct sockaddr *)(void *)&sin6,
			     slen);
		assert_int_not_equal(ret, -1);

		ZERO_STRUCT(cli_in6);

		ret = recvfrom(s,
			       recv_buf,
			       sizeof(recv_buf),
			       0,
			       (struct sockaddr *)&cli_in6,
			       &clen);

		assert_memory_equal(send_buf, recv_buf, sizeof(send_buf));
	}
}
#endif

int main(void) {
	int rc;

	const UnitTest tests[] = {
		unit_test_setup_teardown(test_sendto_recvfrom_ipv4, setup_echo_srv_udp_ipv4, teardown),
#ifdef HAVE_IPV6
		unit_test_setup_teardown(test_sendto_recvfrom_ipv6, setup_echo_srv_udp_ipv6, teardown),
#endif
	};

	rc = run_tests(tests);

	return rc;
}
