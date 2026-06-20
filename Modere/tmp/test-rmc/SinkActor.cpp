                                    


#include "Types.h"
#include "SinkActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "ProducerActor.h"
#include "FilterAActor.h"
#include "FilterBActor.h"
#include "MergerActor.h"
#include "AnalyzerActor.h"
#include "CheckerActor.h"
#include "LoggerActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define STORE 1


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<SinkActor*>(rebecs[(int)((unsigned char)myID)]))

    
SinkActor::SinkActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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

	
		
	
		
       		 messageNames[1] = (char*)"STORE";
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

SinkActor::~SinkActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void SinkActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case STORE: {

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "store("
                	<< ((int)_ref_v)
                << ")";
                break;
               }
    	}
}


void SinkActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void SinkActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"Sink.a\" type=\"int\">" <<  
			    		((int)_ref_a) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Sink.b\" type=\"int\">" <<  
			    		((int)_ref_b) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Sink.c\" type=\"int\">" <<  
			    		((int)_ref_c) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Sink.d\" type=\"int\">" <<  
			    		((int)_ref_d) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Sink.e\" type=\"int\">" <<  
			    		((int)_ref_e) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Sink.writes\" type=\"int\">" <<  
			    		((int)_ref_writes) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Sink.total\" type=\"int\">" <<  
			    		((int)_ref_total) << "</variable>" << endl;
}


char* SinkActor::getClassName() {
    return (char*)"Sink";
}
void SinkActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_a, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_b, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_c, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_d, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_e, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_writes, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_total, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void SinkActor::marshalActorToArray(byte* array) {
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

void SinkActor::unmarshalActorFromArray(byte* array) {

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

void SinkActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_a, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_b, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_c, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_d, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_e, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_writes, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_total, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void SinkActor::printStateContent(long stateNo) {
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
long SinkActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case STORE:
                {
                    int i = 0;

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvstore(
            _ref_v
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long SinkActor::constructorSink (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Sink";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_a=0);
			(_ref_b=0);
			(_ref_c=0);
			(_ref_d=0);
			(_ref_e=0);
			(_ref_writes=0);
			(_ref_total=0);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void SinkActor::_msg_store (byte senderId
	        ,
    		int
	         _ref_v
  	  ){
        queueTail = 0;
        enqueue(messageQueue, STORE);
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









	byte** SinkActor::table = (byte**)0xFFFFFFFF;

	byte** SinkActor::getTable() {
		return table;
	}








	vector<TimeFrame>** SinkActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** SinkActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void SinkActor::_timed_msg_store (byte senderId
	        ,
    		int
	         _ref_v
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_store (senderId
	        ,
            _ref_v
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void SinkActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void SinkActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_a, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_b, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_c, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_d, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_e, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_writes, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_total, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void SinkActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case STORE: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_STORE
	    long SinkActor::msgsrvstore (
    		int
	         _ref_v
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "store";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_writes=(_ref_writes+1));
			(_ref_total=(_ref_total+_ref_v));
			(_ref_a=_ref_b);
			(_ref_b=_ref_c);
			(_ref_c=_ref_d);
			(_ref_d=_ref_e);
			(_ref_e=_ref_v);
		}
	_RETURN_POINT_STORE:
		return 0;

}
