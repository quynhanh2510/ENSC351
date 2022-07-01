////////////////////////////////////////////////
// Generated by SmartState C++ Code Generator //
//                 DO NOT EDIT				  //
////////////////////////////////////////////////

#pragma warning(disable: 4786)
#pragma warning(disable: 4290)

//Additional Includes
#include "AtomicCOUT.h"
//#include <iostream>
#include <stdlib.h>


#include "ReceiverSS.h"
#include "ReceiverY.h"

/*Messages
Define user specific messages in a file and
include that file in the additional includes section in 
the model.
-- FOLLOWING MESSAGES ARE USED --
SER
CONT
TM
*/

//Additional Declarations
#define c wParam



namespace Receiver_SS
{
using namespace std;
using namespace smartstate;

//State Mgr
//--------------------------------------------------------------------
ReceiverSS::ReceiverSS(ReceiverY* ctx, bool startMachine/*=true*/)
 : StateMgr("ReceiverSS"),
   myCtx(ctx)
{
	myConcStateList.push_back(new Receiver_TopLevel_ReceiverSS("Receiver_TopLevel_ReceiverSS", 0, this));

	if(startMachine)
		start();
}

ReceiverY& ReceiverSS::getCtx() const
{
	return *myCtx;
}

//Base State
//--------------------------------------------------------------------
ReceiverBaseState::ReceiverBaseState(const string& name, BaseState* parent, ReceiverSS* mgr)
 : BaseState(name, parent, mgr)
{
}

//--------------------------------------------------------------------
Receiver_TopLevel_ReceiverSS::Receiver_TopLevel_ReceiverSS(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
	mySubStates.push_back(new NON_CAN_Receiver_TopLevel("NON_CAN_Receiver_TopLevel", this, mgr));
	mySubStates.push_back(new CAN_Receiver_TopLevel("CAN_Receiver_TopLevel", this, mgr));
	setType(eSuper);
}

void Receiver_TopLevel_ReceiverSS::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> Receiver_TopLevel_ReceiverSS <onEntry>");

}

void Receiver_TopLevel_ReceiverSS::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< Receiver_TopLevel_ReceiverSS <onExit>");

}

void Receiver_TopLevel_ReceiverSS::onMessage(const Mesg& mesg)
{
	if(mesg.message == SER)
		onSERMessage(mesg);
	else 
		super::onMessage(mesg);
}

void Receiver_TopLevel_ReceiverSS::onSERMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("Receiver_TopLevel_ReceiverSS SER <message trapped>");

	if(true)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("Receiver_TopLevel_ReceiverSS SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("Receiver_TopLevel_ReceiverSS", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("Receiver_TopLevel_ReceiverSS SER <executing effect>");


		//User specified effect begin
		COUT << "Receiver received totally unexpected char #" << c << ": " << (char) c << endl;
		exit(EXIT_FAILURE);
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("Receiver_TopLevel_ReceiverSS SER <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
NON_CAN_Receiver_TopLevel::NON_CAN_Receiver_TopLevel(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
	mySubStates.push_back(new FirstByteStat_NON_CAN("FirstByteStat_NON_CAN", this, mgr));
	mySubStates.push_back(new FirstByteData_NON_CAN("FirstByteData_NON_CAN", this, mgr));
	mySubStates.push_back(new EOT_NON_CAN("EOT_NON_CAN", this, mgr));
	mySubStates.push_back(new CondTransientData_NON_CAN("CondTransientData_NON_CAN", this, mgr));
	mySubStates.push_back(new CondTransientCheck_NON_CAN("CondTransientCheck_NON_CAN", this, mgr));
	mySubStates.push_back(new CondTransientOpen_NON_CAN("CondTransientOpen_NON_CAN", this, mgr));
	mySubStates.push_back(new CondTransientEOT_NON_CAN("CondTransientEOT_NON_CAN", this, mgr));
	mySubStates.push_back(new CondlTransientStat_NON_CAN("CondlTransientStat_NON_CAN", this, mgr));
	mySubStates.push_back(new Timeout_NON_CAN("Timeout_NON_CAN", this, mgr));
	setType(eSuper);
}

void NON_CAN_Receiver_TopLevel::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> NON_CAN_Receiver_TopLevel <onEntry>");

	ReceiverY& ctx = getMgr()->getCtx();

	// Code from Model here
	    ctx.sendByte(ctx.NCGbyte); 
	    ctx.errCnt=0;
	//    ctx.numLastGoodBlk=255;
	    ctx.anotherFile=0;
	    ctx.transferringFileD = -1; 
	    ctx.closeProb = -1;
	    ctx.tm(0);
}

void NON_CAN_Receiver_TopLevel::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< NON_CAN_Receiver_TopLevel <onExit>");

}

