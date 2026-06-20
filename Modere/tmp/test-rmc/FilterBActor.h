












#ifndef __FILTERB_ACTOR_H__
#define __FILTERB_ACTOR_H__

class ProducerActor;
class FilterAActor;
class MergerActor;
class AnalyzerActor;
class CheckerActor;
class LoggerActor;
class SinkActor;

#include "AbstractTimedActor.h"

class FilterBActor :
	public virtual AbstractTimedActor
{

public:
    FilterBActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~FilterBActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	int _ref_accepted;

    	int _ref_rejected;

    	int _ref_last;

    	int _ref_streak;


		long constructorFilterB(byte creatorID
		);

       virtual void _msg_process(byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_mode
        )
        ;
       virtual long msgsrvprocess(
    		int
	         _ref_v
	        , 
    		int
	         _ref_mode
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_process(byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_mode
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