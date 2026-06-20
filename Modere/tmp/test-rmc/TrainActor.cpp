                                    


#include "Types.h"
#include "TrainActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "BridgeControllerActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define YOUMAYPASS 1
#define PASSED 2
#define REACHBRIDGE 3


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<TrainActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_controller (dynamic_cast<BridgeControllerActor*>(rebecs[(int)((unsigned char)known[1])]))
    
TrainActor::TrainActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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
    messageNames = new char *[3 + 1];
    messagePriorities = new int[3 + 1];
	messageNames[0] = (char*)"Empty";

	
		
	
		
       		 messageNames[1] = (char*)"YOUMAYPASS";
			messagePriorities[1] = std::numeric_limits<int>::max();
       		 messageNames[2] = (char*)"PASSED";
			messagePriorities[2] = std::numeric_limits<int>::max();
       		 messageNames[3] = (char*)"REACHBRIDGE";
			messagePriorities[3] = std::numeric_limits<int>::max();








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

TrainActor::~TrainActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void TrainActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case YOUMAYPASS: {
                out << "YouMayPass("
                << ")";
                break;
               }
            case PASSED: {
                out << "Passed("
                << ")";
                break;
               }
            case REACHBRIDGE: {
                out << "ReachBridge("
                << ")";
                break;
               }
    	}
}


void TrainActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void TrainActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"Train.onTheBridge\" type=\"boolean\">" <<  
			    		(_ref_onTheBridge? "true" : "false") << "</variable>" << endl;
}


char* TrainActor::getClassName() {
    return (char*)"Train";
}
void TrainActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            

array [marshalIndex] = 0;
array [marshalIndex] |= (_ref_onTheBridge ? 0xFF : 0x00) & AbstractActor::coeff[0]; 
++marshalIndex;    
	
}
void TrainActor::marshalActorToArray(byte* array) {
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

void TrainActor::unmarshalActorFromArray(byte* array) {

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

void TrainActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
_ref_onTheBridge = (bool) (array [index] & AbstractActor::coeff[0]);
++index;    
	
}

/*#ifdef DEBUG
void TrainActor::printStateContent(long stateNo) {
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
long TrainActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case YOUMAYPASS:
                {
                    int i = 0;
                    return msgsrvYouMayPass(
                    );
                }
            case PASSED:
                {
                    int i = 0;
                    return msgsrvPassed(
                    );
                }
            case REACHBRIDGE:
                {
                    int i = 0;
                    return msgsrvReachBridge(
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long TrainActor::constructorTrain (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;TrainActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Train";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_onTheBridge=false);
			(temp0=_ref_self, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 72"), temp0)->_timed_msg_Passed(myID, _ref_now, MAX_TIME);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void TrainActor::_msg_YouMayPass (byte senderId
  	  ){
        queueTail = 0;
        enqueue(messageQueue, YOUMAYPASS);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}

		void TrainActor::_msg_Passed (byte senderId
  	  ){
        queueTail = 0;
        enqueue(messageQueue, PASSED);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}

		void TrainActor::_msg_ReachBridge (byte senderId
  	  ){
        queueTail = 0;
        enqueue(messageQueue, REACHBRIDGE);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** TrainActor::table = (byte**)0xFFFFFFFF;

	byte** TrainActor::getTable() {
		return table;
	}








	vector<TimeFrame>** TrainActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** TrainActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void TrainActor::_timed_msg_YouMayPass (byte senderId
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_YouMayPass (senderId
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	    void TrainActor::_timed_msg_Passed (byte senderId
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_Passed (senderId
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	    void TrainActor::_timed_msg_ReachBridge (byte senderId
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_ReachBridge (senderId
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void TrainActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void TrainActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
array[_mi] = 0;
array[_mi] |= (_ref_onTheBridge ? 0xFF : 0x00) & AbstractActor::coeff[0];
_mi++;
}

void TrainActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case YOUMAYPASS: {
        break;
    }
    case PASSED: {
        break;
    }
    case REACHBRIDGE: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_YOUMAYPASS
	    long TrainActor::msgsrvYouMayPass (
    ) {
    			long arrayIndexChecker = 0;TrainActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "YouMayPass";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_onTheBridge=true);
			(temp0=_ref_self, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 77"), temp0)->_timed_msg_Passed(myID, _ref_now, MAX_TIME);
		}
	_RETURN_POINT_YOUMAYPASS:
		return 0;

}
	#define _RETURN_ goto _RETURN_POINT_PASSED
	    long TrainActor::msgsrvPassed (
    ) {
    			long arrayIndexChecker = 0;BridgeControllerActor *temp0;TrainActor *temp1;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Passed";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_onTheBridge=false);
			(temp0=_ref_controller, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 82"), temp0)->_timed_msg_Leave(myID, _ref_now, MAX_TIME);
			(temp1=_ref_self, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 83"), temp1)->_timed_msg_ReachBridge(myID, _ref_now, MAX_TIME);
		}
	_RETURN_POINT_PASSED:
		return 0;

}
	#define _RETURN_ goto _RETURN_POINT_REACHBRIDGE
	    long TrainActor::msgsrvReachBridge (
    ) {
    			long arrayIndexChecker = 0;BridgeControllerActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "ReachBridge";
		#endif

	#ifdef TTS
	#endif

		{
			(temp0=_ref_controller, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 87"), temp0)->_timed_msg_Arrive(myID, _ref_now, MAX_TIME);
		}
	_RETURN_POINT_REACHBRIDGE:
		return 0;

}
