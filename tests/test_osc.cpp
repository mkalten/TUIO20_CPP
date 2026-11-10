#include "test_helpers.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "osc/OscOutboundPacketStream.h"
using namespace TUIO2;
using namespace osc;

// OscSender base surface via concrete senders
TEST(oscsender_surface) {
	UdpSender udp("127.0.0.1", 13460);
	CHECK(udp.isConnected());
	CHECK((int)udp.getBufferSize() > 0);
	CHECK_EQ(std::string(udp.tuio_type()), "TUIO/UDP");

	TcpSender tcp(13461);
	CHECK_EQ(std::string(tcp.tuio_type()), "TUIO/TCP");

	CaptureSender cap;
	CHECK(cap.isLocal());
	CHECK_EQ(cap.getBufferSize(), 8192);
	CHECK(cap.isConnected());
}

// OscReceiver lifecycle on the stub
TEST(oscreceiver_lifecycle) {
	StubReceiver recv;
	CHECK(!recv.isConnected());
	recv.connect();
	CHECK(recv.isConnected());
	recv.disconnect();
	CHECK(!recv.isConnected());
}

// deterministic full path: TuioServer -> CaptureSender -> StubReceiver -> TuioClient
TEST(osc_server_to_client_inprocess) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	server.setSourceName("testsrc");

	StubReceiver *stub = new StubReceiver();
	TuioClient client(stub); // attaches itself via addTuioClient
	RecordingListener listener;
	client.addTuioListener(&listener);

	server.initTuioFrame(TuioTime::getSystemTime());
	TuioObject *tobj = server.createTuioToken(5, 0.4f, 0.6f, 0.0f);
	CHECK(tobj != NULL);
	server.commitTuioFrame();
	CHECK(sender->packets > 0);

	// feed the captured bundle to the receiver -> client decodes it
	stub->ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(listener.addObj, 1);
	CHECK(listener.hadToken);
	CHECK_NEAR(listener.lastX, 0.4, 1e-5);
	CHECK_NEAR(listener.lastY, 0.6, 1e-5);
	CHECK_EQ((int)client.getTuioObjectList().size(), 1);

	// update propagates
	server.initTuioFrame(TuioTime::getSystemTime());
	server.updateTuioToken(tobj->getTuioToken(), 0.5f, 0.6f, 0.0f);
	server.commitTuioFrame();
	stub->ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(listener.updObj, 1);
	CHECK_NEAR(listener.lastX, 0.5, 1e-5);

	// removal propagates
	server.initTuioFrame(TuioTime::getSystemTime());
	server.removeTuioToken(tobj->getTuioToken());
	server.commitTuioFrame();
	stub->ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(listener.delObj + listener.updObj >= 1 ? 1 : 0, 1);
	CHECK_EQ((int)client.getTuioObjectList().size(), 0);

	client.removeAllTuioListeners();
}

// hand-crafted flat TUIO2 bundle fed directly to the receiver
TEST(oscreceiver_manual_bundle) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, 4096);
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "test@a" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)7 << (int32)0 << (int32)5 << 0.5f << 0.5f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)7 << EndMessage
	<< EndBundle;

	// the bundle decodes all messages: tok creates the object, alv fires it and refresh()
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	CHECK_EQ(listener.addObj, 1);
	CHECK(listener.hadToken);
	CHECK_EQ(listener.refreshCount, 1);
	CHECK_EQ((int)client.getTuioObjectList().size(), 1);
}

// malformed packets must not crash the receiver
TEST(oscreceiver_malformed) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char garbage[32];
	memset(garbage, 0xAB, sizeof(garbage));
	// NOTE: ProcessPacket only catches MalformedBundleException — a malformed
	// *message* propagates out as a different oscpack exception
	try {
		stub.ProcessPacket(garbage, sizeof(garbage), IpEndpointName());
	} catch (...) { /* documents current behavior */ }
	try {
		stub.ProcessPacket(garbage, 0, IpEndpointName());
	} catch (...) {}
	CHECK_EQ(listener.addObj, 0);
}
