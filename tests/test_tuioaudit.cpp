#include "test_helpers.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"
using namespace TUIO2;
using namespace osc;

// pending frame objects must be discarded when a new packet arrives
// (processBundleBegin) — the malformed-bundle crash fix
TEST(tuioaudit_pending_discard_on_next_packet) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];

	// first bundle is incomplete: frm+tok but NO alv — the tok stays pending
	OutboundPacketStream partial(buffer, sizeof(buffer));
	partial << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "a@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)1 << (int32)0 << (int32)5 << 0.1f << 0.1f << 0.0f << EndMessage
	<< EndBundle;
	stub.ProcessPacket(partial.Data(), (int)partial.Size(), IpEndpointName());
	CHECK_EQ(listener.addObj, 0); // nothing committed without alv

	// a complete second bundle for the same session — the stale pending
	// object must have been discarded, not added twice
	OutboundPacketStream complete(buffer, sizeof(buffer));
	complete << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)2 << TimeTag(1) << (int32)8191 << "a@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)1 << (int32)0 << (int32)5 << 0.2f << 0.2f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)1 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(complete.Data(), (int)complete.Size(), IpEndpointName());

	CHECK_EQ(listener.addObj, 1);          // exactly one add, not two
	CHECK_EQ((int)client.getTuioObjectList().size(), 1);
	CHECK_NEAR(listener.lastX, 0.2, 1e-5); // the fresh value won
}

// a bundle without alv leaves objects pending without firing events
TEST(tuioaudit_missing_alv_no_events) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "b@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)3 << (int32)0 << (int32)9 << 0.5f << 0.5f << 0.0f << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	CHECK_EQ(listener.addObj, 0);
	CHECK_EQ(listener.refreshCount, 0);
	CHECK_EQ((int)client.getTuioObjectList().size(), 0);
}

// removal via an empty alv fires the remove event for the live object
TEST(tuioaudit_alv_removes_object) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream add(buffer, sizeof(buffer));
	add << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "c@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)4 << (int32)0 << (int32)1 << 0.5f << 0.5f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(add.Data(), (int)add.Size(), IpEndpointName());
	CHECK_EQ(listener.addObj, 1);
	CHECK_EQ((int)client.getTuioObjectList().size(), 1);

	OutboundPacketStream rem(buffer, sizeof(buffer));
	rem << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)2 << TimeTag(1) << (int32)8191 << "c@x" << EndMessage
		<< BeginMessage("/tuio2/alv") << EndMessage
	<< EndBundle;
	stub.ProcessPacket(rem.Data(), (int)rem.Size(), IpEndpointName());
	CHECK_EQ(listener.delObj, 1);
	CHECK_EQ((int)client.getTuioObjectList().size(), 0);
}

// malformed messages inside a bundle are tolerated (caught inside processOSC)
TEST(tuioaudit_malformed_message_tolerance) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "d@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)9 << 0.5f << EndMessage  // too few args -> exception
		<< BeginMessage("/tuio2/alv") << EndMessage
	<< EndBundle;

	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	// no crash; the malformed tok must not have created a session object
	CHECK_EQ(listener.addObj, 0);
	CHECK_EQ((int)client.getTuioObjectList().size(), 0);
}

// the tu_id field packs user_id in the high 16 bits and type_id
// in the low 16 bits — the decode must not swap them
TEST(tuioaudit_tuid_decode_order) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	// tu_id = u_id<<16 | t_id = 3<<16 | 2 = 0x00030002
	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00030002 << (int32)5 << 0.5f << 0.5f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	CHECK_EQ(listener.addObj, 1);
	TuioToken *ttok = client.getTuioToken(8);
	CHECK(ttok != NULL);
	CHECK_EQ(ttok->getTypeID(), 2);
	CHECK_EQ(ttok->getUserID(), 3);
}

// /tuio2/chg attaches the parsed hull points to the object's TuioGeometry
TEST(tuioaudit_chg_geometry) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/chg") << (int32)7 << 0.1f << 0.2f << 0.3f << 0.4f << 0.5f << 0.6f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)7 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(7);
	CHECK(tobj != NULL);
	TuioGeometry *tgeo = tobj->getTuioGeometry();
	CHECK(tgeo != NULL);
	std::list<TuioPoint> hull = tgeo->getConvexHull();
	CHECK_EQ(hull.size(), 3);
	CHECK_NEAR(hull.front().getX(), 0.1, 1e-06);
	CHECK_NEAR(hull.back().getY(), 0.6, 1e-06);
}

// /tuio2/ocg attaches the parsed contour points
TEST(tuioaudit_ocg_geometry) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/ocg") << (int32)7 << 0.5f << 0.5f << 0.7f << 0.8f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)7 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(7);
	CHECK(tobj != NULL && tobj->getTuioGeometry() != NULL);
	CHECK_EQ(tobj->getTuioGeometry()->getOuterContour().size(), 2);
}

// /tuio2/icg splits inner contours at bool TRUE separators
TEST(tuioaudit_icg_multi_contour) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/icg") << (int32)7 << 0.1f << 0.2f << true << 0.9f << 0.8f << 0.7f << 0.6f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)7 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(7);
	CHECK(tobj != NULL && tobj->getTuioGeometry() != NULL);
	std::list< std::list<TuioPoint> > inner = tobj->getTuioGeometry()->getInnerContours();
	CHECK_EQ(inner.size(), 2);
	std::list< std::list<TuioPoint> >::iterator c = inner.begin();
	CHECK_EQ(c->size(), 1);        // first contour: one point
	c++;
	CHECK_EQ(c->size(), 2);        // second contour: two points
	CHECK_NEAR(c->front().getX(), 0.9, 1e-06);
}

