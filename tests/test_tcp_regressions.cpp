/*
 Regression tests for the TCP and WebSocket transport lifecycle and wire format.
 These tests use raw POSIX sockets to inspect and produce the exact byte stream.
*/

#include "test_helpers.h"
#include "TuioClient.h"
#include "TcpReceiver.h"
#include "TcpSender.h"
#include "WebSockSender.h"
#include "osc/OscOutboundPacketStream.h"

#include <atomic>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace TUIO;
using namespace TUIO2;

// runs the function in a separate thread and reports whether it returned in time,
// a hanging function is detached so that the remaining tests can still run
template<typename F>
static bool finishesWithin(F func, int timeout_ms) {
	std::atomic<bool> *done = new std::atomic<bool>(false);
	std::thread t([func,done]() mutable { func(); *done = true; });
	bool finished = tuio_test::waitFor([done]{ return done->load(); }, timeout_ms);
	if (finished) { t.join(); delete done; }
	else t.detach();
	return finished;
}

static int rawConnect(int port) {
	int fd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(fd,(struct sockaddr*)&addr,sizeof(addr))<0) { close(fd); return -1; }
	struct timeval tv = {2,0};
	setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
	return fd;
}

static bool recvExact(int fd, char *buf, int len) {
	int got = 0;
	while (got<len) {
		int r = (int)recv(fd,buf+got,len-got,0);
		if (r<=0) return false;
		got += r;
	}
	return true;
}

static std::string recvHttpHeader(int fd) {
	std::string response;
	char c;
	while (response.find("\r\n\r\n")==std::string::npos) {
		if (recv(fd,&c,1,0)<=0) break;
		response += c;
	}
	return response;
}

static bool sendRaw(int fd, const char *data, int len) {
	return send(fd,data,len,0)==len;
}

// RFC 6455 example request, the expected accept key is s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
static const char WS_REQUEST[] =
	"GET / HTTP/1.1\r\n"
	"Host: localhost\r\n"
	"Upgrade: websocket\r\n"
	"Connection: Upgrade\r\n"
	"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
	"Sec-WebSocket-Version: 13\r\n\r\n";

TEST(tcp_sender_listen_teardown) {
	// destroying a listening sender without clients must unblock accept()
	TcpSender *idle = new TcpSender(13430);
	CHECK(finishesWithin([idle]{ delete idle; }, 2000));

	// ... and with a connected client
	TcpSender *sender = new TcpSender(13431);
	TcpReceiver receiver("127.0.0.1",13431);
	receiver.connect();
	WAIT_FOR(sender->isConnected(), 2000);
	CHECK(finishesWithin([sender]{ delete sender; }, 2000));
	CHECK(finishesWithin([&receiver]{ receiver.disconnect(); }, 2000));
}

TEST(tcp_receiver_server_close) {
	TcpSender *sender = new TcpSender(13432);
	TcpReceiver *receiver = new TcpReceiver("127.0.0.1",13432);
	receiver->connect();
	WAIT_FOR(sender->isConnected(), 2000);

	// the server closes first, the client thread closes and releases the main socket
	delete sender;
	WAIT_FOR(receiver->tcp_socket<0, 2000);

	// a new socket will likely reuse the released descriptor number,
	// it must not be shut down or closed by the stale receiver
	int reused = socket(AF_INET,SOCK_STREAM,0);
	CHECK(reused>=0);
	CHECK(finishesWithin([receiver]{ receiver->disconnect(); delete receiver; }, 2000));
	CHECK(fcntl(reused,F_GETFD)!=-1);
	close(reused);
}

