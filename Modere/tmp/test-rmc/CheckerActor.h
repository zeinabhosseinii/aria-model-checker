












#ifndef __CHECKER_ACTOR_H__
#define __CHECKER_ACTOR_H__

class ProducerActor;
class FilterAActor;
class FilterBActor;
class MergerActor;
class AnalyzerActor;
class LoggerActor;
class SinkActor;

#include "AbstractTimedActor.h"

class CheckerActor :
	public virtual AbstractTimedActor
{

public:
    CheckerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~CheckerActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	int _ref_checks;

    	int _ref_failures;

    	int _ref_lastPeak;

    	int _ref_lastTrough;


		long constructorChecker(byte creatorID
		);

       virtual void _msg_verify(byte senderId
	        ,
    		int
	         _ref_peak
	        , 
    		int
	         _ref_trough
	        , 
    		int
	         _ref_balanced
	        , 
    		int
	         _ref_runs
        )
        ;
       virtual long msgsrvverify(
    		int
	         _ref_peak
	        , 
    		int
	         _ref_trough
	        , 
    		int
	         _ref_balanced
	        , 
    		int
	         _ref_runs
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_verify(byte senderId
	        ,
    		int
	         _ref_peak
	        , 
    		int
	         _ref_trough
	        , 
    		int
	         _ref_balanced
	        , 
    		int
	         _ref_runs
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