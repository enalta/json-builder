/**
 * @file config.h
 * @author Fernando Corrêa Santos
 * @date 30 oct 2015
 * @brief Use this file to configure the lib. You can comment defines that references data types you are not going to use, and define the sizes of the buffers.
 */

#ifndef _H_CONFIG_
#define _H_CONFIG_

/**
 * If your project include string.h somewhere, leave this uncommented
 */
#define USE_STRING_H_
/**
 * if your project uses stdint.h somewhere, leave this uncommented
 */
#define USE_STDINT_H_

/**
 * Keep this uncommented if you are going to use Strings.
 */
#define USE_STRING
/**
 * Keep this uncommented if you are going to use Numbers.
 */
#define USE_NUMBER
#ifdef USE_NUMBER
/**
 * If you are sure that the numbers that you'll insert on the document are in compliance with the JSON specification, you can commented this.
 */
#define CHECK_NUMBER_VALIDITY
#endif

/**
 * Keep this uncommented if you are going to use Booleans.
 */
#define USE_BOOLEAN
/**
 * Keep this uncommented if you are going to use Null Values.
 */
#define USE_NULL
/**
 * Keep this uncommented if you are going to use Arrays.
 */
#define USE_ARRAY

/**
 * Define the size of the buffer that will hold the Document.
 */
#define DOCUMENT_BUFFER_MAX_SIZE 256
/**
 * Define the size of the Array buffer. Used when building arrays before inserting them into the document.
 */
#define ARRAY_BUFFER_MAX_SIZE 256
/**
 * Define the max number of nested Documents and Arrays.
 */
#define DOCUMENT_MAX_STACK_SIZE 16

#endif
