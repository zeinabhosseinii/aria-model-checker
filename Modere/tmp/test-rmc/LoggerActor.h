












#ifndef __LOGGER_ACTOR_H__
#define __LOGGER_ACTOR_H__

class ProducerActor;
class FilterAActor;
class FilterBActor;
class MergerActor;
class AnalyzerActor;
class CheckerActor;
class SinkActor;

#include "AbstractTimedActor.h"

class LoggerActor :
	public virtual AbstractTimedActor
{

public:
    LoggerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~LoggerActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	int _ref_entries;

    	int _ref_total;

    	int _ref_last;

    	int _ref_maxSeen;


		long constructorLogger(byte creatorID
		);

       virtual void _msg_logIt(byte senderId
	        ,
    		int
	         _ref_v
        )
        ;
       virtual long msgsrvlogIt(
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
      	  void _timed_msg_logIt(byte senderId
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