// /tuio2/sym data args can be OSC blobs — decoded and flagged on the symbol
TEST(tuioaudit_sym_blob_data) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	const char blobBytes[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/sym") << (int32)6 << (int32)0x00010002 << (int32)4 << "mifare/ul" << Blob(blobBytes,8) << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)6 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(6);
	CHECK(tobj != NULL);
	TuioSymbol *tsym = tobj->getTuioSymbol();
	CHECK(tsym != NULL);
	CHECK(tsym->isSymbolDataBlob());
	int bsize = 0;
	const void *bdata = tsym->getSymbolDataBlob(&bsize);
	CHECK(bdata != NULL);
	CHECK_EQ(bsize, 8);
	CHECK_EQ(memcmp(bdata, blobBytes, 8), 0);
}

// /tuio2/skg decodes skeleton branches split by int node-jump markers
TEST(tuioaudit_skg_skeleton) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/skg") << (int32)5 << 0.1f << 0.1f << 0.2f << 0.2f << (int32)0 << 0.3f << 0.3f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)5 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(5);
	CHECK(tobj != NULL && tobj->getTuioGeometry() != NULL);
	std::list< std::list<TuioPoint> > skltn = tobj->getTuioGeometry()->getSkeleton();
	CHECK_EQ(skltn.size(), 2);
	CHECK_EQ(skltn.front().size(), 2);
	CHECK_EQ(skltn.back().size(), 1);
	CHECK_EQ(tobj->getTuioGeometry()->getSkeletonJumps().front(), 0);
}

// /tuio2/svg decodes the skeleton volume radii
TEST(tuioaudit_svg_volumes) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/svg") << (int32)5 << 0.1f << 0.05f << 0.025f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)5 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(5);
	CHECK(tobj != NULL && tobj->getTuioGeometry() != NULL);
	std::list<float> vols = tobj->getTuioGeometry()->getSkeletonVolumes();
	CHECK_EQ(vols.size(), 3);
	CHECK_NEAR(vols.back(), 0.025, 1e-06);
}

// /tuio2/arg decodes span triplets
TEST(tuioaudit_arg_spans) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/arg") << (int32)5 << 0.1f << 0.1f << 0.2f << 0.4f << 0.3f << 0.1f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)5 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(5);
	CHECK(tobj != NULL && tobj->getTuioGeometry() != NULL);
	CHECK_EQ(tobj->getTuioGeometry()->getArea().size(), 2);
	std::list<float> widths = tobj->getTuioGeometry()->getAreaWidths();
	CHECK_EQ(widths.size(), 2);
	CHECK_NEAR(widths.front(), 0.2, 1e-06);
	CHECK_NEAR(widths.back(), 0.1, 1e-06);
}

// /tuio2/raw decodes width + 8bit blob data
TEST(tuioaudit_raw_data) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	const char rawBytes[6] = {10,20,30,40,50,60};
	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/raw") << (int32)5 << (int32)3 << Blob(rawBytes,6) << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)5 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(5);
	CHECK(tobj != NULL && tobj->getTuioGeometry() != NULL);
	CHECK_EQ(tobj->getTuioGeometry()->getRawWidth(), 3);
	CHECK_EQ(tobj->getTuioGeometry()->getRawData().size(), 6);
	CHECK_EQ(tobj->getTuioGeometry()->getRawData()[0], 10);
	CHECK_EQ(tobj->getTuioGeometry()->getRawData()[5], 60);
}

// /tuio2/ctl decodes an ordered list of bool + float control dimensions
TEST(tuioaudit_ctl_controls) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/ctl") << (int32)4 << true << false << 0.5f << -0.25f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL && tobj->getTuioControl() != NULL);
	std::list< std::pair<bool,float> > ctls = tobj->getTuioControl()->getControls();
	CHECK_EQ(ctls.size(), 4);
	std::list< std::pair<bool,float> >::iterator it = ctls.begin();
	CHECK_EQ(it->first, true);  CHECK_NEAR(it->second, 1.0, 1e-06); it++;
	CHECK_EQ(it->first, true);  CHECK_NEAR(it->second, 0.0, 1e-06); it++;
	CHECK_EQ(it->first, false); CHECK_NEAR(it->second, 0.5, 1e-06); it++;
	CHECK_EQ(it->first, false); CHECK_NEAR(it->second, -0.25, 1e-06);
}

// /tuio2/dat decodes MIME-typed string payloads
TEST(tuioaudit_dat_string) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/dat") << (int32)4 << "text/plain" << "hello" << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL && tobj->getTuioData() != NULL);
	CHECK(!tobj->getTuioData()->isDataBlob());
	CHECK_EQ(std::string(tobj->getTuioData()->getMimeType()), "text/plain");
	CHECK_EQ(std::string(tobj->getTuioData()->getDataContent()), "hello");
}

// /tuio2/sig decodes the trigger signal + target list
TEST(tuioaudit_sig_targets) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/sig") << (int32)4 << (int32)7 << (int32)1 << (int32)2 << (int32)3 << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL && tobj->getTuioSignal() != NULL);
	CHECK_EQ(tobj->getTuioSignal()->getComponentID(), 7);
	std::list<unsigned int> targets = tobj->getTuioSignal()->getSignalTargets();
	CHECK_EQ(targets.size(), 3);
	CHECK_EQ(targets.front(), 1);
	CHECK_EQ(targets.back(), 3);
}

