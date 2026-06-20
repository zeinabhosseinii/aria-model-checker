












#ifndef __MERGER_ACTOR_H__
#define __MERGER_ACTOR_H__

class ProducerActor;
class FilterAActor;
class FilterBActor;
class AnalyzerActor;
class CheckerActor;
class LoggerActor;
class SinkActor;

#include "AbstractTimedActor.h"

class MergerActor :
	public virtual AbstractTimedActor
{

public:
    MergerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~MergerActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	int _ref_fromA;

    	int _ref_fromB;

    	int _ref_totalA;

    	int _ref_totalB;

    	int _ref_combined;


		long constructorMerger(byte creatorID
		);

       virtual void _msg_collectA(byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_weight
        )
        ;
       virtual long msgsrvcollectA(
    		int
	         _ref_v
	        , 
    		int
	         _ref_weight
        )
        ;
       virtual void _msg_collectB(byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_streak
        )
        ;
       virtual long msgsrvcollectB(
    		int
	         _ref_v
	        , 
    		int
	         _ref_streak
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_collectA(byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_weight
        	, TIME_TYPE executionTime, TIME_TYPE deadline)
        		;
      	  void _timed_msg_collectB(byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_streak
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