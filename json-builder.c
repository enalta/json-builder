#include "json-builder.h"

#ifndef USE_STRING_H_

size_t stringLen(char* input){
	int i = 0;

	while(input[i]){
		i++;
	}

	return i;
}


void* buffercpy(void* destination, const void* source, int num){
	int i;

	for(i = 0; i < num; i++) ((char*)destination)[i] = ((char*)source)[i];

	return destination;
}
#endif

#ifdef USE_NUMBER
#ifdef CHECK_NUMBER_VALIDITY
typedef enum{
	START,
	INTEGER,
	NEGATIVE,
	ZERO,
	FLOAT,
	DECIMAL,
	EXPONENT,
	EXPONENT_SIGNAL,
	EXPONENT_NUMBER,
	FAIL
}NumberCheckState;

int checkNumber(char* asciiNumber){
	int i = 0;
	NumberCheckState state = START;


	while (asciiNumber[i] != 0){
		switch(state){
		case START:
			if (asciiNumber[i] == '-') state = NEGATIVE;
			else if(asciiNumber[i] >= '1' && asciiNumber[i] <= '9') state = INTEGER;
			else if(asciiNumber[i] == '0') state = ZERO;
			else state = FAIL;
			break;
		case INTEGER:
			if(asciiNumber[i] < '0' || asciiNumber[i] > '9'){
				if (asciiNumber[i] == '.') state = FLOAT;
				else if (asciiNumber[i] == 'e' || asciiNumber[i] == 'E') state = EXPONENT;
				else state = FAIL;
			}
			break;
		case NEGATIVE:
			if(asciiNumber[i] >= '1' && asciiNumber[i] <= '9') state = INTEGER;
			else if(asciiNumber[i] == '0') state = ZERO;
			else state = FAIL;
			break;
		case ZERO:
			if (asciiNumber[i] == '.') state = FLOAT;
			else if (asciiNumber[i] == 'e' || asciiNumber[i] == 'E') state = EXPONENT;
			else state = FAIL;
			break;
		case FLOAT:
			if (asciiNumber[i] >= '0' && asciiNumber[i] <= '9') state = DECIMAL;
			else state = FAIL;
			break;
		case DECIMAL:
			if(asciiNumber[i] < '0' || asciiNumber[i] > '9'){
				if (asciiNumber[i] == 'e' || asciiNumber[i] == 'E') state = EXPONENT;
				else state = FAIL;
			}
			break;
		case EXPONENT:
			if(asciiNumber[i] == '+' || asciiNumber[i] == '-') state = EXPONENT_SIGNAL;
			else if(asciiNumber[i] >= '0' && asciiNumber[i] <= '9') state = EXPONENT_NUMBER;
			else state = FAIL;
			break;
		case EXPONENT_SIGNAL:
			if(asciiNumber[i] >= '0' && asciiNumber[i] <= '9') state = EXPONENT_NUMBER;
			else state = FAIL;
			break;
		case EXPONENT_NUMBER:
			if(asciiNumber[i] < '0' || asciiNumber[i] > '9') state = FAIL;
			break;
		default:
			return 0;
		}
		if (state != FAIL) i++;
	}
	return (state != START ? 1 : 0);
}
#endif
#endif

void cleanDocument(Serialized_Document* document){
	document->size = 0;
	document->stackSize = 0;
	document->buffer[0] = '\0';
}