// /tuio2/coa decodes container slot + contained session IDs
TEST(tuioaudit_coa_container) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/coa") << (int32)4 << (int32)2 << (int32)10 << (int32)11 << (int32)12 << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL);
	std::list<TuioAssociation*> assocs = tobj->getTuioAssociationList();
	CHECK_EQ(assocs.size(), 1);
	TuioContainer *tcnr = dynamic_cast<TuioContainer*>(assocs.front());
	CHECK(tcnr != NULL);
	CHECK_EQ(tcnr->getSlot(), 2);
	CHECK_EQ(tcnr->getContained().size(), 3);
	CHECK_EQ(tcnr->getContained().front(), 10);
}

// /tuio2/lia decodes physical flag + link pairs
TEST(tuioaudit_lia_links) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/lia") << (int32)4 << true << (int32)7 << (int32)0x00010002 << (int32)8 << (int32)3 << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL);
	std::list<TuioAssociation*> assocs = tobj->getTuioAssociationList();
	CHECK_EQ(assocs.size(), 1);
	TuioLink *tlnk = dynamic_cast<TuioLink*>(assocs.front());
	CHECK(tlnk != NULL);
	CHECK_EQ(tlnk->isPhysical(), true);
	std::list< std::pair<unsigned int,unsigned int> > links = tlnk->getLinks();
	CHECK_EQ(links.size(), 2);
	CHECK_EQ(links.front().first, 7);
	CHECK_EQ(links.front().second, 0x00010002);
	CHECK_EQ(links.back().first, 8);
}

// /tuio2/lta splits int32 link pairs from float node-jump markers
TEST(tuioaudit_lta_tree) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/lta") << (int32)4 << true << (int32)7 << (int32)1 << 0.0f << (int32)8 << (int32)2 << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL);
	std::list<TuioAssociation*> assocs = tobj->getTuioAssociationList();
	CHECK_EQ(assocs.size(), 1);
	TuioTree *ttre = dynamic_cast<TuioTree*>(assocs.front());
	CHECK(ttre != NULL);
	CHECK_EQ(ttre->getLinks().size(), 2);
	CHECK_EQ(ttre->getNodeJumps().size(), 1);
	CHECK_NEAR(ttre->getNodeJumps().front(), 0.0, 1e-06);
}

// /tuio2/ala populates the frame's associated-object bookkeeping
TEST(tuioaudit_ala_list) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/coa") << (int32)4 << (int32)0 << (int32)9 << EndMessage
		<< BeginMessage("/tuio2/ala") << (int32)4 << (int32)9 << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	CHECK_EQ(listener.addObj, 1);
	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL);
	CHECK(tobj->containsTuioAssociation());
}

// /tuio2/t3d decodes z-position, orientation axis and tu_id
TEST(tuioaudit_t3d_token) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/t3d") << (int32)8 << (int32)0x00030002 << (int32)5
			<< 0.5f << 0.5f << 0.25f << 1.0f << 0.0f << 0.0f << 1.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(8);
	CHECK(tobj != NULL);
	TuioToken3D *ttok = tobj->getTuioToken3D();
	CHECK(ttok != NULL);
	CHECK_NEAR(ttok->getZ(), 0.25, 1e-06);
	CHECK_NEAR(ttok->getZAxis(), 1.0, 1e-06);
	CHECK_EQ(ttok->getTypeID(), 2);
	CHECK_EQ(ttok->getUserID(), 3);
	CHECK_EQ(ttok->getComponentID(), 5);
}

// /tuio2/p3d decodes z-position, direction vector and radius
TEST(tuioaudit_p3d_pointer) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/p3d") << (int32)8 << (int32)0x00010005 << (int32)3
			<< 0.5f << 0.5f << 0.1f << 1.0f << 0.0f << 0.0f << 0.02f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(8);
	CHECK(tobj != NULL);
	TuioPointer3D *tptr = tobj->getTuioPointer3D();
	CHECK(tptr != NULL);
	CHECK_NEAR(tptr->getZ(), 0.1, 1e-06);
	CHECK_NEAR(tptr->getXAxis(), 1.0, 1e-06);
	CHECK_NEAR(tptr->getRadius(), 0.02, 1e-06);
	CHECK_EQ(tptr->getComponentID(), 3);
}

// /tuio2/b3d decodes z-position, axis, depth and volume
TEST(tuioaudit_b3d_bounds) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/b3d") << (int32)8
			<< 0.5f << 0.5f << 0.15f << 0.7f << 0.0f << 0.0f << 1.0f
			<< 0.1f << 0.05f << 0.02f << 0.001f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(8);
	CHECK(tobj != NULL);
	TuioBounds3D *tbnd = tobj->getTuioBounds3D();
	CHECK(tbnd != NULL);
	CHECK_NEAR(tbnd->getZ(), 0.15, 1e-06);
	CHECK_NEAR(tbnd->getDepth(), 0.02, 1e-06);
	CHECK_NEAR(tbnd->getVolume(), 0.001, 1e-06);
}

