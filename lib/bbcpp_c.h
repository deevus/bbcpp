#ifndef BBCPP_C_H
#define BBCPP_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* Opaque handles for C interface */
typedef struct bbcpp_document_t* bbcpp_document_handle;
typedef struct bbcpp_node_t* bbcpp_node_handle;

/* Node types */
typedef enum {
    BBCPP_NODE_DOCUMENT = 0,
    BBCPP_NODE_ELEMENT = 1,
    BBCPP_NODE_TEXT = 2,
    BBCPP_NODE_ATTRIBUTE = 3
} bbcpp_node_type;

/* Element types */
typedef enum {
    BBCPP_ELEMENT_SIMPLE = 0,
    BBCPP_ELEMENT_VALUE = 1,
    BBCPP_ELEMENT_PARAMETER = 2,
    BBCPP_ELEMENT_CLOSING = 3
} bbcpp_element_type;

/* Error codes */
typedef enum {
    BBCPP_SUCCESS = 0,
    BBCPP_ERROR_NULL_POINTER = -1,
    BBCPP_ERROR_INVALID_ARGUMENT = -2,
    BBCPP_ERROR_BUFFER_TOO_SMALL = -3,
    BBCPP_ERROR_OUT_OF_MEMORY = -4,
    BBCPP_ERROR_PARSE_ERROR = -5,
    BBCPP_ERROR_NOT_FOUND = -6
} bbcpp_error;

/* Document functions */
bbcpp_document_handle bbcpp_document_create(void);
void bbcpp_document_destroy(bbcpp_document_handle doc);
bbcpp_error bbcpp_document_load(bbcpp_document_handle doc, const char* bbcode);
bbcpp_error bbcpp_document_get_children_count(bbcpp_document_handle doc, size_t* count);
bbcpp_error bbcpp_document_get_child(bbcpp_document_handle doc, size_t index, bbcpp_node_handle* node);
bbcpp_error bbcpp_document_print(bbcpp_document_handle doc);

/* Node functions */
bbcpp_error bbcpp_node_get_type(bbcpp_node_handle node, bbcpp_node_type* type);
bbcpp_error bbcpp_node_get_name(bbcpp_node_handle node, char* buffer, size_t buffer_size, size_t* name_length);
bbcpp_error bbcpp_node_get_children_count(bbcpp_node_handle node, size_t* count);
bbcpp_error bbcpp_node_get_child(bbcpp_node_handle node, size_t index, bbcpp_node_handle* child);
bbcpp_error bbcpp_node_get_parent(bbcpp_node_handle node, bbcpp_node_handle* parent);

/* Text node functions */
bbcpp_error bbcpp_text_get_content(bbcpp_node_handle node, char* buffer, size_t buffer_size, size_t* content_length);

/* Element node functions */
bbcpp_error bbcpp_element_get_type(bbcpp_node_handle node, bbcpp_element_type* type);
bbcpp_error bbcpp_element_get_parameter_count(bbcpp_node_handle node, size_t* count);
bbcpp_error bbcpp_element_get_parameter_by_index(bbcpp_node_handle node, size_t index, 
                                                 char* key_buffer, size_t key_buffer_size, size_t* key_length,
                                                 char* value_buffer, size_t value_buffer_size, size_t* value_length);
bbcpp_error bbcpp_element_get_parameter(bbcpp_node_handle node, const char* key,
                                        char* value_buffer, size_t value_buffer_size, size_t* value_length);
bbcpp_error bbcpp_element_has_parameter(bbcpp_node_handle node, const char* key, int* has_parameter);

/* Utility functions */
bbcpp_error bbcpp_get_raw_string(bbcpp_node_handle node, char* buffer, size_t buffer_size, size_t* content_length);
const char* bbcpp_error_string(bbcpp_error error);

/* Helper macros for buffer size checking */
#define BBCPP_BUFFER_SIZE_CHECK(required, available) \
    ((required) <= (available) ? BBCPP_SUCCESS : BBCPP_ERROR_BUFFER_TOO_SMALL)

#ifdef __cplusplus
}
#endif

#endif /* BBCPP_C_H */