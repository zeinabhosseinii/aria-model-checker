












#ifndef __RESOURCEMANAGER_ACTOR_H__
#define __RESOURCEMANAGER_ACTOR_H__

class AppMasterActor;

#include "AbstractTimedActor.h"

class ResourceManagerActor :
	public virtual AbstractTimedActor
{

public:
    ResourceManagerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














    );
	~ResourceManagerActor();
	
	long methodSelectorAndExecutor();
	
    char* getClassName();
    
	virtual void exportStateVariablesInXML(ostream &out, string tab);
	virtual void exportAMessageInXML(ostream &out, string tab, int cnt);

	virtual void exportInsideStateVariablesTagInXML(ostream &out, string tab);

    virtual void marshalActorToArray(byte* array);
    void marshalVariablesToArray(byte * array);

    virtual void unmarshalActorFromArray(byte* array);
    void unmarshalVariablesFromArray(byte * array);


    	int _ref_FREE;

    	int _ref_BUSY;

    	int _ref_appMaster1;

    	int _ref_appMaster2;

    	int _ref_m_queue_misses;

    	int _ref_m_update_miss;

    	int _ref_m_job_complete;

    	std::array<int, 3> _ref_fifo_queue;

    	int _ref_DEFAULT_DEADLINE;

    	int _ref_QUEUE_SIZE;


		long constructorResourceManager(byte creatorID
		);

       virtual void _msg_checkQueue(byte senderId
        )
        ;
       virtual long msgsrvcheckQueue(
        )
        ;
       virtual void _msg_update(byte senderId
	        ,
    		boolean
	         _ref_deadline_miss
        )
        ;
       virtual long msgsrvupdate(
    		boolean
	         _ref_deadline_miss
        )
        ;








	protected:
	    static byte** table;
		virtual byte** getTable();







	protected:
	    static vector<TimeFrame>** hashtableTimeExtension;
		virtual vector<TimeFrame>** getHashtableTimeExtension();







public:
      	  void _timed_msg_checkQueue(byte senderId
        	, TIME_TYPE executionTime, TIME_TYPE deadline)
        		;
      	  void _timed_msg_update(byte senderId
	        ,
    		boolean
	         _ref_deadline_miss
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