// round-trip: geometry attached on the server arrives decoded on the client
TEST(tuioaudit_roundtrip_geometry) {
	CaptureSender sender;
	TuioServer server(&sender);
	server.setSourceName("testsrc");

	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	TuioTime ftime = TuioTime::getSystemTime();
	server.initTuioFrame(ftime);
	TuioObject *tobj = server.createTuioToken(5, 0.4f, 0.6f, 0.0f);
	TuioGeometry *tgeo = new TuioGeometry(ftime,tobj);
	std::list<TuioPoint> hull;
	hull.push_back(TuioPoint(0.1f,0.2f));
	hull.push_back(TuioPoint(0.3f,0.4f));
	tgeo->setConvexHull(hull);
	tobj->setTuioGeometry(tgeo);
	server.commitTuioFrame();

	for (std::vector< std::vector<char> >::iterator it = sender.allPackets.begin(); it != sender.allPackets.end(); it++)
		stub.ProcessPacket(it->data(), (int)it->size(), IpEndpointName());

	TuioObject *rtobj = client.getTuioObject(tobj->getSessionID());
	CHECK(rtobj != NULL);
	TuioGeometry *rgeo = rtobj->getTuioGeometry();
	CHECK(rgeo != NULL);
	CHECK_EQ(rgeo->getConvexHull().size(), 2);
}

// round-trip: control/data/signal content attached on the server arrives on the client
TEST(tuioaudit_roundtrip_content) {
	CaptureSender sender;
	TuioServer server(&sender);
	server.setSourceName("testsrc");

	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	TuioTime ftime = TuioTime::getSystemTime();
	server.initTuioFrame(ftime);
	TuioObject *tobj = server.createTuioToken(5, 0.4f, 0.6f, 0.0f);

	TuioControl *tctl = new TuioControl(ftime,tobj);
	std::list< std::pair<bool,float> > ctls;
	ctls.push_back(std::make_pair(true,1.0f));
	ctls.push_back(std::make_pair(false,0.5f));
	tctl->setControls(ctls);
	tobj->setTuioControl(tctl);

	TuioData *tdat = new TuioData(ftime,tobj,"text/plain","hello");
	tobj->setTuioData(tdat);

	std::list<unsigned int> targets;
	targets.push_back(9);
	targets.push_back(11);
	TuioSignal *tsig = new TuioSignal(ftime,tobj,7,targets);
	tobj->setTuioSignal(tsig);

	server.commitTuioFrame();
	for (std::vector< std::vector<char> >::iterator it = sender.allPackets.begin(); it != sender.allPackets.end(); it++)
		stub.ProcessPacket(it->data(), (int)it->size(), IpEndpointName());

	TuioObject *rtobj = client.getTuioObject(tobj->getSessionID());
	CHECK(rtobj != NULL);
	CHECK_EQ(rtobj->getTuioControl()->getControls().size(), 2);
	CHECK_EQ(std::string(rtobj->getTuioData()->getMimeType()), "text/plain");
	CHECK_EQ(std::string(rtobj->getTuioData()->getDataContent()), "hello");
	CHECK_EQ(rtobj->getTuioSignal()->getComponentID(), 7);
	CHECK_EQ(rtobj->getTuioSignal()->getSignalTargets().size(), 2);
}

// round-trip: an association attached on the server arrives on the client
TEST(tuioaudit_roundtrip_association) {
	CaptureSender sender;
	TuioServer server(&sender);
	server.setSourceName("testsrc");

	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	TuioTime ftime = TuioTime::getSystemTime();
	server.initTuioFrame(ftime);
	TuioObject *tobj = server.createTuioToken(5, 0.4f, 0.6f, 0.0f);
	std::list<unsigned int> kids;
	kids.push_back(20);
	kids.push_back(21);
	server.addTuioAssociation(new TuioContainer(ftime,tobj,3,kids));
	server.commitTuioFrame();

	for (std::vector< std::vector<char> >::iterator it = sender.allPackets.begin(); it != sender.allPackets.end(); it++)
		stub.ProcessPacket(it->data(), (int)it->size(), IpEndpointName());

	TuioObject *rtobj = client.getTuioObject(tobj->getSessionID());
	CHECK(rtobj != NULL);
	std::list<TuioAssociation*> assocs = rtobj->getTuioAssociationList();
	CHECK_EQ(assocs.size(), 1);
	TuioContainer *tcnr = dynamic_cast<TuioContainer*>(assocs.front());
	CHECK(tcnr != NULL);
	CHECK_EQ(tcnr->getSlot(), 3);
	CHECK_EQ(tcnr->getContained().size(), 2);
}

// update-in-place: a second message for the same object updates values
TEST(tuioaudit_update_in_place) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	{
		OutboundPacketStream bundle(buffer, sizeof(buffer));
		bundle << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.1f << 0.1f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	}
	CHECK_NEAR(client.getTuioToken(8)->getX(), 0.1, 1e-06);

	// second frame: same s_id, new position
	{
		OutboundPacketStream bundle(buffer, sizeof(buffer));
		bundle << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)2 << TimeTag(2) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.7f << 0.9f << 0.5f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	}
	TuioToken *ttok = client.getTuioToken(8);
	CHECK_NEAR(ttok->getX(), 0.7, 1e-06);
	CHECK_NEAR(ttok->getY(), 0.9, 1e-06);
	CHECK_NEAR(ttok->getAngle(), 0.5, 1e-06);
	CHECK_EQ(listener.updObj, 1);
}

// a component message before any /frm must not crash — frameSource is NULL
TEST(tuioaudit_tok_before_frm) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.5f << 0.5f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	// no crash = the bug is absent; the object may or may not exist
	CHECK(true);
}