void NON_CAN_Receiver_TopLevel::onMessage(const Mesg& mesg)
{
	if(mesg.message == SER)
		onSERMessage(mesg);
	else 
		super::onMessage(mesg);
}

void NON_CAN_Receiver_TopLevel::onSERMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("NON_CAN_Receiver_TopLevel SER <message trapped>");

	if(c==CAN)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("NON_CAN_Receiver_TopLevel SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("NON_CAN_Receiver_TopLevel", "CAN_Receiver_TopLevel");
		/* -g option specified while compilation. */
		myMgr->debugLog("NON_CAN_Receiver_TopLevel SER <executing effect>");


		//User specified effect begin
		//nil
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("NON_CAN_Receiver_TopLevel SER <executing entry>");

		getMgr()->executeEntry(root, "CAN_Receiver_TopLevel");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
FirstByteData_NON_CAN::FirstByteData_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void FirstByteData_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> FirstByteData_NON_CAN <onEntry>");

}

void FirstByteData_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< FirstByteData_NON_CAN <onExit>");

}

void FirstByteData_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == SER)
		onSERMessage(mesg);
	else 
		super::onMessage(mesg);
}

void FirstByteData_NON_CAN::onSERMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteData_NON_CAN SER <message trapped>");

	if(c == EOT)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteData_NON_CAN SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("FirstByteData_NON_CAN", "EOT_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteData_NON_CAN SER <executing effect>");


		//User specified effect begin
		ctx.sendByte(NAK);
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteData_NON_CAN SER <executing entry>");

		getMgr()->executeEntry(root, "EOT_NON_CAN");
		return;
	}
	else
	if(c==SOH)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteData_NON_CAN SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("FirstByteData_NON_CAN", "CondTransientData_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteData_NON_CAN SER <executing effect>");


		//User specified effect begin
		ctx.getRestBlk();
		if (ctx.goodBlk1st) {
		     ctx.errCnt=0;
		     ctx.anotherFile=0;
		}
		else ctx.errCnt++;
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteData_NON_CAN SER <executing entry>");

		getMgr()->executeEntry(root, "CondTransientData_NON_CAN");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
EOT_NON_CAN::EOT_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void EOT_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> EOT_NON_CAN <onEntry>");

}

void EOT_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< EOT_NON_CAN <onExit>");

}

void EOT_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == SER)
		onSERMessage(mesg);
	else 
		super::onMessage(mesg);
}

void EOT_NON_CAN::onSERMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("EOT_NON_CAN SER <message trapped>");

	if(c==EOT)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("EOT_NON_CAN SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("EOT_NON_CAN", "CondTransientEOT_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("EOT_NON_CAN SER <executing effect>");


		//User specified effect begin
		ctx.closeTransferredFile();
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("EOT_NON_CAN SER <executing entry>");

		getMgr()->executeEntry(root, "CondTransientEOT_NON_CAN");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
CondTransientData_NON_CAN::CondTransientData_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void CondTransientData_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> CondTransientData_NON_CAN <onEntry>");

	ReceiverY& ctx = getMgr()->getCtx();

	// Code from Model here
	     POST("*",CONT);
}

void CondTransientData_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< CondTransientData_NON_CAN <onExit>");

}

void CondTransientData_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == CONT)
		onCONTMessage(mesg);
	else 
		super::onMessage(mesg);
}

void CondTransientData_NON_CAN::onCONTMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientData_NON_CAN CONT <message trapped>");

	if(!ctx.syncLoss && (ctx.errCnt < errB))
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientData_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientData_NON_CAN", "FirstByteData_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientData_NON_CAN CONT <executing effect>");


		//User specified effect begin
		if (ctx.goodBlk) { 
		     ctx.sendByte(ACK);
		     if (ctx.anotherFile) ctx.sendByte(ctx.NCGbyte);
		}
		else  ctx.sendByte(NAK);
		if (ctx.goodBlk1st) 
		     ctx.writeChunk();
		
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientData_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FirstByteData_NON_CAN");
		return;
	}
	else
	if(ctx.syncLoss || ctx.errCnt >= errB)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientData_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientData_NON_CAN", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientData_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.cans();
		ctx.closeTransferredFile();
		if (ctx.syncLoss)
		     ctx.result="LossOfSyncronization";
		else
		     ctx.result="ExcessiveErrors";
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientData_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
FirstByteStat_NON_CAN::FirstByteStat_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void FirstByteStat_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> FirstByteStat_NON_CAN <onEntry>");

}

void FirstByteStat_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< FirstByteStat_NON_CAN <onExit>");

}

void FirstByteStat_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == SER)
		onSERMessage(mesg);
	else 
		super::onMessage(mesg);
}

