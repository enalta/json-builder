# JSON Builder C lib

This lib can be used to easily build JSON documents on embedded systems. The footprint of the binary is as low as 2 kB and is not dependent on external libs.

### Build instructions

The lib is provided as an Eclipse project. Just import it into Eclipse, add it as a reference to your project and configure it to use your toolchain. Alternatively just copy the files **json-builder.h** and **json-builder.c** to your project directory, include the lib on the file where it will be used and add the lib files to the compilation process.

### Usage example

```c
#include <stdio.h>
#include <string.h>
#include "json-builder.h"

int main(){  
	char numberBuffer[50];

  // Declare and clean the document
	Serialized_Document document;  
	cleanDocument(&document);

	// Open the root document
	startNewDocument(NULL, &document);
	// Add some elements to it
	insertString(&document, "hello", "world");
	sprintf(numberBuffer, "%d", 1);
	insertNumber(&document, "integer", numberBuffer);
	sprintf(numberBuffer, "%.1f", -1.9);
	insertNumber(&document, "float", "-1.9e1");
	// Start a child document
	startNewDocument("child", &document);
	// Add some elements to it, than close
	insertString(&document, "foo", "bar");
	closeDocument(&document);
	// Start an Array add some elements, than close
	startNewArray(&document, "arrayTest");
	insertString(&document, NULL, "String inside array");
	insertString(&document, NULL, "Another string");
	insertNumber(&document, NULL, "1234");
	closeArray(&document);
	// Add some more elements to the root document
	insertTrue(&document, "true");
	insertFalse(&document, "false");
	insertNull(&document, "null");
	startNewDocument("child", &document);
	insertNumber(&document, "integer", "23235452");
	// Close all remaining documents
	closeDocument(&document);
	closeDocument(&document);

	puts(document.buffer);

	return 0;

}


```

### Splitting the lib

If you are not going to use all the features of a JSON document, you can edit the file **config.h** and select what parts of the lib you want to use. In the following example we are going to configure the lib to build only the String representation of data, without using **string.h** and **stdint.h**:

```c
#ifndef _H_CONFIG_
#define _H_CONFIG_

//#define USE_STRING_H_
//#define USE_STDINT_H_


#define USE_STRING
//#define USE_NUMBER
#ifdef USE_NUMBER
#define CHECK_NUMBER_VALIDITY
#endif
//#define USE_BOOLEAN
//#define USE_NULL
//#define USE_ARRAY

#define DOCUMENT_BUFFER_MAX_SIZE 256
#define ARRAY_BUFFER_MAX_SIZE 256
#define DOCUMENT_MAX_STACK_SIZE 16

#endif


```

With this configuration, the lib compiled against the x86_64 architecture will have a footprint of just 2.8kB.