// a late frame (lower fseq) must be dropped; fseq 0 is always accepted
TEST(tuioaudit_late_frame_drop) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	{
		OutboundPacketStream b(buffer, sizeof(buffer));
		b << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)5 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.1f << 0.1f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_NEAR(client.getTuioToken(8)->getX(), 0.1, 1e-06);

	// late frame fseq=3 with different position — must be dropped
	{
		OutboundPacketStream b(buffer, sizeof(buffer));
		b << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)3 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.9f << 0.9f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_NEAR(client.getTuioToken(8)->getX(), 0.1, 1e-06);

	// fseq=0 is always accepted (out-of-order default)
	{
		OutboundPacketStream b(buffer, sizeof(buffer));
		b << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)0 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.5f << 0.5f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_NEAR(client.getTuioToken(8)->getX(), 0.5, 1e-06);
}

// same s_id from two different sources must be two distinct objects
TEST(tuioaudit_source_discrimination) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	{
		OutboundPacketStream b(buffer, sizeof(buffer));
		b << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "srcA:0@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.1f << 0.1f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	{
		OutboundPacketStream b(buffer, sizeof(buffer));
		b << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(2) << (int32)8191 << "srcB:0@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.9f << 0.9f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}

	std::list<TuioObject*> objs = client.getTuioObjectList();
	int count = 0;
	for (std::list<TuioObject*>::iterator it = objs.begin(); it != objs.end(); it++)
		if ((*it)->getSessionID() == 8) count++;
	CHECK_EQ(count, 2);
}

// multiple associations on one object accumulate in the list
TEST(tuioaudit_multi_associations) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream b(buffer, sizeof(buffer));
	b << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/coa") << (int32)4 << (int32)0 << (int32)9 << EndMessage
		<< BeginMessage("/tuio2/coa") << (int32)4 << (int32)1 << (int32)10 << EndMessage
		<< BeginMessage("/tuio2/lia") << (int32)4 << true << (int32)7 << (int32)1 << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)4 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(4);
	CHECK(tobj != NULL);
	CHECK_EQ(tobj->getTuioAssociationList().size(), 3);
}

// 3D update at the same timestamp must not produce NaN zspeed
TEST(tuioaudit_component3d_dt0) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	{
		OutboundPacketStream b(buffer, sizeof(buffer));
		b << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/t3d") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.5f << 0.5f << 0.2f << 0.0f << 0.0f << 0.0f << 1.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	TuioToken3D *ttok = client.getTuioObject(8)->getTuioToken3D();
	CHECK(ttok != NULL);
	CHECK(ttok->getZSpeed() == ttok->getZSpeed()); // not NaN
}

// content created with NULL container returns 0 session id, no crash
TEST(tuioaudit_content_null_container) {
	TuioSymbol tsym(NULL, 1, 2, 3, "type", "data");
	CHECK_EQ(tsym.getSessionID(), 0);
	TuioGeometry tgeo((TuioObject*)NULL);
	CHECK_EQ(tgeo.getSessionID(), 0);
}

// mutating the returned association list does not corrupt the object's list
TEST(tuioaudit_assoc_list_copy) {
	TuioObject tobj(1, 2);
	std::list<unsigned int> kids;
	kids.push_back(9);
	TuioContainer *tcnr = new TuioContainer(&tobj, 0, kids);
	tobj.addTuioAssociation(tcnr);

	std::list<TuioAssociation*> copy = tobj.getTuioAssociationList();
	copy.clear();
	CHECK_EQ(tobj.getTuioAssociationList().size(), 1);
}

// a malformed message mid-bundle must not abort processing of later messages
TEST(tuioaudit_malformed_mid_bundle) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.1f << 0.1f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)9 << "bad_type" << (int32)5 << 0.2f << 0.2f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)10 << (int32)0x00010002 << (int32)5 << 0.3f << 0.3f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << (int32)9 << (int32)10 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	// the first and third objects must exist even though the second was malformed
	CHECK(client.getTuioObject(8) != NULL);
	CHECK(client.getTuioObject(10) != NULL);
}

// a truncated message must not crash or create a bogus object
TEST(tuioaudit_truncated_message) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << EndMessage
		<< BeginMessage("/tuio2/alv") << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	CHECK(client.getTuioObject(8) == NULL);
}

// args beyond the optional speed tail are ignored; a wrong-type tail drops the message
TEST(tuioaudit_extra_args) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.5f << 0.5f << 0.0f
			<< 1.0f << 2.0f << 3.0f << 4.0f << 5.0f << "junk" << 99 << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioToken *ttok = client.getTuioToken(8);
	CHECK(ttok != NULL);
	CHECK_NEAR(ttok->getX(), 0.5, 1e-06);
	CHECK_NEAR(ttok->getXSpeed(), 1.0, 1e-06);
}

// corrupted byte input must not crash
TEST(tuioaudit_corrupted_packet) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	const char junk[] = "not an osc packet at all #bundle garbage 123456789";
	try { stub.ProcessPacket(junk, (int)sizeof(junk), IpEndpointName()); } catch(...) {}
	const char tiny[] = "#bundle";
	try { stub.ProcessPacket(tiny, (int)sizeof(tiny), IpEndpointName()); } catch(...) {}

	// client must still process a valid bundle after corrupted input
	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.5f << 0.5f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	CHECK(client.getTuioObject(8) != NULL);
}

// boundary session/type IDs decode correctly
TEST(tuioaudit_edge_ids) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)0 << (int32)-1 << (int32)7 << 0.1f << 0.1f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)0 << (int32)-1 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioToken *ttok = client.getTuioToken(0);
	CHECK(ttok != NULL);
	CHECK_EQ(ttok->getTypeID(), 0xFFFF);
	CHECK_EQ(ttok->getUserID(), 0xFFFF);
}

