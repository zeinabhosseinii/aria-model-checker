                                    


#include "Types.h"
#include "BridgeControllerActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "TrainActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define ARRIVE 1
#define LEAVE 2


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<BridgeControllerActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_t1 (dynamic_cast<TrainActor*>(rebecs[(int)((unsigned char)known[1])]))
    	#define _ref_t2 (dynamic_cast<TrainActor*>(rebecs[(int)((unsigned char)known[2])]))
    
BridgeControllerActor::BridgeControllerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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
    messageNames = new char *[2 + 1];
    messagePriorities = new int[2 + 1];
	messageNames[0] = (char*)"Empty";

	
		
	
		
       		 messageNames[1] = (char*)"ARRIVE";
			messagePriorities[1] = std::numeric_limits<int>::max();
       		 messageNames[2] = (char*)"LEAVE";
			messagePriorities[2] = std::numeric_limits<int>::max();








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

BridgeControllerActor::~BridgeControllerActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void BridgeControllerActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case ARRIVE: {
                out << "Arrive("
                << ")";
                break;
               }
            case LEAVE: {
                out << "Leave("
                << ")";
                break;
               }
    	}
}


void BridgeControllerActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void BridgeControllerActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"BridgeController.isWaiting1\" type=\"boolean\">" <<  
			    		(_ref_isWaiting1? "true" : "false") << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"BridgeController.isWaiting2\" type=\"boolean\">" <<  
			    		(_ref_isWaiting2? "true" : "false") << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"BridgeController.signal1\" type=\"boolean\">" <<  
			    		(_ref_signal1? "true" : "false") << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"BridgeController.signal2\" type=\"boolean\">" <<  
			    		(_ref_signal2? "true" : "false") << "</variable>" << endl;
}


char* BridgeControllerActor::getClassName() {
    return (char*)"BridgeController";
}
void BridgeControllerActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
            
            
            

array [marshalIndex] = 0;
array [marshalIndex] |= (_ref_isWaiting1 ? 0xFF : 0x00) & AbstractActor::coeff[0]; 
array [marshalIndex] |= (_ref_isWaiting2 ? 0xFF : 0x00) & AbstractActor::coeff[1]; 
array [marshalIndex] |= (_ref_signal1 ? 0xFF : 0x00) & AbstractActor::coeff[2]; 
array [marshalIndex] |= (_ref_signal2 ? 0xFF : 0x00) & AbstractActor::coeff[3]; 
++marshalIndex;    
	
}
void BridgeControllerActor::marshalActorToArray(byte* array) {
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

void BridgeControllerActor::unmarshalActorFromArray(byte* array) {

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

void BridgeControllerActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            

            
            

            
            

            
            
_ref_isWaiting1 = (bool) (array [index] & AbstractActor::coeff[0]);
_ref_isWaiting2 = (bool) (array [index] & AbstractActor::coeff[1]);
_ref_signal1 = (bool) (array [index] & AbstractActor::coeff[2]);
_ref_signal2 = (bool) (array [index] & AbstractActor::coeff[3]);
++index;    
	
}

/*#ifdef DEBUG
void BridgeControllerActor::printStateContent(long stateNo) {
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
long BridgeControllerActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case ARRIVE:
                {
                    int i = 0;
                    return msgsrvArrive(
                    );
                }
            case LEAVE:
                {
                    int i = 0;
                    return msgsrvLeave(
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long BridgeControllerActor::constructorBridgeController (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "BridgeController";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_signal1=false);
			(_ref_signal2=false);
			(_ref_isWaiting1=false);
			(_ref_isWaiting2=false);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void BridgeControllerActor::_msg_Arrive (byte senderId
  	  ){
        queueTail = 0;
        enqueue(messageQueue, ARRIVE);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}

		void BridgeControllerActor::_msg_Leave (byte senderId
  	  ){
        queueTail = 0;
        enqueue(messageQueue, LEAVE);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** BridgeControllerActor::table = (byte**)0xFFFFFFFF;

	byte** BridgeControllerActor::getTable() {
		return table;
	}








	vector<TimeFrame>** BridgeControllerActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** BridgeControllerActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void BridgeControllerActor::_timed_msg_Arrive (byte senderId
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_Arrive (senderId
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	    void BridgeControllerActor::_timed_msg_Leave (byte senderId
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_Leave (senderId
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void BridgeControllerActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void BridgeControllerActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
array[_mi] = 0;
array[_mi] |= (_ref_isWaiting1 ? 0xFF : 0x00) & AbstractActor::coeff[0];
array[_mi] |= (_ref_isWaiting2 ? 0xFF : 0x00) & AbstractActor::coeff[1];
array[_mi] |= (_ref_signal1 ? 0xFF : 0x00) & AbstractActor::coeff[2];
array[_mi] |= (_ref_signal2 ? 0xFF : 0x00) & AbstractActor::coeff[3];
_mi++;
}

void BridgeControllerActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case ARRIVE: {
        break;
    }
    case LEAVE: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_ARRIVE
	    long BridgeControllerActor::msgsrvArrive (
    ) {
    			long arrayIndexChecker = 0;TrainActor *temp0;TrainActor *temp1;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Arrive";
		#endif

	#ifdef TTS
	#endif

		{
			if ((_ref_sender==_ref_t1)) {
				{
					if ((_ref_signal2==false)) {
						{
							(_ref_signal1=true);
							(temp0=_ref_t1, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 25"), temp0)->_timed_msg_YouMayPass(myID, _ref_now, MAX_TIME);
						}
}
					else {
						{
							(_ref_isWaiting1=true);
						}
}
				}
}
			else {
				{
					if ((_ref_signal1==false)) {
						{
							(_ref_signal2=true);
							(temp1=_ref_t2, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 34"), temp1)->_timed_msg_YouMayPass(myID, _ref_now, MAX_TIME);
						}
}
					else {
						{
							(_ref_isWaiting2=true);
						}
}
				}
}
		}
	_RETURN_POINT_ARRIVE:
		return 0;

}
	#define _RETURN_ goto _RETURN_POINT_LEAVE
	    long BridgeControllerActor::msgsrvLeave (
    ) {
    			long arrayIndexChecker = 0;TrainActor *temp0;TrainActor *temp1;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Leave";
		#endif

	#ifdef TTS
	#endif

		{
			if ((_ref_sender==_ref_t1)) {
				{
					(_ref_signal1=false);
					if (_ref_isWaiting2) {
						{
							(_ref_signal2=true);
							(temp0=_ref_t2, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 47"), temp0)->_timed_msg_YouMayPass(myID, _ref_now, MAX_TIME);
							(_ref_isWaiting2=false);
						}
}
				}
}
			else {
				{
					(_ref_signal2=false);
					if (_ref_isWaiting1) {
						{
							(_ref_signal1=true);
							(temp1=_ref_t1, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 54"), temp1)->_timed_msg_YouMayPass(myID, _ref_now, MAX_TIME);
							(_ref_isWaiting1=false);
						}
}
				}
}
		}
	_RETURN_POINT_LEAVE:
		return 0;

}
