                                    


#include "Types.h"
#include "ProducerActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "FilterAActor.h"
#include "FilterBActor.h"
#include "MergerActor.h"
#include "AnalyzerActor.h"
#include "CheckerActor.h"
#include "LoggerActor.h"
#include "SinkActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define PRODUCE 1


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<ProducerActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_fa (dynamic_cast<FilterAActor*>(rebecs[(int)((unsigned char)known[1])]))
    	#define _ref_fb (dynamic_cast<FilterBActor*>(rebecs[(int)((unsigned char)known[2])]))
    
ProducerActor::ProducerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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

	
		
	
		
       		 messageNames[1] = (char*)"PRODUCE";
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

ProducerActor::~ProducerActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void ProducerActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case PRODUCE: {
                out << "produce("
                << ")";
                break;
               }
    	}
}


void ProducerActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void ProducerActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"Producer.sent\" type=\"int\">" <<  
			    		((int)_ref_sent) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Producer.mode\" type=\"int\">" <<  
			    		((int)_ref_mode) << "</variable>" << endl;
}


char* ProducerActor::getClassName() {
    return (char*)"Producer";
}
void ProducerActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_sent, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_mode, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void ProducerActor::marshalActorToArray(byte* array) {
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

void ProducerActor::unmarshalActorFromArray(byte* array) {

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

void ProducerActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_sent, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_mode, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void ProducerActor::printStateContent(long stateNo) {
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
long ProducerActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case PRODUCE:
                {
                    int i = 0;
                    return msgsrvproduce(
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long ProducerActor::constructorProducer (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;ProducerActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Producer";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_sent=0);
			(_ref_mode=0);
			(temp0=_ref_self, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 13"), temp0)->_timed_msg_produce(myID, _ref_now, MAX_TIME);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void ProducerActor::_msg_produce (byte senderId
  	  ){
        queueTail = 0;
        enqueue(messageQueue, PRODUCE);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** ProducerActor::table = (byte**)0xFFFFFFFF;

	byte** ProducerActor::getTable() {
		return table;
	}








	vector<TimeFrame>** ProducerActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** ProducerActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void ProducerActor::_timed_msg_produce (byte senderId
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_produce (senderId
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void ProducerActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void ProducerActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_sent, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_mode, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void ProducerActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case PRODUCE: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_PRODUCE
	    long ProducerActor::msgsrvproduce (
    ) {
    			long arrayIndexChecker = 0;FilterAActor *temp0;FilterBActor *temp1;ProducerActor *temp2;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "produce";
		#endif
		long __tNumber = 0;
		static byte nonDetVariable0 = 0;

	#ifdef TTS
	#endif

		{
			(_ref_sent=(_ref_sent+1));
			(_ref_mode=(__tNumber|=1, nonDetVariable0));
			if ((_ref_sent<=15)) {
				{
					(temp0=_ref_fa, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 19"), temp0)->_timed_msg_process(myID, _ref_sent, _ref_mode, _ref_now, MAX_TIME);
					(temp1=_ref_fb, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 20"), temp1)->_timed_msg_process(myID, _ref_sent, _ref_mode, _ref_now, MAX_TIME);
					(temp2=_ref_self, assertion(temp2!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 21"), temp2)->_timed_msg_produce(myID, _ref_now, MAX_TIME);
				}
}
		}
	_RETURN_POINT_PRODUCE:
		if (__tNumber & 1) {
			if (false){/*Dummy 'if' statement to create nested else-if easily!*/}
			else if(nonDetVariable0 == 0) {
				nonDetVariable0 = 1;
			}
			else if(nonDetVariable0 == 1) {
				nonDetVariable0 = 2;
			}
			else if(nonDetVariable0 == 2) {
				nonDetVariable0 = 0;
				__tNumber &= ~1;
			}
			if (__tNumber & 1) {
				return __tNumber;
			}
		}
		return 0;

}