TEST(tcp_framing_partial_and_coalesced) {
	TcpReceiver receiver(13433);
	TuioClient client(&receiver);
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();

	int fd = rawConnect(13433);
	CHECK(fd>=0);

	// build three length-prefixed TUIO frames, each triggering a refresh
	std::string frames[3];
	for (int i=0;i<3;i++) {
		char buffer[512];
		osc::OutboundPacketStream bundle(buffer,sizeof(buffer));
		bundle << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio2/frm") << (int32_t)(i+1) << osc::TimeTag(1) << (int32_t)8191 << "tcp@t" << osc::EndMessage
			<< osc::BeginMessage("/tuio2/alv") << osc::EndMessage
			<< osc::EndBundle;
		int size = (int)bundle.Size();
		char prefix[4] = { (char)((size>>24)&0xFF), (char)((size>>16)&0xFF), (char)((size>>8)&0xFF), (char)(size&0xFF) };
		frames[i] = std::string(prefix,4) + std::string(bundle.Data(),size);
	}

	// the first frame is sent byte by byte
	for (size_t i=0;i<frames[0].size();i++) {
		sendRaw(fd,&frames[0][i],1);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	WAIT_FOR(listener.refreshCount==1, 2000);

	// the remaining two frames are sent in a single write
	std::string both = frames[1]+frames[2];
	sendRaw(fd,both.data(),(int)both.size());
	WAIT_FOR(listener.refreshCount==3, 2000);

	close(fd);
	CHECK(finishesWithin([&client]{ client.disconnect(); }, 2000));
}

TEST(tcp_ctor_failure) {
	TcpSender first(13434);
	// a second listener on the same port must throw without leaking
	bool threw = false;
	try { TcpSender second(13434); } catch (std::exception&) { threw = true; }
	CHECK(threw);

	// a receiver that could not bind remains inert
	TcpReceiver receiver(13434);
	CHECK(receiver.tcp_socket<0);
	receiver.connect();
	CHECK(!receiver.isConnected());
}

TEST(websocket_handshake_and_frames) {
	WebSockSender ws(13435);
	CHECK_EQ(std::string(ws.tuio_type()), "TUIO/WEB");

	int fd = rawConnect(13435);
	CHECK(fd>=0);
	sendRaw(fd,WS_REQUEST,(int)strlen(WS_REQUEST));
	std::string response = recvHttpHeader(fd);
	CHECK(response.compare(0,12,"HTTP/1.1 101")==0);
	CHECK(response.find("Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n")!=std::string::npos);
	WAIT_FOR(ws.isConnected(), 2000);

	// OSC messages "/t" with a blob have a size of 12 + blob size,
	// covering the 7-bit, 16-bit and 64-bit WebSocket length encodings
	int blob_sizes[4] = { 112, 116, 65520, 65524 };
	int expected_hs[4] = { 2, 4, 4, 10 };
	std::vector<char> blob(65524, 'x');
	std::vector<char> buffer(70000);
	for (int i=0;i<4;i++) {
		osc::OutboundPacketStream msg(&buffer[0],buffer.size());
		msg << osc::BeginMessage("/t") << osc::Blob(&blob[0],blob_sizes[i]) << osc::EndMessage;
		int len = (int)msg.Size();
		CHECK_EQ(len, 12+blob_sizes[i]);
		CHECK(ws.sendOscPacket(&msg));

		unsigned char header[10];
		CHECK(recvExact(fd,(char*)header,2));
		CHECK_EQ((int)header[0], 0x82);
		int hs = 2;
		long frame_len = header[1];
		if (frame_len==126) {
			hs = 4;
			CHECK(recvExact(fd,(char*)header+2,2));
			frame_len = (header[2]<<8) | header[3];
		} else if (frame_len==127) {
			hs = 10;
			CHECK(recvExact(fd,(char*)header+2,8));
			frame_len = 0;
			for (int b=2;b<10;b++) frame_len = (frame_len<<8) | header[b];
		}
		CHECK_EQ(hs, expected_hs[i]);
		CHECK_EQ(frame_len, (long)len);

		std::vector<char> payload(len);
		CHECK(recvExact(fd,&payload[0],len));
		CHECK(memcmp(&payload[0],msg.Data(),len)==0);
	}
	close(fd);
}

TEST(websocket_handshake_before_frames) {
	WebSockSender ws(13436);

	// a first client keeps the sender connected
	int first = rawConnect(13436);
	sendRaw(first,WS_REQUEST,(int)strlen(WS_REQUEST));
	recvHttpHeader(first);
	WAIT_FOR(ws.isConnected(), 2000);

	// frames are sent continuously while the second client connects
	std::atomic<bool> stop(false);
	std::thread pump([&ws,&stop]{
		char buffer[256];
		while (!stop) {
			osc::OutboundPacketStream msg(buffer,sizeof(buffer));
			msg << osc::BeginMessage("/tuio2/alv") << osc::EndMessage;
			ws.sendOscPacket(&msg);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

	// the second client delays its request, no frame may arrive before the response
	int second = rawConnect(13436);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	sendRaw(second,WS_REQUEST,(int)strlen(WS_REQUEST));
	char start[12];
	CHECK(recvExact(second,start,12));
	CHECK(memcmp(start,"HTTP/1.1 101",12)==0);

	stop = true;
	pump.join();
	close(second);
	close(first);
}

TEST(websocket_rejects_invalid_handshake) {
	WebSockSender ws(13437);
	int fd = rawConnect(13437);
	const char request[] = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	sendRaw(fd,request,(int)strlen(request));
	// the connection is shut down without a response
	char c;
	CHECK(recv(fd,&c,1,0)<=0);
	close(fd);
}