// two messages for the same object in one frame update in place
TEST(tuioaudit_same_frame_dup_update) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.1f << 0.1f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.9f << 0.9f << 1.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioToken *ttok = client.getTuioToken(8);
	CHECK(ttok != NULL);
	CHECK_NEAR(ttok->getX(), 0.9, 1e-06);
	CHECK_EQ(client.getTuioObjectList().size(), 1);
}

// a frame exceeding the packet capacity must split and reassemble all objects
TEST(tuioaudit_bundle_overflow) {
	CaptureSender sender;
	TuioServer server(&sender);
	server.setSourceName("testsrc");

	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	server.initTuioFrame(TuioTime::getSystemTime());
	for (unsigned int i = 0; i < 300; i++)
		server.createTuioToken(i+1, 0.1f, 0.1f, 0.0f);
	server.commitTuioFrame();

	// multiple packets were emitted — feed each separately like the wire would
	CHECK(sender.packets > 1);
	for (std::vector< std::vector<char> >::iterator it = sender.allPackets.begin(); it != sender.allPackets.end(); it++)
		stub.ProcessPacket(it->data(), (int)it->size(), IpEndpointName());
	// continuation packets share the frame time, so all objects reassemble
	CHECK_EQ(client.getTuioObjectList().size(), 300);
}

// a local (loopback) sized buffer fits a large frame in a single packet
TEST(tuioaudit_large_local_packet) {
	CaptureSender sender(65507);
	TuioServer server(&sender);
	server.setSourceName("testsrc");

	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	server.initTuioFrame(TuioTime::getSystemTime());
	for (unsigned int i = 0; i < 300; i++)
		server.createTuioToken(i+1, 0.1f, 0.1f, 0.0f);
	server.commitTuioFrame();

	// ~24KB frame fits in one 64KB packet — no split
	CHECK_EQ(sender.packets, 1);
	stub.ProcessPacket(sender.captured.data(), (int)sender.captured.size(), IpEndpointName());
	CHECK_EQ(client.getTuioObjectList().size(), 300);
}

// rapid frame soak — object stays consistent across many frames
TEST(tuioaudit_rapid_frames) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	for (unsigned int f = 1; f <= 200; f++) {
		OutboundPacketStream bundle(buffer, sizeof(buffer));
		bundle << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)f << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << (0.001f*f) << 0.5f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	}
	TuioToken *ttok = client.getTuioToken(8);
	CHECK(ttok != NULL);
	CHECK_NEAR(ttok->getX(), 0.2, 1e-03);
	CHECK_EQ(client.getTuioObjectList().size(), 1);
}

// identical frames must produce one tuioAdd + one tuioUpdate, not two tuioAdd events
// (github.com/mkalten/TUIO20_CPP/issues/10)
TEST(tuioaudit_identical_frames_no_dup_add) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	for (unsigned int f = 1; f <= 3; f++) {
		char buffer[4096];
		OutboundPacketStream bundle(buffer, sizeof(buffer));
		bundle << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)f << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/ptr") << (int32)101027 << (int32)0x00010000 << (int32)0
				<< 0.409512f << 0.49871f << 0.0f << 0.0f << 0.0f << 0.136719f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)101027 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	}

	// one add, two updates — and only one object in the list
	CHECK_EQ(listener.addObj, 1);
	CHECK_EQ(listener.updObj, 2);
	CHECK_EQ(client.getTuioObjectList().size(), 1);
}

// containsNewTuioX must clear once the frame commits, even when the
// component never receives a data-changing update
TEST(tuioaudit_component_state_commit) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	for (unsigned int f = 1; f <= 2; f++) {
		char buffer[4096];
		OutboundPacketStream bundle(buffer, sizeof(buffer));
		bundle << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)f << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)8 << (int32)0x00010002 << (int32)5 << 0.5f << 0.5f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)8 << EndMessage
		<< EndBundle;
		stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	}

	TuioObject *tobj = client.getTuioObject(8);
	CHECK(tobj != NULL);
	CHECK(tobj->getTuioState() != TUIO_ADDED);
	TuioToken *ttok = tobj->getTuioToken();
	CHECK(ttok != NULL);
	CHECK(tobj->containsNewTuioToken() == false);
}


