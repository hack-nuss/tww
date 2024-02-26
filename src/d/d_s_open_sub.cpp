//
// Generated by dtk
// Translation Unit: d_s_open_sub.cpp
//

#include "d/d_s_open.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_msg_mng.h"
#include "JSystem/J2DGraph/J2DScreen.h"
#include "JSystem/J2DGraph/J2DTextBox.h"
#include "JSystem/JKernel/JKRExpHeap.h"
#include "JSystem/JUtility/JUTAssert.h"

/* 80232EFC-8023334C       .text set_message__18dScnOpen_message_cFUli */
void dScnOpen_message_c::set_message(u32, int) {
    /* Nonmatching */
}

/* 8023334C-80233524       .text exec__18dScnOpen_message_cFv */
void dScnOpen_message_c::exec() {
    /* Nonmatching */
}

/* 80233524-80233620       .text __ct__18dScnOpen_message_cFP10JKRExpHeap */
dScnOpen_message_c::dScnOpen_message_c(JKRExpHeap*) {
    /* Nonmatching */
}

/* 80233620-80233698       .text __dt__18dScnOpen_message_cFv */
dScnOpen_message_c::~dScnOpen_message_c() {
    /* Nonmatching */
}

/* 80233698-80233B0C       .text __ct__15dScnOpen_proc_cFv */
dScnOpen_proc_c::dScnOpen_proc_c() {
    /* Nonmatching */
    exp_heap = fopMsgM_createExpHeap(0x20000);
    JUT_ASSERT(0xe2, exp_heap != 0);
    JKRHeap* oldHeap = mDoExt_setCurrentHeap(exp_heap);
    m_Screen = new J2DScreen();
    JUT_ASSERT(0xe7, m_Screen != 0);
    m_message = new dScnOpen_message_c(exp_heap);
    JUT_ASSERT(0xe9, m_message != 0);
    dRes_info_c* resInfo = dComIfG_getObjectResInfo("Opening");
    JUT_ASSERT(0xeb, resInfo != 0);
    m_Screen->set("Opening.blo", resInfo->getArchive());
    d1 = m_Screen->search('\0\0d1');
    d2 = m_Screen->search('\0\0d2');
    d3 = m_Screen->search('\0\0d3');
    d4 = m_Screen->search('\0\0d4');
    d42 = m_Screen->search('\0d42');
    d5 = m_Screen->search('\0\0d5');
    d6 = m_Screen->search('\0\0d6');
    mak1 = m_Screen->search('mak1');
    mak2 = m_Screen->search('mak2');
    tx1 = (J2DTextBox*)m_Screen->search('\0tx1');
    tx2 = (J2DTextBox*)m_Screen->search('\0tx2');
    tx1->move(tx1->mBounds.i.x, 376.0f);
}

/* 80233B0C-80233BE4       .text __dt__15dScnOpen_proc_cFv */
dScnOpen_proc_c::~dScnOpen_proc_c() {
    /* Nonmatching */
}

/* 80233BE4-802344D8       .text proc_execute__15dScnOpen_proc_cFv */
void dScnOpen_proc_c::proc_execute() {
    /* Nonmatching */
}

/* 802344D8-80234550       .text proc_draw__15dScnOpen_proc_cFv */
void dScnOpen_proc_c::proc_draw() {
    /* Nonmatching */
}

/* 80234550-80234570       .text draw__15dScnOpen_proc_cFv */
void dScnOpen_proc_c::draw() {
    proc_draw();
}
