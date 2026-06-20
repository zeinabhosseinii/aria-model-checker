












#ifndef __ANALYZER_ACTOR_H__
#define __ANALYZER_ACTOR_H__

class ProducerActor;
class FilterAActor;
class FilterBActor;
class MergerActor;
class CheckerActor;
class LoggerActor;
class SinkActor;

#include "AbstractTimedActor.h"

class AnalyzerActor :
	public virtual AbstractTimedActor
{

public:
    AnalyzerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~AnalyzerActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	int _ref_runs;

    	int _ref_peak;

    	int _ref_trough;

    	int _ref_sumAll;

    	int _ref_balanced;


		long constructorAnalyzer(byte creatorID
		);

       virtual void _msg_analyze(byte senderId
	        ,
    		int
	         _ref_combined
	        , 
    		int
	         _ref_cntA
	        , 
    		int
	         _ref_cntB
        )
        ;
       virtual long msgsrvanalyze(
    		int
	         _ref_combined
	        , 
    		int
	         _ref_cntA
	        , 
    		int
	         _ref_cntB
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_analyze(byte senderId
	        ,
    		int
	         _ref_combined
	        , 
    		int
	         _ref_cntA
	        , 
    		int
	         _ref_cntB
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