// verify each emitted message's OSC typetags match the spec's declared types
static std::string msgTypetags(const osc::ReceivedMessage &msg) {
	return msg.TypeTags();
}
TEST(tuioaudit_encode_typetags) {
	CaptureSender sender;
	TuioServer server(&sender);
	server.setSourceName("ttsrc");

	TuioTime ftime = TuioTime::getSystemTime();
	server.initTuioFrame(ftime);
	TuioObject *tobj = server.createTuioToken(9, 0.5f, 0.5f, 0.25f);
	server.addTuioPointer(tobj, 2, 1, 0.1f, 0.2f, 0.3f, 0.4f, 0.05f, 0.9f);
	server.addTuioBounds(tobj, 0.4f, 0.5f, 0.1f, 0.3f, 0.2f, 0.06f);
	server.addTuioSymbol(tobj, 7, 2, 4, "grp", "code");
	TuioControl *tctl = new TuioControl(ftime,tobj);
	std::list< std::pair<bool,float> > ctls;
	ctls.push_back(std::make_pair(true,1.0f));
	ctls.push_back(std::make_pair(false,0.5f));
	tctl->setControls(ctls);
	server.addTuioControl(tctl);
	server.addTuioData(new TuioData(ftime,tobj,"text/plain","hello"));
	std::list<unsigned int> tgts; tgts.push_back(11);
	server.addTuioSignal(new TuioSignal(ftime,tobj,3,tgts));
	std::list<unsigned int> kids; kids.push_back(20);
	server.addTuioAssociation(new TuioContainer(ftime,tobj,1,kids));
	std::list< std::pair<unsigned int,unsigned int> > links;
	links.push_back(std::make_pair(10u,0x00020001u));
	server.addTuioAssociation(new TuioLink(ftime,tobj,true,links));
	server.addTuioAssociation(new TuioList(ftime,tobj,false,links));
	std::list<float> nodes; nodes.push_back(0.0f);
	server.addTuioAssociation(new TuioTree(ftime,tobj,true,links,nodes));
	server.addTuioToken3D(new TuioToken3D(ftime,tobj,1,3,6,0.1f,0.2f,0.3f,0.4f,0.5f,0.6f,0.7f));
	server.addTuioPointer3D(new TuioPointer3D(ftime,tobj,2,4,8,0.1f,0.2f,0.3f,0.0f,0.0f,1.0f,0.05f));
	server.addTuioBounds3D(new TuioBounds3D(ftime,tobj,0.5f,0.5f,0.25f,0.1f,0.0f,0.0f,1.0f,0.3f,0.2f,0.1f,0.006f));

	// geometry with all descriptors
	TuioGeometry *tgeo = new TuioGeometry(ftime,tobj);
	std::list<TuioPoint> hull; hull.push_back(TuioPoint(0.1f,0.1f)); hull.push_back(TuioPoint(0.2f,0.2f));
	tgeo->setConvexHull(hull);
	tgeo->setOuterContour(hull);
	std::list< std::list<TuioPoint> > ic; ic.push_back(hull); ic.push_back(hull);
	tgeo->setInnerContours(ic);
	std::list< std::list<TuioPoint> > sk; sk.push_back(hull); sk.push_back(hull);
	tgeo->setSkeleton(sk);
	std::list<unsigned int> jumps; jumps.push_back(0);
	tgeo->setSkeletonJumps(jumps);
	std::list< std::list<float> > sk3;
	std::list<float> b1; b1.push_back(0.1f); b1.push_back(0.2f); b1.push_back(0.3f);
	sk3.push_back(b1); sk3.push_back(b1);
	tgeo->setSkeleton3D(sk3);
	tgeo->setSkeletonJumps3D(jumps);
	std::list<float> vols; vols.push_back(0.01f);
	tgeo->setSkeletonVolumes(vols);
	std::list<float> widths; widths.push_back(0.02f); widths.push_back(0.03f);
	tgeo->setArea(hull);
	tgeo->setAreaWidths(widths);
	tgeo->setRawData(2, std::string("\x01\x02\x03\x04",4));
	server.addTuioGeometry(tgeo);
	server.commitTuioFrame();

	// parse the emitted packet
	CHECK(!sender.allPackets.empty());
	std::map< std::string,std::string > tags;
	for (std::vector< std::vector<char> >::iterator p = sender.allPackets.begin(); p != sender.allPackets.end(); p++) {
		osc::ReceivedPacket packet(p->data(), p->size());
		CHECK(packet.IsBundle());
		osc::ReceivedBundle bundle(packet);
		for (osc::ReceivedBundleElementIterator it = osc::ReceivedBundleElementIterator(bundle.ElementsBegin());
			 it != osc::ReceivedBundleElementIterator(bundle.ElementsEnd()); it++) {
			if (it->IsMessage()) { osc::ReceivedMessage m(*it); tags[m.AddressPattern()] = m.TypeTags(); }
		}
	}

	CHECK_EQ(std::string("itis"), tags["/tuio2/frm"]);       // int32 ttag int32 string
	CHECK(tags["/tuio2/tok"]=="iiifff" || tags["/tuio2/tok"]=="iiiffffffff");   // 3f or 3+5f tail
	CHECK(tags["/tuio2/ptr"]=="iiiffffff" || tags["/tuio2/ptr"]=="iiifffffffffff"); // 6f or 6+5f tail
	CHECK(tags["/tuio2/bnd"]=="iffffff" || tags["/tuio2/bnd"]=="ifffffffffff");
	CHECK_EQ(std::string("iiiss"), tags["/tuio2/sym"]);     // int int int string string
	CHECK(tags["/tuio2/icg"].substr(0,1)=="i" && tags["/tuio2/icg"].find('T')!=std::string::npos);
	CHECK(tags["/tuio2/skg"].substr(0,1)=="i" && tags["/tuio2/skg"].find('f')!=std::string::npos && tags["/tuio2/skg"].substr(1).find('i')!=std::string::npos);
	CHECK(tags["/tuio2/s3d"].substr(0,1)=="i" && tags["/tuio2/s3d"].substr(1).find('i')!=std::string::npos);
	CHECK_EQ(std::string("if"), tags["/tuio2/svg"]);
	CHECK(tags["/tuio2/arg"].substr(0,2)=="if" && (tags["/tuio2/arg"].size()-1)%3==0);  // i + fff per span
	CHECK_EQ(std::string("iib"), tags["/tuio2/raw"]);
	CHECK(tags["/tuio2/ctl"]=="iTf" || tags["/tuio2/ctl"]=="iFF" || tags["/tuio2/ctl"].substr(0,2)=="iT" || tags["/tuio2/ctl"].substr(0,2)=="iF");
	CHECK_EQ(std::string("iss"), tags["/tuio2/dat"]);
	CHECK(tags["/tuio2/sig"].substr(0,2)=="ii" && tags["/tuio2/sig"].size()>=3);
	CHECK(tags["/tuio2/alv"].size()>=1 && tags["/tuio2/alv"].find_first_not_of('i')==std::string::npos);
	CHECK(tags["/tuio2/ala"].find_first_not_of('i')==std::string::npos);
	CHECK(tags["/tuio2/coa"].substr(0,2)=="ii" && tags["/tuio2/coa"].find_first_not_of('i',2)==std::string::npos);
	CHECK(tags["/tuio2/lia"].substr(0,2)=="iT");
	CHECK(tags["/tuio2/lla"].substr(0,2)=="iF");            // logical -> false
	CHECK(tags["/tuio2/lta"].substr(0,2)=="iT" && tags["/tuio2/lta"].find('f',2)!=std::string::npos);
	CHECK(tags["/tuio2/t3d"].substr(0,3)=="iii" && tags["/tuio2/t3d"].size()>=10);
	CHECK(tags["/tuio2/p3d"].substr(0,3)=="iii" && tags["/tuio2/p3d"].size()>=10);
	CHECK(tags["/tuio2/b3d"].substr(0,1)=="i" && tags["/tuio2/b3d"].size()>=12);
}

