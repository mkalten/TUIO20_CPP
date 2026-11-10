#include "test_helpers.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "UdpSender.h"
#include "UdpReceiver.h"
#include "TcpSender.h"
#include "TcpReceiver.h"
using namespace TUIO2;

TEST(client_server_udp_loopback) {
	TuioClient client(13401);
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();
	CHECK(client.isConnected());

	TuioServer server(new UdpSender("127.0.0.1", 13401));
	server.setSourceName("utest");
	server.initTuioFrame(TuioTime::getSystemTime());
	server.createTuioToken(5, 0.4f, 0.6f, 0.5f);
	server.createTuioPointer(0.3f, 0.3f, 0.0f, 0.0f, 0.1f, 0.5f);
	server.commitTuioFrame();

	WAIT_FOR(listener.addObj >= 1, 2000);
	WAIT_FOR(listener.refreshCount >= 1, 2000);

	// client-side lists reflect the received state
	CHECK((int)client.getTuioTokenList().size() >= 1);
	CHECK((int)client.getTuioPointerList().size() >= 1);

	// update propagates over the wire
	TuioObject *tobj = server.getTuioObjectList().front();
	TuioToken *ttok = tobj->getTuioToken();
	server.initTuioFrame(TuioTime::getSystemTime());
	if (ttok) server.updateTuioToken(ttok, 0.7f, 0.7f, 0.5f);
	server.commitTuioFrame();
	WAIT_FOR(listener.updObj >= 1, 2000);
	CHECK_NEAR(listener.lastX, 0.7, 1e-4);

	// removal propagates
	server.initTuioFrame(TuioTime::getSystemTime());
	if (ttok) server.removeTuioToken(ttok);
	server.commitTuioFrame();
	WAIT_FOR(listener.delObj >= 1, 2000);

	client.disconnect();
	CHECK(!client.isConnected());
}

TEST(client_server_tcp_loopback) {
	// listen-mode sender + connect-mode receiver
	TcpSender *sender = new TcpSender(13405);
	TuioServer server(sender);
	server.setSourceName("ttest");
	TuioClient client(new TcpReceiver("127.0.0.1", 13405));
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();

	// wait for the accept before sending — packets sent to an empty
	// client list are silently dropped
	WAIT_FOR(sender->isConnected(), 2000);

	server.initTuioFrame(TuioTime::getSystemTime());
	server.createTuioToken(7, 0.2f, 0.8f, 0.0f);
	server.commitTuioFrame();

	WAIT_FOR(listener.addObj >= 1, 3000);
	CHECK(listener.hadToken);
	CHECK_NEAR(listener.lastX, 0.2, 1e-4);
	CHECK_NEAR(listener.lastY, 0.8, 1e-4);

	client.disconnect();
}

TEST(sender_packet_bounds) {
	char buf[128];
	// a packet stream with no data must be rejected
	osc::OutboundPacketStream empty(buf, sizeof(buf));
	CHECK_EQ((int)empty.Size(), 0);

	UdpSender udp("127.0.0.1", 13411);
	CHECK(udp.isConnected());
	CHECK(!udp.sendOscPacket(&empty)); // Size()==0 -> false

	osc::OutboundPacketStream msg(buf, sizeof(buf));
	msg << osc::BeginMessage("/tuio2/alv") << osc::EndMessage;
	CHECK(udp.sendOscPacket(&msg));

	// a packet larger than the sender's buffer size is rejected
	// (loopback allows up to 65507 = MAX_LOCAL_UDP_SIZE)
	std::vector<char> bigbuf(66000);
	std::vector<char> blob(65540, 'x');
	osc::OutboundPacketStream oversized(&bigbuf[0], bigbuf.size());
	oversized << osc::BeginMessage("/t") << osc::Blob(&blob[0], 65540) << osc::EndMessage;
	CHECK((int)oversized.Size() > 65507);
	CHECK(!udp.sendOscPacket(&oversized));
}
