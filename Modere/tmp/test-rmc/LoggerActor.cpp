                                    


#include "Types.h"
#include "LoggerActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "ProducerActor.h"
#include "FilterAActor.h"
#include "FilterBActor.h"
#include "MergerActor.h"
#include "AnalyzerActor.h"
#include "CheckerActor.h"
#include "SinkActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define LOGIT 1


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<LoggerActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_sk (dynamic_cast<SinkActor*>(rebecs[(int)((unsigned char)known[1])]))
    
LoggerActor::LoggerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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

	
		
	
		
       		 messageNames[1] = (char*)"LOGIT";
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

LoggerActor::~LoggerActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void LoggerActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case LOGIT: {

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "logIt("
                	<< ((int)_ref_v)
                << ")";
                break;
               }
    	}
}


void LoggerActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void LoggerActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"Logger.entries\" type=\"int\">" <<  
			    		((int)_ref_entries) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Logger.total\" type=\"int\">" <<  
			    		((int)_ref_total) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Logger.last\" type=\"int\">" <<  
			    		((int)_ref_last) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Logger.maxSeen\" type=\"int\">" <<  
			    		((int)_ref_maxSeen) << "</variable>" << endl;
}


char* LoggerActor::getClassName() {
    return (char*)"Logger";
}
void LoggerActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_entries, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_total, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_last, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_maxSeen, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void LoggerActor::marshalActorToArray(byte* array) {
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

void LoggerActor::unmarshalActorFromArray(byte* array) {

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

void LoggerActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_entries, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_total, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_last, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_maxSeen, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void LoggerActor::printStateContent(long stateNo) {
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
long LoggerActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case LOGIT:
                {
                    int i = 0;

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvlogIt(
            _ref_v
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long LoggerActor::constructorLogger (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Logger";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_entries=0);
			(_ref_total=0);
			(_ref_last=0);
			(_ref_maxSeen=0);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void LoggerActor::_msg_logIt (byte senderId
	        ,
    		int
	         _ref_v
  	  ){
        queueTail = 0;
        enqueue(messageQueue, LOGIT);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            
                	memcpy(&paramsValues [index], &_ref_v, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** LoggerActor::table = (byte**)0xFFFFFFFF;

	byte** LoggerActor::getTable() {
		return table;
	}








	vector<TimeFrame>** LoggerActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** LoggerActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void LoggerActor::_timed_msg_logIt (byte senderId
	        ,
    		int
	         _ref_v
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_logIt (senderId
	        ,
            _ref_v
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void LoggerActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void LoggerActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_entries, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_total, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_last, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_maxSeen, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void LoggerActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case LOGIT: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_LOGIT
	    long LoggerActor::msgsrvlogIt (
    		int
	         _ref_v
    ) {
    			long arrayIndexChecker = 0;SinkActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "logIt";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_entries=(_ref_entries+1));
			(_ref_total=(_ref_total+_ref_v));
			(_ref_last=_ref_v);
			if ((_ref_v>_ref_maxSeen)) {
				{
					(_ref_maxSeen=_ref_v);
				}
}
			(temp0=_ref_sk, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 212"), temp0)->_timed_msg_store(myID, _ref_total, _ref_now, MAX_TIME);
			assertion((_ref_maxSeen<=100));
			assertion((_ref_entries<=35));
		}
	_RETURN_POINT_LOGIT:
		return 0;

}