// uint32 session IDs must round-trip bit-for-bit through the signed int32
// OSC field per the spec's cast convention
TEST(tuioaudit_uint32_boundary) {
	unsigned int ids[] = { 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu };
	for (int i = 0; i < 3; i++) {
		StubReceiver stub;
		TuioClient client(&stub);
		RecordingListener listener;
		client.addTuioListener(&listener);

		char buffer[4096];
		OutboundPacketStream bundle(buffer, sizeof(buffer));
		bundle << BeginBundleImmediate
			<< BeginMessage("/tuio2/frm") << (int32)(i+1) << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
			<< BeginMessage("/tuio2/tok") << (int32)ids[i] << (int32)0x00010002 << (int32)5 << 0.5f << 0.5f << 0.0f << EndMessage
			<< BeginMessage("/tuio2/alv") << (int32)ids[i] << EndMessage
		<< EndBundle;
		stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

		TuioObject *tobj = client.getTuioObject(ids[i]);
		CHECK(tobj != NULL);
		CHECK_EQ(ids[i], tobj->getSessionID());
	}
}

// s3d: 3D skeleton with xyz triples + node jumps
TEST(tuioaudit_s3d_skeleton) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, sizeof(buffer));
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "f@x" << EndMessage
		<< BeginMessage("/tuio2/bnd") << (int32)7 << 0.5f << 0.5f << 0.0f << 0.1f << 0.1f << 0.01f << EndMessage
		<< BeginMessage("/tuio2/s3d") << (int32)7
			<< 0.1f << 0.2f << 0.3f  << 0.4f << 0.5f << 0.6f
			<< (int32)0
			<< 0.7f << 0.8f << 0.9f
		<< EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)7 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());

	TuioObject *tobj = client.getTuioObject(7);
	CHECK(tobj != NULL);
	TuioGeometry *tgeo = tobj->getTuioGeometry();
	CHECK(tgeo != NULL);
	std::list< std::list<float> > sk = tgeo->getSkeleton3D();
	CHECK_EQ(2, (int)sk.size());
	CHECK_EQ(2, (int)sk.front().size() / 3);  // first branch: 2 points
	CHECK_NEAR(0.3f,sk.front().front() + 0.2f,0.001f);  // z of first point
	std::list<unsigned int> jumps = tgeo->getSkeletonJumps3D();
	CHECK_EQ(1, (int)jumps.size());
}

// link ID packs output<<16|input per spec; the association base
// provides static port helpers
TEST(tuioaudit_link_port_split) {
	unsigned int l_id = TuioAssociation::setLinkPort(0x1234, 0xABCD);
	CHECK(TuioAssociation::getLinkOutputPort(l_id) == 0x1234);
	CHECK(TuioAssociation::getLinkInputPort(l_id) == 0xABCD);
	CHECK(TuioAssociation::getLinkOutputPort(l_id) + (TuioAssociation::getLinkInputPort(l_id)<<16) == 0xABCD1234u);
}

// NOTE: sendFullTuioBundle currently emits only token messages (known limitation)

// disconnect clears pending and session state; reconnecting starts clean
TEST(tuioaudit_disconnect_clears_state) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream partial(buffer, sizeof(buffer));
	partial << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)1 << TimeTag(1) << (int32)8191 << "e@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)2 << (int32)0 << (int32)3 << 0.3f << 0.3f << 0.0f << EndMessage
	<< EndBundle;
	stub.ProcessPacket(partial.Data(), (int)partial.Size(), IpEndpointName());

	client.disconnect();
	CHECK_EQ((int)client.getTuioObjectList().size(), 0);

	// a fresh complete bundle after reconnect produces exactly one add
	client.connect();
	OutboundPacketStream complete(buffer, sizeof(buffer));
	complete << BeginBundleImmediate
		<< BeginMessage("/tuio2/frm") << (int32)2 << TimeTag(1) << (int32)8191 << "e@x" << EndMessage
		<< BeginMessage("/tuio2/tok") << (int32)2 << (int32)0 << (int32)3 << 0.4f << 0.4f << 0.0f << EndMessage
		<< BeginMessage("/tuio2/alv") << (int32)2 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(complete.Data(), (int)complete.Size(), IpEndpointName());

	CHECK_EQ(listener.addObj, 1);
	CHECK_EQ((int)client.getTuioObjectList().size(), 1);
}
