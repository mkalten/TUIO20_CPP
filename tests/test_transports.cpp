#include "test_helpers.h"
#include "UdpReceiver.h"
#include "UdpSender.h"
#include "TcpReceiver.h"
#include "TcpSender.h"
using namespace TUIO;

TEST(udp_receiver_busy_port) {
	// first receiver binds the port; second must fail gracefully
	UdpReceiver first(13415);
	CHECK(first.socket != NULL);
	UdpReceiver second(13415);
	CHECK(second.socket == NULL); // bind failure -> NULL socket path
	second.connect();             // must not spawn a thread or crash
	CHECK(!second.isConnected());
}

TEST(udp_receiver_connect_cycle) {
	UdpReceiver recv(13416);
	recv.connect();
	CHECK(recv.isConnected());
	// disconnect must return promptly (AsynchronousBreak + join)
	recv.disconnect();
	CHECK(!recv.isConnected());
	// idempotent disconnect is safe
	recv.disconnect();
}

TEST(udp_sender_send) {
	UdpSender sender("127.0.0.1", 13417);
	CHECK(sender.isConnected());
	char buf[256];
	osc::OutboundPacketStream msg(buf, sizeof(buf));
	msg << osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << osc::EndMessage;
	CHECK(sender.sendOscPacket(&msg));
}

TEST(tcp_handshake_and_disconnect) {
	// listen-mode receiver, connect-mode sender
	TcpReceiver recv(13418);
	recv.connect();
	CHECK(recv.isConnected());

	TcpSender sender("127.0.0.1", 13418);
	CHECK(sender.isConnected());
	CHECK_EQ(std::string(sender.tuio_type()), "TUIO/TCP");

	// OSC packet delivered end-to-end through the stream framing
	char buf[256];
	osc::OutboundPacketStream msg(buf, sizeof(buf));
	msg << osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << osc::EndMessage;
	CHECK(sender.sendOscPacket(&msg));

	recv.disconnect();
	CHECK(!recv.isConnected());
}

TEST(tcp_sender_listen_mode) {
	TcpSender listener(13419);
	CHECK_EQ(std::string(listener.tuio_type()), "TUIO/TCP");
	TcpReceiver client("127.0.0.1", 13419);
	CHECK(client.isConnected() == false); // not connected until connect()
	client.connect();
	WAIT_FOR(listener.isConnected(), 2000);
	client.disconnect();
}