void FirstByteStat_NON_CAN::onSERMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <message trapped>");

	if(c==EOT && !ctx.closeProb && ctx.errCnt >= errB)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("FirstByteStat_NON_CAN", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <executing effect>");


		//User specified effect begin
		ctx.cans();
		ctx.result="ExcessiveEOTs";
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}
	else
	if(c==EOT && !ctx.closeProb && ctx.errCnt < errB)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <executing effect>");


		//User specified effect begin
		ctx.sendByte(ACK);
		ctx.sendByte(ctx.NCGbyte);
		ctx.errCnt++;
		//User specified effect end

		return;
	}
	else
	if(c==SOH)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("FirstByteStat_NON_CAN", "CondlTransientStat_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <executing effect>");


		//User specified effect begin
		ctx.getRestBlk();
		if (!ctx.closeProb) {
		    ctx.errCnt=0;
		    ctx.closeProb = -1;
		}
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("FirstByteStat_NON_CAN SER <executing entry>");

		getMgr()->executeEntry(root, "CondlTransientStat_NON_CAN");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
CondTransientCheck_NON_CAN::CondTransientCheck_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void CondTransientCheck_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> CondTransientCheck_NON_CAN <onEntry>");

	ReceiverY& ctx = getMgr()->getCtx();

	// Code from Model here
	     POST("*",CONT);
}

void CondTransientCheck_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< CondTransientCheck_NON_CAN <onExit>");

}

void CondTransientCheck_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == CONT)
		onCONTMessage(mesg);
	else 
		super::onMessage(mesg);
}

void CondTransientCheck_NON_CAN::onCONTMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientCheck_NON_CAN CONT <message trapped>");

	if(!ctx.anotherFile)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientCheck_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientCheck_NON_CAN", "Timeout_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientCheck_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.sendByte(ACK);
		ctx.tm(TM_END); 
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientCheck_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "Timeout_NON_CAN");
		return;
	}
	else
	if(ctx.anotherFile)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientCheck_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientCheck_NON_CAN", "CondTransientOpen_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientCheck_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.openFileForTransfer();
		
		
		
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientCheck_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "CondTransientOpen_NON_CAN");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
CondTransientOpen_NON_CAN::CondTransientOpen_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void CondTransientOpen_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> CondTransientOpen_NON_CAN <onEntry>");

	ReceiverY& ctx = getMgr()->getCtx();

	// Code from Model here
	     POST("*",CONT);
}

void CondTransientOpen_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< CondTransientOpen_NON_CAN <onExit>");

}

void CondTransientOpen_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == CONT)
		onCONTMessage(mesg);
	else 
		super::onMessage(mesg);
}

void CondTransientOpen_NON_CAN::onCONTMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientOpen_NON_CAN CONT <message trapped>");

	if(ctx.transferringFileD != -1)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientOpen_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientOpen_NON_CAN", "FirstByteData_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientOpen_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.sendByte(ACK);
		ctx.sendByte(ctx.NCGbyte);
		
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientOpen_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FirstByteData_NON_CAN");
		return;
	}
	else
	if(ctx.transferringFileD == -1)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientOpen_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientOpen_NON_CAN", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientOpen_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.cans();
		ctx.result="CreatError";
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientOpen_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
CondTransientEOT_NON_CAN::CondTransientEOT_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void CondTransientEOT_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> CondTransientEOT_NON_CAN <onEntry>");

	ReceiverY& ctx = getMgr()->getCtx();

	// Code from Model here
	     POST("*",CONT);
}

void CondTransientEOT_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< CondTransientEOT_NON_CAN <onExit>");

}

void CondTransientEOT_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == CONT)
		onCONTMessage(mesg);
	else 
		super::onMessage(mesg);
}

void CondTransientEOT_NON_CAN::onCONTMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientEOT_NON_CAN CONT <message trapped>");

	if(!ctx.closeProb)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientEOT_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientEOT_NON_CAN", "FirstByteStat_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientEOT_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.sendByte(ACK);
		ctx.sendByte(ctx.NCGbyte);
		ctx.errCnt=0;
		//ctx.numLastGoodBlk=255;
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientEOT_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FirstByteStat_NON_CAN");
		return;
	}
	else
	if(ctx.closeProb)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientEOT_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondTransientEOT_NON_CAN", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientEOT_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.cans(); 
		ctx.result="CloseError";
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondTransientEOT_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
CondlTransientStat_NON_CAN::CondlTransientStat_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void CondlTransientStat_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> CondlTransientStat_NON_CAN <onEntry>");

	ReceiverY& ctx = getMgr()->getCtx();

	// Code from Model here
	     POST("*",CONT);
}

