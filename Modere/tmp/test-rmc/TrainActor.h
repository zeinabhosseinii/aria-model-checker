












#ifndef __TRAIN_ACTOR_H__
#define __TRAIN_ACTOR_H__

class BridgeControllerActor;

#include "AbstractTimedActor.h"

class TrainActor :
	public virtual AbstractTimedActor
{

public:
    TrainActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~TrainActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	boolean _ref_onTheBridge;


		long constructorTrain(byte creatorID
		);

       virtual void _msg_YouMayPass(byte senderId
        )
        ;
       virtual long msgsrvYouMayPass(
        )
        ;
       virtual void _msg_Passed(byte senderId
        )
        ;
       virtual long msgsrvPassed(
        )
        ;
       virtual void _msg_ReachBridge(byte senderId
        )
        ;
       virtual long msgsrvReachBridge(
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_YouMayPass(byte senderId
        	, TIME_TYPE executionTime, TIME_TYPE deadline)
        		;
      	  void _timed_msg_Passed(byte senderId
        	, TIME_TYPE executionTime, TIME_TYPE deadline)
        		;
      	  void _timed_msg_ReachBridge(byte senderId
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