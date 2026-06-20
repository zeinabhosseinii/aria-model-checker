












#ifndef __SINK_ACTOR_H__
#define __SINK_ACTOR_H__

class ProducerActor;
class FilterAActor;
class FilterBActor;
class MergerActor;
class AnalyzerActor;
class CheckerActor;
class LoggerActor;

#include "AbstractTimedActor.h"

class SinkActor :
	public virtual AbstractTimedActor
{

public:
    SinkActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~SinkActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	int _ref_a;

    	int _ref_b;

    	int _ref_c;

    	int _ref_d;

    	int _ref_e;

    	int _ref_writes;

    	int _ref_total;


		long constructorSink(byte creatorID
		);

       virtual void _msg_store(byte senderId
	        ,
    		int
	         _ref_v
        )
        ;
       virtual long msgsrvstore(
    		int
	         _ref_v
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_store(byte senderId
	        ,
    		int
	         _ref_v
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