void CondlTransientStat_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< CondlTransientStat_NON_CAN <onExit>");

}

void CondlTransientStat_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == CONT)
		onCONTMessage(mesg);
	else 
		super::onMessage(mesg);
}

void CondlTransientStat_NON_CAN::onCONTMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <message trapped>");

	if(!ctx.syncLoss && (ctx.errCnt < errB)  && !ctx.goodBlk)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondlTransientStat_NON_CAN", "FirstByteStat_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.sendByte(NAK);
		ctx.errCnt++;
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FirstByteStat_NON_CAN");
		return;
	}
	else
	if(!ctx.syncLoss && (ctx.errCnt < errB) && ctx.goodBlk)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondlTransientStat_NON_CAN", "CondTransientCheck_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.checkForAnotherFile();
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "CondTransientCheck_NON_CAN");
		return;
	}
	else
	if(ctx.syncLoss || ctx.errCnt >= errB)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing exit>");

		const BaseState* root = getMgr()->executeExit("CondlTransientStat_NON_CAN", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing effect>");


		//User specified effect begin
		ctx.cans();
		if (ctx.syncLoss)
		     ctx.result="LossOfSync at Stat Blk";
		else
		     ctx.result="ExcessiveErrors at Stat";
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CondlTransientStat_NON_CAN CONT <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
Timeout_NON_CAN::Timeout_NON_CAN(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void Timeout_NON_CAN::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> Timeout_NON_CAN <onEntry>");

}

void Timeout_NON_CAN::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< Timeout_NON_CAN <onExit>");

}

void Timeout_NON_CAN::onMessage(const Mesg& mesg)
{
	if(mesg.message == TM)
		onTMMessage(mesg);
	else if(mesg.message == SER)
		onSERMessage(mesg);
	else 
		super::onMessage(mesg);
}

void Timeout_NON_CAN::onTMMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN TM <message trapped>");

	if(true)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN TM <executing exit>");

		const BaseState* root = getMgr()->executeExit("Timeout_NON_CAN", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN TM <executing effect>");


		//User specified effect begin
		ctx.result="Done";
		
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN TM <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}

	super::onMessage(mesg);
}

void Timeout_NON_CAN::onSERMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN SER <message trapped>");

	if(c==SOH)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("Timeout_NON_CAN", "CondlTransientStat_NON_CAN");
		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN SER <executing effect>");


		//User specified effect begin
		ctx.getRestBlk();
		ctx.errCnt++;
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("Timeout_NON_CAN SER <executing entry>");

		getMgr()->executeEntry(root, "CondlTransientStat_NON_CAN");
		return;
	}

	super::onMessage(mesg);
}

//--------------------------------------------------------------------
CAN_Receiver_TopLevel::CAN_Receiver_TopLevel(const string& name, BaseState* parent, ReceiverSS* mgr)
 : ReceiverBaseState(name, parent, mgr)
{
	myHistory = false;
}

void CAN_Receiver_TopLevel::onEntry()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("> CAN_Receiver_TopLevel <onEntry>");

}

void CAN_Receiver_TopLevel::onExit()
{
	/* -g option specified while compilation. */
	myMgr->debugLog("< CAN_Receiver_TopLevel <onExit>");

}

void CAN_Receiver_TopLevel::onMessage(const Mesg& mesg)
{
	if(mesg.message == SER)
		onSERMessage(mesg);
	else 
		super::onMessage(mesg);
}

void CAN_Receiver_TopLevel::onSERMessage(const Mesg& mesg)
{
	int wParam = mesg.wParam;
	int lParam = mesg.lParam;
	ReceiverY& ctx = getMgr()->getCtx();

		/* -g option specified while compilation. */
		myMgr->debugLog("CAN_Receiver_TopLevel SER <message trapped>");

	if(c == CAN)
	{
		/* -g option specified while compilation. */
		myMgr->debugLog("CAN_Receiver_TopLevel SER <executing exit>");

		const BaseState* root = getMgr()->executeExit("CAN_Receiver_TopLevel", "FinalState");
		/* -g option specified while compilation. */
		myMgr->debugLog("CAN_Receiver_TopLevel SER <executing effect>");


		//User specified effect begin
		if (ctx.transferringFileD != -1) 
		     ctx.closeTransferredFile();
		ctx.result="SndCancelled";
		
		//User specified effect end

		/* -g option specified while compilation. */
		myMgr->debugLog("CAN_Receiver_TopLevel SER <executing entry>");

		getMgr()->executeEntry(root, "FinalState");
		return;
	}

	super::onMessage(mesg);
}


} /*end namespace*/

//___________________________________vv^^vv___________________________________
