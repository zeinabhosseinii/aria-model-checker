                                    


#include "Types.h"
#include "AppMasterActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "ResourceManagerActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define RUNJOB 1


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<AppMasterActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_rm (dynamic_cast<ResourceManagerActor*>(rebecs[(int)((unsigned char)known[1])]))
    
AppMasterActor::AppMasterActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














)
	:
		AbstractTimedActor (myID, myName, maxQueueLength, maxParamLength, rebecsRef
			#ifdef DEBUG_LEVEL_2 
				, out
			#endif







		, stateSize, hashmapSize














		)
	, AbstractActor (myID, myName, maxQueueLength, maxParamLength, rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, out
		#endif







		, stateSize, hashmapSize














	)
{
    messageNames = new char *[1 + 1];
    messagePriorities = new int[1 + 1];
	messageNames[0] = (char*)"Empty";

	
		
	
		
       		 messageNames[1] = (char*)"RUNJOB";
			messagePriorities[1] = std::numeric_limits<int>::max();








	if (table == (byte**)0xFFFFFFFF) {	// avoid reinitialization
		table = new byte* [1 << hashmapSize];
        memset (table, 0, sizeof(byte*)*(1 << hashmapSize));
	}








	if (hashtableTimeExtension == (vector<TimeFrame>**)0xFFFFFFFF) {	// avoid reinitialization
        hashtableTimeExtension = new vector<TimeFrame>*[1 << hashmapSize];
        memset (hashtableTimeExtension, 0, sizeof(vector<TimeFrame>*)*(1 << hashmapSize));
	}







	
	
	queueTail = 0;
}

AppMasterActor::~AppMasterActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void AppMasterActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case RUNJOB: {

            	int _ref_dline;
            
            
                	memcpy(&_ref_dline, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "runJob("
                	<< ((int)_ref_dline)
                << ")";
                break;
               }
    	}
}


void AppMasterActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void AppMasterActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"AppMaster.doneJobs\" type=\"int\">" <<  
			    		((int)_ref_doneJobs) << "</variable>" << endl;
}


char* AppMasterActor::getClassName() {
    return (char*)"AppMaster";
}
void AppMasterActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_doneJobs, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void AppMasterActor::marshalActorToArray(byte* array) {
    int i,j, marshalIndex = 0;

	for (i = (shift ? 1 : 0); i < maxQueueLength; i++) {
        array[marshalIndex++] = messageQueue[i];
    }

    if (shift) {
        array[marshalIndex++] = 0;
    }
    
	for (i = (shift ? 1 : 0); i < maxQueueLength; i++) {
        array[marshalIndex++] = senderQueue[i];
    }

    if (shift) {
        array[marshalIndex++] = 0;
    }

	if(maxParamLength != 0) {
        for (i = (shift ? 1 : 0); i < maxQueueLength; i++) {
            for (j = 0; j < maxParamLength; j++) {
                array[marshalIndex++] = paramQueue[i][j];
            }
        }
        if (shift) {
            for (j = 0; j < maxParamLength; j++) {
                array[marshalIndex++] = 0;
            }
        }
    }

	 #ifdef NO_ATOMIC_EXECUTION
    	memcpy(&array [marshalIndex], &__pc, 4);
    	marshalIndex += 4;
    	//array[marshalIndex++] = __pc;
    #endif

		marshalVariablesToArray( & array[marshalIndex]);

   

}

void AppMasterActor::unmarshalActorFromArray(byte* array) {

    int index = 0, i, j;
    shift = 0;

	for (i = 0; i < maxQueueLength; i++) {
        messageQueue[i] = array[index++];
    }
	
	for (i = 0; i < maxQueueLength; i++) {
        senderQueue[i] = array[index++];
    }
    
	if(maxParamLength != 0) {
        for (i = 0; i < maxQueueLength; i++) {
            for (j = 0; j < maxParamLength; j++) {
                paramQueue[i][j] = array[index++];
            }
        }
    }
    
	#ifdef NO_ATOMIC_EXECUTION
    	memcpy(&__pc, &array [index], 4);
    	index += 4;
    	//__pc = array[index++];
    #endif
    
	unmarshalVariablesFromArray( & array[index]);
}

void AppMasterActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_doneJobs, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void AppMasterActor::printStateContent(long stateNo) {
	out << myName << "=>[";
	byte* currentValues = getTable()[stateNo];
	for (int cnt = 0; cnt < STATE_SIZE; cnt++)
		out << (int)currentValues[cnt] << ", ";
	out << "]";
}
#endif
*/

//---------------------------------------------------------
/**
  Checks which action of this rebec is enabled, and executes it. Actions are the
  same as the location-counter defined at the beginning of this file.
  In the case of non-atomic message servers, a location-counter may point to the
  middle of a message server.
  If this action leads to more than one transition (non_det), this method executes
  only one of these transitions. With the help of some static variables, consecutive
  calls to this method execute different transitions caused by this action.
  Local variables must be properly valuated before any call to this method (see marshal()).

  returns: >0 : there are more possible next states (non-det transitions).
		   =0 : no more non_det transitions
		   -1 : no enabled action (empty queue)
		   -2 : Local hash table is full - immediate termination of search
 */
long AppMasterActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case RUNJOB:
                {
                    int i = 0;

            	int _ref_dline;
            
            
                	memcpy(&_ref_dline, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvrunJob(
            _ref_dline
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long AppMasterActor::constructorAppMaster (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "AppMaster";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_doneJobs=0);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void AppMasterActor::_msg_runJob (byte senderId
	        ,
    		int
	         _ref_dline
  	  ){
        queueTail = 0;
        enqueue(messageQueue, RUNJOB);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            
                	memcpy(&paramsValues [index], &_ref_dline, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** AppMasterActor::table = (byte**)0xFFFFFFFF;

	byte** AppMasterActor::getTable() {
		return table;
	}








	vector<TimeFrame>** AppMasterActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** AppMasterActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void AppMasterActor::_timed_msg_runJob (byte senderId
	        ,
    		int
	         _ref_dline
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_runJob (senderId
	        ,
            _ref_dline
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void AppMasterActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void AppMasterActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_doneJobs, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void AppMasterActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case RUNJOB: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_RUNJOB
	    long AppMasterActor::msgsrvrunJob (
    		int
	         _ref_dline
    ) {
    			long arrayIndexChecker = 0;ResourceManagerActor *temp0;ResourceManagerActor *temp1;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "runJob";
		#endif

	#ifdef TTS
	#endif

		{
			int _ref_completion = 2;

			boolean _ref_deadline_miss;

			if ((_ref_completion>_ref_dline)) {
				{
					(_ref_deadline_miss=true);
					(temp0=_ref_rm, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 142"), temp0)->_timed_msg_update(myID, _ref_deadline_miss, _ref_now, MAX_TIME);
				}
}
			else {
				{
					(_ref_deadline_miss=false);
					(temp1=_ref_rm, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 148"), temp1)->_timed_msg_update(myID, _ref_deadline_miss, _ref_now, MAX_TIME);
					(_ref_doneJobs++);
					if ((_ref_doneJobs>5)) {
						(_ref_doneJobs=1);}
				}
}
		}
	_RETURN_POINT_RUNJOB:
		return 0;

}