uint32_t startNewDocument(Serialized_Document* document, char* key){
	int keylen;
	int addedSize;
	if (document->stackSize >= DOCUMENT_MAX_STACK_SIZE) return __JSON_BUILDER_FULL_STACK;
	if (key == NULL){
		if(document->stackSize > 0){
			if(document->stack[document->stackSize - 1] != JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
		}
	}
	else {
		if (document->stackSize == 0) return __JSON_BUILDER_INVALID_INSERTION;
		if(document->stack[document->stackSize - 1] == JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
		keylen = stringLen(key);
		addedSize = keylen + 4;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		document->buffer[document->size] = '"';
		document->size++;
		buffercpy(document->buffer + document->size, key, keylen);
		document->size += keylen;
		document->buffer[document->size] = '"';
		document->size++;
		document->buffer[document->size] = ':';
		document->size++;

	}
	document->stack[document->stackSize] = JSON_DOCUMENT;
	document->stackSize++;
	document->buffer[document->size] = '{';
	document->size++;
	return __JSON_BUILDER_SUCCESS;
}

#ifdef USE_ARRAY
uint32_t startNewArray(Serialized_Document* document, char* key){
	int keylen;
	int addedSize;
	if (document->stackSize >= DOCUMENT_MAX_STACK_SIZE) return __JSON_BUILDER_FULL_STACK;
	if (key == NULL && document->stack[document->stackSize -1] != JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
	if (key != NULL && document->stack[document->stackSize -1] == JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
	if (document->stackSize == 0) return __JSON_BUILDER_INVALID_INSERTION;
	if(key != NULL){
		keylen = stringLen(key);
		addedSize = keylen + 4;
	}
	else addedSize = 1;
	if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
	if (key != NULL){
		document->buffer[document->size] = '"';
		document->size++;
		buffercpy(document->buffer + document->size, key, keylen);
		document->size += keylen;
		document->buffer[document->size] = '"';
		document->size++;
		document->buffer[document->size] = ':';
		document->size++;
	}
	document->stack[document->stackSize] = JSON_ARRAY;
	document->stackSize++;
	document->buffer[document->size] = '[';
	document->size++;
	return __JSON_BUILDER_SUCCESS;

}
#endif

#ifdef USE_STRING
uint32_t insertString(Serialized_Document* document, char* key, char* value){
	int keylen;
	int valuelen = stringLen(value);
	int addedSize;

	if (key == NULL && document->stack[document->stackSize - 1] == JSON_ARRAY){
		addedSize = valuelen + 3;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		document->buffer[document->size] = '"';
		document->size++;
		buffercpy(document->buffer + document->size, value, valuelen);
		document->size += valuelen;
		document->buffer[document->size] = '"';
		document->size++;
		document->buffer[document->size] = ',';
		document->size++;
		return 0;
	}
	if (key == NULL && document->stack[document->stackSize - 1] == JSON_DOCUMENT) return __JSON_BUILDER_INVALID_INSERTION;
	if (key != NULL && document->stack[document->stackSize - 1] == JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
	keylen = stringLen(key);
	addedSize = keylen + valuelen + 6;

	if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
	if(document->stackSize < 1) return __JSON_BUILDER_INVALID_INSERTION;

	document->buffer[document->size] = '"';
	document->size++;
	buffercpy(document->buffer + document->size, key, keylen);
	document->size += keylen;
	document->buffer[document->size] = '"';
	document->size++;
	document->buffer[document->size] = ':';
	document->size++;
	document->buffer[document->size] = '"';
	document->size++;
	buffercpy(document->buffer + document->size, value, valuelen);
	document->size += valuelen;
	document->buffer[document->size] = '"';
	document->size++;
	document->buffer[document->size] = ',';
	document->size++;

	return 0;
}
#endif
#ifdef USE_NUMBER
uint32_t insertNumber(Serialized_Document* document, char* key, char* asciiNumber){
	int keylen;
	int valuelen = stringLen(asciiNumber);
	int addedSize;
#ifdef CHECK_NUMBER_VALIDITY
	if (!checkNumber(asciiNumber)) return __JSON_BUILDER_INVALID_NUMBER;
#endif

	if (key == NULL && document->stack[document->stackSize - 1] == JSON_ARRAY){
		addedSize = valuelen + 1;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		buffercpy(document->buffer + document->size, asciiNumber, valuelen);
		document->size += valuelen;
		document->buffer[document->size] = ',';
		document->size++;
		return 0;
	}
	if (key == NULL && document->stack[document->stackSize - 1] == JSON_DOCUMENT) return __JSON_BUILDER_INVALID_INSERTION;
	if (key != NULL && document->stack[document->stackSize - 1] == JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;

	keylen = stringLen(key);
	addedSize = keylen + valuelen + 6;

	if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
	if(document->stackSize < 1) return __JSON_BUILDER_INVALID_INSERTION;

	document->buffer[document->size] = '"';
	document->size++;
	buffercpy(document->buffer + document->size, key, keylen);
	document->size += keylen;
	document->buffer[document->size] = '"';
	document->size++;
	document->buffer[document->size] = ':';
	document->size++;
	buffercpy(document->buffer + document->size, asciiNumber, valuelen);
	document->size += valuelen;
	document->buffer[document->size] = ',';
	document->size++;

	return __JSON_BUILDER_SUCCESS;
}
#endif
uint32_t closeDocument(Serialized_Document* document){
	if (document->stackSize < 1) return __JSON_BUILDER_NO_DOCUMENT;
	if (document->stack[document->stackSize - 1] != JSON_DOCUMENT) return __JSON_BUILDER_NOT_A_DOCUMENT;
	if (document->buffer[document->size - 1] == ',') document->buffer[document->size - 1] = '}';
	else if (document->buffer[document->size - 1] == '{'){
		if(document->size + 1 >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		document->buffer[document->size] = '}';
		document->size++;
	}
	document->stackSize--;
	if(document->stackSize == 0) document->buffer[document->size] = '\0';
	else {
		document->buffer[document->size] = ',';
		document->size++;
	}
	return __JSON_BUILDER_SUCCESS;
}
#ifdef USE_ARRAY
uint32_t closeArray(Serialized_Document* document){
	if (document->stackSize < 2) return __JSON_BUILDER_NO_ARRAY;
	if (document->stack[document->stackSize - 1] != JSON_ARRAY) return __JSON_BUILDER_NOT_AN_ARRAY;
	if (document->buffer[document->size - 1] == ',') document->buffer[document->size - 1] = ']';
	else if (document->buffer[document->size - 1] == '['){
		if(document->size + 1 >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		document->buffer[document->size] = ']';
		document->size++;
	}
	document->stackSize--;
	document->buffer[document->size] = ',';
	document->size++;
	return __JSON_BUILDER_SUCCESS;
}
#endif

#ifdef USE_BOOLEAN
uint32_t insertTrue(Serialized_Document* document, char* key){
	uint32_t addedSize;
	uint32_t keylen;

	if(key == NULL && document->stack[document->stackSize - 1] != JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
	if(key != NULL && document->stack[document->stackSize - 1] == JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;

	if (key != NULL){
		keylen = stringLen(key);
		addedSize = keylen + 8;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		document->buffer[document->size] = '"';
		document->size++;
		buffercpy(document->buffer + document->size, key, keylen);
		document->size += keylen;
		document->buffer[document->size] = '"';
		document->size++;
		document->buffer[document->size] = ':';
		document->size++;
	}
	else {
		addedSize = 5;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
	}
	buffercpy(document->buffer + document->size, "true", 4);
	document->size += 4;
	document->buffer[document->size] = ',';
	document->size++;
	return 0;

}

uint32_t insertFalse(Serialized_Document* document, char* key){
	uint32_t addedSize;
	uint32_t keylen;

	if(key == NULL && document->stack[document->stackSize - 1] != JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
	if(key != NULL && document->stack[document->stackSize - 1] == JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;

	if (key != NULL){
		keylen = stringLen(key);
		addedSize = keylen + 9;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		document->buffer[document->size] = '"';
		document->size++;
		buffercpy(document->buffer + document->size, key, keylen);
		document->size += keylen;
		document->buffer[document->size] = '"';
		document->size++;
		document->buffer[document->size] = ':';
		document->size++;
	}
	else {
		addedSize = 6;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
	}
	buffercpy(document->buffer + document->size, "false", 5);
	document->size += 5;
	document->buffer[document->size] = ',';
	document->size++;
	return 0;

}
#endif
#ifdef USE_NULL
uint32_t insertNull(Serialized_Document* document, char* key){
	uint32_t addedSize;
	uint32_t keylen;

	if(key == NULL && document->stack[document->stackSize - 1] != JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;
	if(key != NULL && document->stack[document->stackSize - 1] == JSON_ARRAY) return __JSON_BUILDER_INVALID_INSERTION;

	if (key != NULL){
		keylen = stringLen(key);
		addedSize = keylen + 8;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
		document->buffer[document->size] = '"';
		document->size++;
		buffercpy(document->buffer + document->size, key, keylen);
		document->size += keylen;
		document->buffer[document->size] = '"';
		document->size++;
		document->buffer[document->size] = ':';
		document->size++;
	}
	else {
		addedSize = 5;
		if(document->size + addedSize >= DOCUMENT_BUFFER_MAX_SIZE) return __JSON_BUILDER_BUFFER_OVERFLOW;
	}
	buffercpy(document->buffer + document->size, "null", 4);
	document->size += 4;
	document->buffer[document->size] = ',';
	document->size++;
	return 0;

}
#endif
