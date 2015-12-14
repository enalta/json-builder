/**
 * @file json_builder.h
 * @author Fernando Corrêa Santos
 * @date 30 oct 2015
 * @brief Library used to serialize data in JSON format
 */

#ifndef _H_JSON_BUILDER_
#define _H_JSON_BUILDER_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#ifdef USE_STRING_H_
#include <string.h>
#define stringLen(X) strlen(X);
#define buffercpy(X, Y, Z) memcpy(X, Y, Z)
#else
typedef unsigned int size_t;
#define NULL (void*) 0
#endif

#ifdef USE_STDINT_H_
#include <stdint.h>
#else
typedef unsigned long int uint32_t;
#endif

#define __JSON_BUILDER_SUCCESS 0
#define __JSON_BUILDER_BUFFER_OVERFLOW -1
#define __JSON_BUILDER_INVALID_NAME -2
#define __JSON_BUILDER_INVALID_INSERTION -3
#define __JSON_BUILDER_INVALID_NUMBER -4
#define __JSON_BUILDER_NO_DOCUMENT -5
#define __JSON_BUILDER_NOT_A_DOCUMENT -6
#define __JSON_BUILDER_NO_ARRAY -7
#define __JSON_BUILDER_NOT_AN_ARRAY -8
#define __JSON_BUILDER_FULL_STACK -9

typedef enum{
	JSON_DOCUMENT,
	JSON_ARRAY
}StackAlphabet;

typedef StackAlphabet Stack[DOCUMENT_MAX_STACK_SIZE];

/**
 * Struct that holds the document in a serialized form.
 */
typedef struct{
	char buffer[DOCUMENT_BUFFER_MAX_SIZE]; /**< Buffer that holds the document. */
	uint32_t size; /**< Size of the document. */
	uint32_t stackSize; /**< Current size of the stack. */
	Stack stack; /**< Stack used to control the insertion of nested documents and arrays. */
}Serialized_Document;

/**
 * Set the stackSize and size fields of a Serialized_Documento to 0. Also set the first byte of the Serialized_Document buffer to 0x00, so it's can be interpreted as an empty string.
 * @param document A pointer to the document to be cleaned;
 */
void cleanDocument(Serialized_Document* document);

/**
 * If the Serialized_Document is empty, start a root document. If not, create a embedded document.
 * @param document The Serialized_Document to be built.
 * @param key The key of the key-value pair. If it's the root document or an array, must be NULL.
 * @return __JSON_BUILDER_FULL_STACK If the Document Stack is full.
 * @return __JSON_BUILDER_INVALID_INSERTION If you try to insert a new Document with a key to a clean Document or to an Array.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW if the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t startNewDocument(Serialized_Document* document, char* key);
#ifdef USE_ARRAY
/**
 * Starts a new array inside a Serialized_Document. The document must be already started.
 * @param document The Serialized_Document to receive the new array.
 * @param key The key of the key-value pair. If the Array is inside another array, must be NULL.
 * @return __JSON_BUILDER_FULL_STACK If the Document Stack is full.
 * @return __JSON_BUILDER_INVALID_INSERTION If you try to start a new Array with a key to another array, or a clean Document, or a new Array without a key inside a Document.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW if the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t startNewArray(Serialized_Document* document, char* key);
#endif
#ifdef USE_STRING
/**
 * Insert a string in the Serialized_Document.
 * @param document The document that will receive the string.
 * @param key The key to the key-value pair. If it's inside an array, must be NULL.
 * @param value The string to be inserted. It's also the value of the key-value pair.
 * @return __JSON_BUILDER_INVALID_INSERTION If you try to insert a String with a key on an Array, or without a Key on a Document.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW If the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t insertString(Serialized_Document* document, char* key, char* value);
#endif
#ifdef USE_NUMBER
/**
 * Insert a number in the Serialized_Document. The number format must follow the JSON specification.
 * @see http://json.org/
 * @param document The document that will receive the number.
 * @param key The key to the key-value pair. If it's inside an array, must be NULL.
 * @param asciiNumber The string containing a ascii representation of the number.
 * @return __JSON_BUILDER_INVALID_NUMBER If the number inserted is not compliant with the JSON specification and the CHECK_NUMBER_VALIDITY define is uncommented
 * @return __JSON_BUILDER_BUFFER_OVERFLOW If the Document buffer is full.
 * @return __JSON_BUILDER_INVALID_INSERTION If you try to insert a Number with a key on an Array, or without a Key on a Document.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t insertNumber(Serialized_Document* document, char* key, char* asciiNumber);
#endif
#ifdef USE_BOOLEAN
/**
 * Insert a TRUE value in the Serialized_Document.
 * @param document The document that will receive the boolean.
 * @param key The key to the key-value pair. If it's inside an array, must be NULL.
 * @return __JSON_BUILDER_INVALID_INSERTION If you try to insert a True with a key on an Array, or without a Key on a Document.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW If the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t insertTrue(Serialized_Document* document, char* key);
/**
 * Insert a FALSE value in the Serialized_Document.
 * @param document The document that will receive the boolean.
 * @param key The key to the key-value pair. If it's inside an array, must be NULL.
 * @return __JSON_BUILDER_INVALID_INSERTION If you try to insert a False with a key on an Array, or without a Key on a Document.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW If the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t insertFalse(Serialized_Document* document, char* key);
#endif
#ifdef USE_NULL
/**
 * Insert a NULL value in the Serialized_Document.
 * @param document The document that will receive the NULL value.
 * @param key The key to the key-value pair. If it's inside an array, must be NULL.
 * @return __JSON_BUILDER_INVALID_INSERTION If you try to insert a Null with a key on an Array, or without a Key on a Document.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW If the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t insertNull(Serialized_Document* document, char* key);
#endif
//void insertDocument(Document* document, char* key, Document* child);
//void insertArray(Document* document, Array* child);
/**
 * Close the last openned Document of the Stack.
 * @param document The Serialized_document that contains the document to be closed.
 * @return __JSON_BUILDER_NO_DOCUMENT If the document stack is empty.
 * @return __JSON_BUILDER_NOT_A_DOCUMENT If the last item of the stack refers to an Array.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW If the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t closeDocument(Serialized_Document* document);
#ifdef USE_ARRAY
/**
 * Close the last openned Array of the Stack.
 * @param document The Serialized_document that contains the array to be closed.
 * @return __JSON_BUILDER_NO_ARRAY If the stack size is 1 or 0, it's impossible to have an open Array on the buffer.
 * @return __JSON_BUILDER_NOT_AN_ARRAY The stack size is greater than 1, but the last element on the stack is not an Array.
 * @return __JSON_BUILDER_BUFFER_OVERFLOW If the Document buffer is full.
 * @return __JSON_BUILDER_SUCCESS If success.
 */
uint32_t closeArray(Serialized_Document* document);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _H_JSON_BUILDER_ */
