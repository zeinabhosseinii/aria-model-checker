












#ifndef __BRIDGECONTROLLER_ACTOR_H__
#define __BRIDGECONTROLLER_ACTOR_H__

class TrainActor;

#include "AbstractTimedActor.h"

class BridgeControllerActor :
	public virtual AbstractTimedActor
{

public:
    BridgeControllerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~BridgeControllerActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	boolean _ref_isWaiting1;

    	boolean _ref_isWaiting2;

    	boolean _ref_signal1;

    	boolean _ref_signal2;


		long constructorBridgeController(byte creatorID
		);

       virtual void _msg_Arrive(byte senderId
        )
        ;
       virtual long msgsrvArrive(
        )
        ;
       virtual void _msg_Leave(byte senderId
        )
        ;
       virtual long msgsrvLeave(
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_Arrive(byte senderId
        	, TIME_TYPE executionTime, TIME_TYPE deadline)
        		;
      	  void _timed_msg_Leave(byte senderId
        	, TIME_TYPE executionTime, TIME_TYPE deadline)
        		;
    virtual void setNow(TIME_TYPE now);
    

    #ifdef SYMM
    virtual void marshalVariablesToArrayWithPerm(byte* array, byte* perm);
    virtual void marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm);
    #endif

protected:
};

#endif